#pragma once

#include <engine/scene/Component.h>
#include <engine/scene/ScriptHooks.h>
#include <engine/scene/SystemOrder.h>
#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace eng {

class ScriptComponent;

class ScriptBehaviour {
public:
    virtual ~ScriptBehaviour() = default;
    Entity* Owner() const;
    EntityId OwnerId() const;
    Scene* GetScene() const;
    Transform2D* Transform() const;

private:
    friend class ScriptComponent;
    ScriptComponent* m_component = nullptr;
};

class ScriptRegistry {
public:
    using CreateFn = std::unique_ptr<ScriptBehaviour> (*)();

    struct Entry {
        CreateFn create = nullptr;
        ScriptHooks hooks;
        std::string sourceFile;
    };

    static void Register(std::string_view scriptName, CreateFn create, const ScriptHooks& hooks, std::string_view sourceFile);
    static bool IsRegistered(std::string_view scriptName);
    static const Entry* Find(std::string_view scriptName);
    static std::unique_ptr<ScriptBehaviour> Create(std::string_view scriptName);
    static void ForEachScript(const std::function<void(const char*)>& fn);
    static void ForEachEntry(const std::function<void(const char* name, const Entry& entry)>& fn);
    static std::size_t Count();
    static void Clear();
};

class ScriptComponent final : public Component {
public:
    static constexpr const char* kTypeName = "ScriptComponent";
    ~ScriptComponent() override;
    const char* TypeName() const override { return kTypeName; }
    bool Deserialize(const Json& node, std::string& outError) override;
    bool Serialize(Json& out) const override;
    void OnAttach() override;
    void OnDetach() override;
    const std::string& ScriptName() const { return m_scriptName; }
    void SetScriptName(std::string_view name);
    bool IsResolved() const { return m_behaviour != nullptr; }
    const ScriptHooks& Hooks() const { return m_hooks; }
    bool NeedsTick() const { return m_behaviour != nullptr && (!m_started || m_hooks.update != nullptr); }
    void Tick(float deltaSeconds);
    void DispatchCollision(const std::string& messageType, EntityId other);
    void UnbindForReload();
    void RebindAfterReload();

private:
    void Bind();
    void Unbind();
    std::string m_scriptName;
    std::unique_ptr<ScriptBehaviour> m_behaviour;
    ScriptHooks m_hooks;
    bool m_started = false;
};

class ScriptSystem final : public System {
public:
    void Update(float deltaSeconds) override;
    const char* Name() const override  { return "ScriptSystem"; }
    int Order() const override { return SystemStage::kGameplay; }
    static void Register(ScriptComponent& script);
    static void Unregister(ScriptComponent& script);
    static void Clear();
    static std::size_t Count();
    static std::size_t TickingCount();
    static std::size_t UnresolvedCount();
    static void UnbindAll();
    static void RebindAll();
    static std::size_t RebindRenamed(std::string_view oldName, std::string_view newName);
    static std::size_t CountUsing(std::string_view scriptName);
    static void RegisterComponentTypes();
    static void SubscribeToCollisions();
};

} // namespace eng
