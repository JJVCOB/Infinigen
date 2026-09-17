#include <engine/scene/Component.h>
#include <engine/scene/Entity.h>

namespace eng {

Entity::~Entity() {
}

void Entity::DestroyInternal() {
}

void Entity::SetName(std::string_view name) {
}

Component* Entity::AddComponent(std::string_view typeName) {
    return nullptr;
}

Component* Entity::AddComponent(std::unique_ptr<Component> component) {
    return nullptr;
}

Component* Entity::FindComponent(std::string_view typeName) {
    return nullptr;
}

const Component* Entity::FindComponent(std::string_view typeName) const {
    return nullptr;
}

bool Entity::RemoveComponent(std::string_view typeName) {
    return false;
}

Component* Entity::ComponentAt(std::size_t index) {
    return nullptr;
}

void Entity::ForEachComponent(const std::function<void(Component&)>& fn) {
}

Transform2D& Entity::Transform() {
    static Transform2D fallback;
    return fallback;
}

const Transform2D& Entity::Transform() const {
    static const Transform2D fallback;
    return fallback;
}

} // namespace eng
