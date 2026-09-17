#include <engine/scene/Component.h>

namespace eng {

EntityId Component::OwnerId() const {
    return EntityId{};
}

Scene* Component::GetScene() const {
    return nullptr;
}

Transform2D* Component::OwnerTransform() const {
    return nullptr;
}

void ComponentFactory::Register(std::string_view typeName, CreateFn create) {
}

std::unique_ptr<Component> ComponentFactory::Create(std::string_view typeName) {
    return nullptr;
}

bool ComponentFactory::IsRegistered(std::string_view typeName) {
    return false;
}

void ComponentFactory::ForEachType(const std::function<void(const char*)>& fn) {
}

void ComponentFactory::RegisterBuiltins() {
}

bool TransformComponent::Deserialize(const Json& node, std::string& outError) {
    return false;
}

bool TransformComponent::Serialize(Json& out) const {
    return false;
}

SpriteComponent::~SpriteComponent() {
}

bool SpriteComponent::Deserialize(const Json& node, std::string& outError) {
    return false;
}

bool SpriteComponent::Serialize(Json& out) const {
    return false;
}

void SpriteComponent::OnAttach() {
}

void SpriteComponent::OnDetach() {
}

void SpriteComponent::SetTint(Color tint) {
}

void SpriteComponent::SetLayer(int layer) {
}

void SpriteComponent::SetTexture(std::string_view virtualPath) {
}

void SpriteRenderSystem::Register(SpriteComponent& sprite) {
}

void SpriteRenderSystem::Unregister(SpriteComponent& sprite) {
}

void SpriteRenderSystem::Render(Camera& camera) {
}

std::size_t SpriteRenderSystem::Count() {
    return 0;
}

void SpriteRenderSystem::Clear() {
}

} // namespace eng
