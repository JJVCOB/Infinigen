#include <engine/core/Log.h>
#include <engine/fs/FileSystem.h>
#include <engine/render/Gizmos.h>
#include <engine/resource/ResourceManager.h>
#include <engine/scene/Component.h>
#include <engine/scene/Scene.h>
#include <unordered_map>

namespace eng {
namespace {

Scene* g_active = nullptr;

const Json& Field(const Json& object, const char* name) {
    static const Json kEmpty = Json::object();
    if (!object.is_object()) {
        return kEmpty;
    }
    const auto it = object.find(name);
    return (it != object.end()) ? *it : kEmpty;
}

} // namespace

Scene::Scene() = default;

Scene::~Scene() {
    Unload();
    if (g_active == this) {
        g_active = nullptr;
    }
}

Scene* Scene::Active() {
    return g_active;
}
void Scene::SetActive(Scene* scene) {
    g_active = scene;
}

EntityId Scene::CreateEntity(std::string_view name) {
    int index = 0;

    if (!m_freeIndices.empty()) {
        index = m_freeIndices.back();
        m_freeIndices.pop_back();
    } else {
        index = static_cast<int>(m_slots.size());
        m_slots.emplace_back();
    }

    Slot& slot = m_slots[static_cast<std::size_t>(index)];
    slot.entity = std::make_unique<Entity>();
    slot.occupied = true;

    slot.entity->m_id = EntityId{index, slot.generation};
    slot.entity->m_scene = this;
    slot.entity->m_alive = true;
    slot.entity->SetName(name);

    m_byName[std::string(name)] = slot.entity->m_id;
    ++m_liveCount;

    return slot.entity->m_id;
}

void Scene::DestroyEntityImmediate(EntityId id) {
    if (!IsValid(id)) {
        return;
    }

    Slot& slot = m_slots[static_cast<std::size_t>(id.index)];

    if (slot.entity != nullptr) {
        m_byName.erase(slot.entity->Name());
        slot.entity->DestroyInternal();
    }
    slot.entity.reset();
    slot.occupied = false;

    ++slot.generation;

    m_freeIndices.push_back(id.index);
    if (m_liveCount > 0) {
        --m_liveCount;
    }
}

Entity* Scene::Get(EntityId id) {
    if (!IsValid(id)) {
        return nullptr;
    }
    return m_slots[static_cast<std::size_t>(id.index)].entity.get();
}

bool Scene::IsValid(EntityId id) const {
    if (id.IsNull() || id.index >= static_cast<int>(m_slots.size())) {
        return false;
    }
    const Slot& slot = m_slots[static_cast<std::size_t>(id.index)];

    return slot.occupied && slot.generation == id.generation;
}

EntityId Scene::Find(std::string_view name) const {
    const std::string key(name);
    if (m_byName.contains(key)) {
        return m_byName.at(key);
    }
    return EntityId{};
}

void Scene::ForEach(const std::function<void(Entity&)>& fn) {
    for (std::size_t i = 0; i < m_slots.size(); ++i) {
        Slot& slot = m_slots[i];
        if (slot.occupied && slot.entity != nullptr) {
            fn(*slot.entity);
        }
    }
}

EntityId Scene::CreateEntityFromJson(const Json& node, std::string_view nameOverride,
                                     std::string& outError) {
    std::string name(nameOverride);
    if (name.empty()) {
        name = ReadString(node, "name", "");
    }
    if (name.empty()) {
        outError = "an entity in this scene has no \"name\"";
        return EntityId{};
    }

    const EntityId id = CreateEntity(name);
    Entity* entity = Get(id);
    if (entity == nullptr) {
        outError = "could not create an entity called '" + name + "'";
        return EntityId{};
    }

    const Json& components = Field(node, "components");
    if (components.is_null() || !components.is_array()) {
        return id;
    }

    for (const Json& componentNode : components) {
        const std::string typeName = ReadString(componentNode, "type", "");
        if (typeName.empty()) {
            ENGINE_LOG_ERROR(Channels::kScene, "'{}': a component entry has no \"type\"", name);
            continue;
        }

        std::unique_ptr<Component> component = ComponentFactory::Create(typeName);
        if (component == nullptr) {
            ENGINE_LOG_ERROR(Channels::kScene, "'{}': there is no component type called '{}'", name,
                             typeName);
            continue;
        }

        std::string componentError;
        if (!component->Deserialize(componentNode, componentError)) {
            ENGINE_LOG_ERROR(Channels::kScene, "'{}' / {}: {}", name, typeName, componentError);
        }

        entity->AddComponent(std::move(component));
    }

    return id;
}

void Scene::ResolveParents(const Json& entitiesArray) {
    for (const Json& node : entitiesArray) {
        const std::string childName = ReadString(node, "name", "");
        const std::string parentName = ReadString(node, "parent", "");
        if (childName.empty() || parentName.empty()) {
            continue;
        }

        Entity* child = Get(Find(childName));
        Entity* parent = Get(Find(parentName));
        if (child == nullptr) {
            continue;
        }
        if (parent == nullptr) {
            ENGINE_LOG_ERROR(Channels::kScene,
                             "'{}' says its parent is '{}', but there is no entity with "
                             "that name in this scene",
                             childName, parentName);
            continue;
        }
        child->Transform().SetParent(&parent->Transform());
    }
}

bool Scene::BuildFromDocument(std::string& outError) {
    m_name = ReadString(m_document, "name", "<unnamed>");

    const Json& camera = Field(m_document, "camera");
    if (camera.is_object()) {
        m_cameraPosition = ReadVec2(camera, "position", Vec2{0.0f, 0.0f}, "camera");
        m_cameraZoom = ReadFloat(camera, "zoom", 1.0f, "camera");
    }

    const Json& entities = Field(m_document, "entities");
    if (!entities.is_array()) {
        outError = "this scene file has no \"entities\" list";
        ENGINE_LOG_ERROR(Channels::kScene, "{}", outError);
        return false;
    }

    for (const Json& node : entities) {
        std::string entityError;
        if (CreateEntityFromJson(node, {}, entityError).IsNull()) {
            ENGINE_LOG_ERROR(Channels::kScene, "{}", entityError);
        }
    }

    ResolveParents(entities);

    SetActive(this);
    outError.clear();
    return true;
}

bool Scene::Load(std::string_view virtualPath, std::string& outError) {
    Unload();

    std::string text;
    if (!FileSystem::ReadTextFile(virtualPath, text, outError)) {
        ENGINE_LOG_ERROR(Channels::kScene, "{}", outError);
        return false;
    }

    m_document = ParseJson(text, outError);
    if (!outError.empty()) {
        ENGINE_LOG_ERROR(Channels::kScene, "{}: {}", virtualPath, outError);
        return false;
    }

    m_sourcePath.assign(virtualPath);

    if (!BuildFromDocument(outError)) {
        return false;
    }

    ENGINE_LOG_INFO(Channels::kScene, "scene '{}' loaded from {}: {} entities, {} image(s)", m_name,
                    virtualPath, m_liveCount, ResourceManager::LoadedCount());
    return true;
}

namespace {

Json BuildSceneDocument(Scene& scene, const std::string& sceneName, Vec2 cameraPosition,
                        float cameraZoom, std::size_t& outSkipped, const Json& existing) {
    Json root = existing.is_object() ? existing : Json::object();

    root["name"] = sceneName.empty() ? std::string("Untitled") : sceneName;
    root["camera"]["position"] = Json::array({cameraPosition.x, cameraPosition.y});
    root["camera"]["zoom"] = cameraZoom;

    std::unordered_map<const Transform2D*, std::string> transformNames;
    scene.ForEach([&](Entity& entity) { transformNames[&entity.Transform()] = entity.Name(); });

    outSkipped = 0;
    Json entities = Json::array();

    scene.ForEach([&](Entity& entity) {
        Json entityJson = Json::object();
        entityJson["name"] = entity.Name();

        const Transform2D* parent = entity.Transform().Parent();
        if (parent != nullptr && transformNames.contains(parent)) {
            entityJson["parent"] = transformNames.at(parent);
        }

        Json components = Json::array();
        entity.ForEachComponent([&](Component& component) {
            Json componentJson = Json::object();
            if (!component.Serialize(componentJson)) {
                ENGINE_LOG_WARN(Channels::kScene,
                                "saving '{}': the component '{}' cannot be saved and was "
                                "left out of the file",
                                entity.Name(), component.TypeName());
                ++outSkipped;
                return;
            }
            componentJson["type"] = component.TypeName();
            components.push_back(std::move(componentJson));
        });

        entityJson["components"] = std::move(components);
        entities.push_back(std::move(entityJson));
    });

    root["entities"] = std::move(entities);
    return root;
}

} // namespace

bool Scene::Save(std::string_view virtualPath, std::string& outError) {
    if (virtualPath.empty()) {
        outError = "no file to save to";
        return false;
    }

    std::size_t skipped = 0;
    const Json root = BuildSceneDocument(*this, m_name, m_cameraPosition, m_cameraZoom, skipped, m_document);

    const std::string text = root.dump(2);
    if (!FileSystem::WriteTextFile(virtualPath, text, outError)) {
        ENGINE_LOG_ERROR(Channels::kScene, "could not save '{}': {}", virtualPath, outError);
        return false;
    }

    m_sourcePath.assign(virtualPath);

    if (skipped > 0) {
        ENGINE_LOG_WARN(Channels::kScene,
                        "saved '{}', but {} component(s) were left out - the file is not "
                        "a complete record of the scene",
                        virtualPath, skipped);
    } else {
        ENGINE_LOG_INFO(Channels::kScene, "saved '{}': {} entities", virtualPath, m_liveCount);
    }
    outError.clear();
    return true;
}

bool Scene::SaveToString(std::string& outText, std::string& outError) {
    std::size_t skipped = 0;
    const Json root = BuildSceneDocument(*this, m_name, m_cameraPosition, m_cameraZoom, skipped, m_document);

    if (skipped > 0) {
        outError = "this scene contains " + std::to_string(skipped) +
                   " component(s) that cannot be saved, so pressing Stop would not put "
                   "the scene back the way it was";
        return false;
    }

    outText = root.dump();
    outError.clear();
    return true;
}

bool Scene::LoadFromString(std::string_view text, std::string& outError) {
    Unload();

    m_document = ParseJson(text, outError);
    if (!outError.empty()) {
        ENGINE_LOG_ERROR(Channels::kScene, "could not restore the scene: {}", outError);
        return false;
    }

    return BuildFromDocument(outError);
}

void Scene::Unload() {
    if (m_slots.empty()) {
        return;
    }

    for (Slot& slot : m_slots) {
        if (slot.occupied && slot.entity != nullptr) {
            slot.entity->DestroyInternal();
            slot.entity.reset();
            slot.occupied = false;
        }
    }

    m_slots.clear();
    m_freeIndices.clear();
    m_byName.clear();
    m_liveCount = 0;

    Gizmos::Clear();

    ResourceManager::PruneCache();
    ENGINE_LOG_INFO(Channels::kScene, "scene unloaded; {} image(s) still loaded",
                    ResourceManager::LoadedCount());

    m_name.clear();
}

std::string Scene::MakeUniqueName(std::string_view base) const {
    std::string candidate(base);
    if (candidate.empty()) {
        candidate = "Entity";
    }
    if (Find(candidate).IsNull()) {
        return candidate;
    }
    for (int suffix = 1; suffix < 100000; ++suffix) {
        std::string attempt = std::string(base) + "_" + std::to_string(suffix);
        if (Find(attempt).IsNull()) {
            return attempt;
        }
    }
    return candidate;
}

bool Scene::RenameEntity(EntityId id, std::string_view newName) {
    Entity* entity = Get(id);
    if (entity == nullptr || newName.empty()) {
        return false;
    }
    if (entity->Name() == newName) {
        return true; // renaming something to what it is already called
    }
    if (!Find(newName).IsNull()) {
        ENGINE_LOG_WARN(Channels::kScene, "cannot rename '{}': something is already called '{}'",
                        entity->Name(), newName);
        return false;
    }

    m_byName.erase(entity->Name());
    entity->SetName(newName);
    m_byName[std::string(newName)] = id;
    return true;
}

EntityId Scene::DuplicateEntity(EntityId id, std::string& outError) {
    Entity* source = Get(id);
    if (source == nullptr) {
        outError = "there is nothing selected to duplicate";
        return EntityId{};
    }

    Json componentBlobs = Json::array();
    std::vector<std::string> componentTypes;

    source->ForEachComponent([&](Component& component) {
        Json blob = Json::object();
        if (!component.Serialize(blob)) {
            ENGINE_LOG_WARN(Channels::kScene,
                            "duplicating '{}': '{}' cannot be copied and will be created "
                            "with its default settings",
                            source->Name(), component.TypeName());
        }
        componentBlobs.push_back(std::move(blob));
        componentTypes.emplace_back(component.TypeName());
    });

    const std::string name = MakeUniqueName(source->Name());
    const EntityId copyId = CreateEntity(name);
    Entity* copy = Get(copyId);
    if (copy == nullptr) {
        outError = "could not create the copy";
        return EntityId{};
    }

    for (std::size_t i = 0; i < componentTypes.size(); ++i) {
        std::unique_ptr<Component> component = ComponentFactory::Create(componentTypes[i]);
        if (component == nullptr) {
            continue;
        }
        std::string componentError;
        if (!component->Deserialize(componentBlobs[i], componentError)) {
            ENGINE_LOG_WARN(Channels::kScene, "duplicating '{}': {}", name, componentError);
        }
        copy->AddComponent(std::move(component));
    }

    if (Transform2D* sourceParent = source->Transform().Parent(); sourceParent != nullptr) {
        copy->Transform().SetParent(sourceParent);
    }

    outError.clear();
    return copyId;
}

} // namespace eng