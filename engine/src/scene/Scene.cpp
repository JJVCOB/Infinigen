#include <engine/scene/Scene.h>

namespace eng {

Scene::Scene() = default;

Scene::~Scene() {
}

Scene* Scene::Active() {
    return nullptr;
}

void Scene::SetActive(Scene* scene) {
}

EntityId Scene::CreateEntity(std::string_view name) {
    return EntityId{};
}

void Scene::DestroyEntityImmediate(EntityId id) {
}

Entity* Scene::Get(EntityId id) {
    return nullptr;
}

bool Scene::IsValid(EntityId id) const {
    return false;
}

EntityId Scene::Find(std::string_view name) const {
    return EntityId{};
}

void Scene::ForEach(const std::function<void(Entity&)>& fn) {
}

EntityId Scene::CreateEntityFromJson(const Json& node, std::string_view nameOverride, std::string& outError) {
    return EntityId{};
}

void Scene::ResolveParents(const Json& entitiesArray) {
}

bool Scene::BuildFromDocument(std::string& outError) {
    return false;
}

bool Scene::Load(std::string_view virtualPath, std::string& outError) {
    return false;
}

bool Scene::Save(std::string_view virtualPath, std::string& outError) {
    return false;
}

bool Scene::SaveToString(std::string& outText, std::string& outError) {
    return false;
}

bool Scene::LoadFromString(std::string_view text, std::string& outError) {
    return false;
}

void Scene::Unload() {
}

std::string Scene::MakeUniqueName(std::string_view base) const {
    return {};
}

bool Scene::RenameEntity(EntityId id, std::string_view newName) {
    return false;
}

EntityId Scene::DuplicateEntity(EntityId id, std::string& outError) {
    return EntityId{};
}

} // namespace eng
