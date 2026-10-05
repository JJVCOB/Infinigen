#include <engine/core/Log.h>
#include <engine/math/Transform2D.h>
#include <engine/scene/Messaging.h>
#include <engine/scene/Scene.h>
#include <engine/scene/ScriptComponent.h>
#include <algorithm>
#include <map>
#include <vector>

namespace eng {
namespace {

std::vector<ScriptComponent*> g_scripts;
std::vector<ScriptComponent*> g_ticking;
bool g_collisionsSubscribed = false;
using ScriptTable = std::map<std::string, ScriptRegistry::Entry>;

ScriptTable& Table() {
    static ScriptTable table;
    return table;
}

void AddToTicking(ScriptComponent* script) {
    if (script->NeedsTick() &&
        std::find(g_ticking.begin(), g_ticking.end(), script) == g_ticking.end()) {
        g_ticking.push_back(script);
    }
}

} // namespace

Entity* ScriptBehaviour::Owner() const {
    return m_component != nullptr ? m_component->Owner() : nullptr;
}

EntityId ScriptBehaviour::OwnerId() const {
    return m_component != nullptr ? m_component->OwnerId() : EntityId{};
}

Scene* ScriptBehaviour::GetScene() const {
    return m_component != nullptr ? m_component->GetScene() : nullptr;
}

Transform2D* ScriptBehaviour::Transform() const {
    return m_component != nullptr ? m_component->OwnerTransform() : nullptr;
}

std::string DescribeHooks(const ScriptHooks& hooks) {
    std::string out;
    const auto add = [&out](const char* name) {
        if (!out.empty()) {
            out += ", ";
        }
        out += name;
    };

    if (hooks.start != nullptr) {
        add("OnStart");
    }
    if (hooks.update != nullptr) {
        add("OnUpdate");
    }
    if (hooks.destroy != nullptr) {
        add("OnDestroy");
    }
    if (hooks.collisionEnter != nullptr) {
        add("OnCollisionEnter");
    }
    if (hooks.collisionStay != nullptr) {
        add("OnCollisionStay");
    }
    if (hooks.collisionExit != nullptr) {
        add("OnCollisionExit");
    }

    if (out.empty()) {
        out = "NO HOOKS - check the spelling of OnStart / OnUpdate";
    }
    return out;
}

void ScriptRegistry::Register(std::string_view scriptName, CreateFn create,
                              const ScriptHooks& hooks, std::string_view sourceFile) {
    if (scriptName.empty() || create == nullptr) {
        return;
    }

    ScriptTable& table = Table();
    const std::string name(scriptName);

    if (table.contains(name)) {
        const Entry& already = table.at(name);

        if (already.sourceFile == sourceFile) {
            return;
        }

        ENGINE_LOG_WARN(Channels::kScene,
                        "two different files both define a script called '{}' ('{}' and "
                        "'{}') - only the first can run, so rename one of them",
                        scriptName, already.sourceFile, sourceFile);
        return;
    }

    Entry entry;
    entry.create = create;
    entry.hooks = hooks;
    entry.sourceFile = std::string(sourceFile);
    table[name] = entry;
}

bool ScriptRegistry::IsRegistered(std::string_view scriptName) {
    return Table().contains(std::string(scriptName));
}

const ScriptRegistry::Entry* ScriptRegistry::Find(std::string_view scriptName) {
    const std::string name(scriptName);
    if (Table().contains(name)) {
        return &Table().at(name);
    }
    return nullptr;
}

std::unique_ptr<ScriptBehaviour> ScriptRegistry::Create(std::string_view scriptName) {
    const Entry* entry = Find(scriptName);
    return (entry != nullptr) ? entry->create() : nullptr;
}

void ScriptRegistry::ForEachScript(const std::function<void(const char*)>& fn) {
    for (const auto& [name, entry] : Table()) {
        fn(name.c_str());
    }
}

void ScriptRegistry::ForEachEntry(
    const std::function<void(const char* name, const Entry& entry)>& fn) {
    for (const auto& [name, entry] : Table()) {
        fn(name.c_str(), entry);
    }
}

std::size_t ScriptRegistry::Count() {
    return Table().size();
}

void ScriptRegistry::Clear() {
    Table().clear();
}

ScriptComponent::~ScriptComponent() {
    ScriptSystem::Unregister(*this);
    Unbind();
}

bool ScriptComponent::Deserialize(const Json& node, std::string& outError) {
    m_scriptName = ReadString(node, "script", "", kTypeName);
    if (m_scriptName.empty()) {
        outError = "ScriptComponent needs a \"script\" naming the behaviour to run";
        return false;
    }
    return true;
}

bool ScriptComponent::Serialize(Json& out) const {
    out["script"] = m_scriptName;
    return true;
}

void ScriptComponent::OnAttach() {
    Bind();
    ScriptSystem::Register(*this);
}

void ScriptComponent::OnDetach() {
    if (m_behaviour != nullptr && m_started && m_hooks.destroy != nullptr) {
        m_hooks.destroy(m_behaviour.get());
    }
    ScriptSystem::Unregister(*this);
    Unbind();
}

void ScriptComponent::SetScriptName(std::string_view name) {
    if (m_scriptName == name) {
        return;
    }
    if (m_behaviour != nullptr && m_started && m_hooks.destroy != nullptr) {
        m_hooks.destroy(m_behaviour.get());
    }
    Unbind();
    m_scriptName = std::string(name);
    m_started = false;
    Bind();

    ScriptSystem::Register(*this);
}

void ScriptComponent::UnbindForReload() {
    if (m_behaviour != nullptr && m_started && m_hooks.destroy != nullptr) {
        m_hooks.destroy(m_behaviour.get());
    }
    Unbind();

    m_started = false;
}

void ScriptComponent::RebindAfterReload() {
    Bind();
}

void ScriptComponent::Bind() {
    m_hooks = ScriptHooks{};

    if (const ScriptRegistry::Entry* entry = ScriptRegistry::Find(m_scriptName); entry != nullptr) {
        m_behaviour = entry->create();
        if (m_behaviour != nullptr) {
            m_behaviour->m_component = this;
            m_hooks = entry->hooks;
            return;
        }
    }

    if (!m_scriptName.empty()) {
        ENGINE_LOG_WARN(Channels::kScene,
                        "the script '{}' is not compiled into this build, so it is "
                        "attached but will not run ({} script(s) available)",
                        m_scriptName, ScriptRegistry::Count());
    }
}

void ScriptComponent::Unbind() {
    if (m_behaviour != nullptr) {
        m_behaviour->m_component = nullptr;
        m_behaviour.reset();
    }
    m_hooks = ScriptHooks{};
}

void ScriptComponent::Tick(float deltaSeconds) {
    if (m_behaviour == nullptr) {
        return;
    }

    if (!m_started) {
        m_started = true;
        if (m_hooks.start != nullptr) {
            m_hooks.start(m_behaviour.get());
        }
    }

    if (m_hooks.update != nullptr) {
        m_hooks.update(m_behaviour.get(), deltaSeconds);
    }
}

void ScriptComponent::DispatchCollision(const std::string& messageType, EntityId other) {
    if (m_behaviour == nullptr || !m_started || !m_hooks.AnyCollision()) {
        return;
    }

    if (messageType == MessageTypes::kCollisionEnter) {
        if (m_hooks.collisionEnter != nullptr) {
            m_hooks.collisionEnter(m_behaviour.get(), other);
        }
    } else if (messageType == MessageTypes::kCollisionStay) {
        if (m_hooks.collisionStay != nullptr) {
            m_hooks.collisionStay(m_behaviour.get(), other);
        }
    } else if (messageType == MessageTypes::kCollisionExit) {
        if (m_hooks.collisionExit != nullptr) {
            m_hooks.collisionExit(m_behaviour.get(), other);
        }
    }
}

void ScriptSystem::Register(ScriptComponent& script) {
    if (std::find(g_scripts.begin(), g_scripts.end(), &script) == g_scripts.end()) {
        g_scripts.push_back(&script);
    }
    AddToTicking(&script);
}

void ScriptSystem::Unregister(ScriptComponent& script) {
    std::erase(g_scripts, &script);
    std::erase(g_ticking, &script);
}

void ScriptSystem::Clear() {
    g_scripts.clear();
    g_ticking.clear();
}
std::size_t ScriptSystem::Count() {
    return g_scripts.size();
}
std::size_t ScriptSystem::TickingCount() {
    return g_ticking.size();
}

std::size_t ScriptSystem::UnresolvedCount() {
    std::size_t count = 0;
    for (const ScriptComponent* script : g_scripts) {
        if (!script->IsResolved()) {
            ++count;
        }
    }
    return count;
}

std::size_t ScriptSystem::CountUsing(std::string_view scriptName) {
    std::size_t count = 0;
    for (const ScriptComponent* script : g_scripts) {
        if (script->ScriptName() == scriptName) {
            ++count;
        }
    }
    return count;
}

std::size_t ScriptSystem::RebindRenamed(std::string_view oldName, std::string_view newName) {
    if (oldName.empty() || newName.empty() || oldName == newName) {
        return 0;
    }

    std::size_t moved = 0;
    for (ScriptComponent* script : std::vector<ScriptComponent*>(g_scripts)) {
        if (script->ScriptName() == oldName) {
            script->SetScriptName(newName);
            ++moved;
        }
    }
    return moved;
}

void ScriptSystem::UnbindAll() {
    for (ScriptComponent* script : std::vector<ScriptComponent*>(g_scripts)) {
        script->UnbindForReload();
    }
    g_ticking.clear();
}

void ScriptSystem::RebindAll() {
    for (ScriptComponent* script : std::vector<ScriptComponent*>(g_scripts)) {
        script->RebindAfterReload();
        AddToTicking(script);
    }
}

void ScriptSystem::Update(float deltaSeconds) {
    for (std::size_t i = 0; i < g_ticking.size(); ++i) {
        g_ticking[i]->Tick(deltaSeconds);
    }

    std::erase_if(g_ticking, [](const ScriptComponent* script) { return !script->NeedsTick(); });
}

void ScriptSystem::SubscribeToCollisions() {
    if (g_collisionsSubscribed) {
        return;
    }
    g_collisionsSubscribed = true;

    const auto forward = [](const Message& message) {
        Scene* scene = Scene::Active();
        if (scene == nullptr) {
            return;
        }
        Entity* entity = scene->Get(message.target);
        if (entity == nullptr) {
            return;
        }
        if (auto* script = entity->Find<ScriptComponent>(); script != nullptr) {
            script->DispatchCollision(message.type, message.other);
        }
    };

    MessageBus::SubscribeBroadcast(MessageTypes::kCollisionEnter, forward);
    MessageBus::SubscribeBroadcast(MessageTypes::kCollisionStay, forward);
    MessageBus::SubscribeBroadcast(MessageTypes::kCollisionExit, forward);
}

void ScriptSystem::RegisterComponentTypes() {
    ComponentFactory::Register(ScriptComponent::kTypeName, []() -> std::unique_ptr<Component> {
        return std::make_unique<ScriptComponent>();
    });
}

} // namespace eng