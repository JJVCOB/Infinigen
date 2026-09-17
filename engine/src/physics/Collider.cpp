#include <engine/physics/Collider.h>

namespace eng {

bool ColliderComponent::CaresAbout(const std::string& layer) const {
    return false;
}

bool ColliderComponent::Deserialize(const Json& node, std::string& outError) {
    return false;
}

bool ColliderComponent::Serialize(Json& out) const {
    return false;
}

void ColliderComponent::OnAttach() {
}

void ColliderComponent::OnDetach() {
}

bool AABBColliderComponent::Deserialize(const Json& node, std::string& outError) {
    return false;
}

bool AABBColliderComponent::Serialize(Json& out) const {
    return false;
}

AABB AABBColliderComponent::WorldBounds() const {
    return AABB{};
}

bool CircleColliderComponent::Deserialize(const Json& node, std::string& outError) {
    return false;
}

bool CircleColliderComponent::Serialize(Json& out) const {
    return false;
}

Circle CircleColliderComponent::WorldCircle() const {
    return Circle{};
}

AABB CircleColliderComponent::WorldBounds() const {
    return AABB{};
}

void CollisionSystem::Register(ColliderComponent& collider) {
}

void CollisionSystem::Unregister(ColliderComponent& collider) {
}

void CollisionSystem::Clear() {
}

void CollisionSystem::Update(float deltaSeconds) {
}

std::size_t CollisionSystem::ColliderCount() {
    return 0;
}

std::size_t CollisionSystem::ActivePairCount() {
    return 0;
}

void CollisionSystem::RegisterComponentTypes() {
}

} // namespace eng
