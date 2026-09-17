#include <engine/scene/ScriptComponent.h>

namespace eng {

Entity* ScriptBehaviour::Owner() const {
    return nullptr;
}

EntityId ScriptBehaviour::OwnerId() const {
    return EntityId{};
}

Scene* ScriptBehaviour::GetScene() const {
    return nullptr;
}

Transform2D* ScriptBehaviour::Transform() const {
    return nullptr;
}

std::string DescribeHooks(const ScriptHooks& hooks) {
    return {};
}

void ScriptRegistry::Register(std::string_view scriptName, CreateFn create, const ScriptHooks& hooks, std::string_view sourceFile) {
}

bool ScriptRegistry::IsRegistered(std::string_view scriptName) {
    return false;
}

const ScriptRegistry::Entry* ScriptRegistry::Find(std::string_view scriptName) {
    return nullptr;
}

std::unique_ptr<ScriptBehaviour> ScriptRegistry::Create(std::string_view scriptName) {
    return nullptr;
}

void ScriptRegistry::ForEachScript(const std::function<void(const char*)>& fn) {
}

void ScriptRegistry::ForEachEntry(
    const std::function<void(const char* name, const Entry& entry)>& fn) {
}

std::size_t ScriptRegistry::Count() {
    return 0;
}

void ScriptRegistry::Clear() {
}

ScriptComponent::~ScriptComponent() {
}

bool ScriptComponent::Deserialize(const Json& node, std::string& outError) {
    return false;
}

bool ScriptComponent::Serialize(Json& out) const {
    return false;
}

void ScriptComponent::OnAttach() {
}

void ScriptComponent::OnDetach() {
}

void ScriptComponent::SetScriptName(std::string_view name) {
}

void ScriptComponent::UnbindForReload() {
}

void ScriptComponent::RebindAfterReload() {
}

void ScriptComponent::Bind() {
}

void ScriptComponent::Unbind() {
}

void ScriptComponent::Tick(float deltaSeconds) {
}

void ScriptComponent::DispatchCollision(const std::string& messageType, EntityId other) {
}

void ScriptSystem::Register(ScriptComponent& script) {
}

void ScriptSystem::Unregister(ScriptComponent& script) {
}

void ScriptSystem::Clear() {
}

std::size_t ScriptSystem::Count() {
    return 0;
}

std::size_t ScriptSystem::TickingCount() {
    return 0;
}

std::size_t ScriptSystem::UnresolvedCount() {
    return 0;
}

std::size_t ScriptSystem::CountUsing(std::string_view scriptName) {
    return 0;
}

std::size_t ScriptSystem::RebindRenamed(std::string_view oldName, std::string_view newName) {
    return 0;
}

void ScriptSystem::UnbindAll() {
}

void ScriptSystem::RebindAll() {
}

void ScriptSystem::Update(float deltaSeconds) {
}

void ScriptSystem::SubscribeToCollisions() {
}

void ScriptSystem::RegisterComponentTypes() {
}

} // namespace eng
