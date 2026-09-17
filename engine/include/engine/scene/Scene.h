#pragma once

#include <engine/core/Json.h>
#include <engine/scene/Entity.h>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace eng {

class Scene {
public:
    Scene();
    ~Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    bool Load(std::string_view virtualPath, std::string& outError);
    bool Save(std::string_view virtualPath, std::string& outError);
    bool SaveToString(std::string& outText, std::string& outError);
    bool LoadFromString(std::string_view text, std::string& outError);
    void Unload();
    const std::string& Name() const { return m_name; }
    const std::string& SourcePath() const { return m_sourcePath; }
    bool IsLoaded() const { return m_liveCount > 0; }
    EntityId CreateEntity(std::string_view name);
    void DestroyEntityImmediate(EntityId id);
    bool RenameEntity(EntityId id, std::string_view newName);
    EntityId DuplicateEntity(EntityId id, std::string& outError);
    std::string MakeUniqueName(std::string_view base) const;
    Entity* Get(EntityId id);
    bool IsValid(EntityId id) const;
    EntityId Find(std::string_view name) const;
    void ForEach(const std::function<void(Entity&)>& fn);
    std::size_t EntityCount() const { return m_liveCount; }
    EntityId CreateEntityFromJson(const Json& node, std::string_view nameOverride, std::string& outError);
    Vec2  InitialCameraPosition() const { return m_cameraPosition; }
    float InitialCameraZoom() const { return m_cameraZoom; }

    void SetCameraState(Vec2 position, float zoom) {
        m_cameraPosition = position;
        m_cameraZoom = zoom;
    }

    static Scene* Active();
    static void   SetActive(Scene* scene);

private:
    struct Slot {
        std::unique_ptr<Entity> entity;
        int generation = 1;
        bool occupied   = false;
    };

    void ResolveParents(const Json& entitiesArray);
    bool BuildFromDocument(std::string& outError);
    std::vector<Slot> m_slots;
    std::vector<int> m_freeIndices;
    std::unordered_map<std::string, EntityId> m_byName;
    std::size_t m_liveCount = 0;
    Json m_document = Json::object();
    std::string m_name;
    std::string m_sourcePath;
    Vec2 m_cameraPosition{0.0f, 0.0f};
    float m_cameraZoom = 1.0f;
};

} // namespace eng
