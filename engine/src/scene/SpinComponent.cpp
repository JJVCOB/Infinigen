#include <engine/scene/SpinComponent.h>

namespace eng {

SpinComponent::~SpinComponent() {
}

bool SpinComponent::Deserialize(const Json& node, std::string& outError) {
    return false;
}

bool SpinComponent::Serialize(Json& out) const {
    return false;
}

void SpinComponent::OnAttach() {
}

void SpinComponent::OnDetach() {
}

void SpinSystem::Register(SpinComponent& spin) {
}

void SpinSystem::Unregister(SpinComponent& spin) {
}

void SpinSystem::Clear() {
}

std::size_t SpinSystem::Count() {
    return 0;
}

void SpinSystem::Update(float deltaSeconds) {
}

void SpinSystem::RegisterComponentTypes() {
}

} // namespace eng
