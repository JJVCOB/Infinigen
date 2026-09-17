#include <engine/input/InputMap.h>

namespace eng {

const char* ToString(ActionState state) {
    return "Idle";
}

void InputMap::PushContext(std::string_view context) {
}

void InputMap::PopContext() {
}

void InputMap::ClearContexts() {
}

std::string InputMap::ActiveContext() {
    return {};
}

std::size_t InputMap::ContextDepth() {
    return 0;
}

bool InputMap::IsPressed(std::string_view action) {
    return false;
}

bool InputMap::IsHeld(std::string_view action) {
    return false;
}

bool InputMap::IsReleased(std::string_view action) {
    return false;
}

bool InputMap::IsDown(std::string_view action) {
    return false;
}

ActionState InputMap::GetState(std::string_view action) {
    return ActionState::Idle;
}

float InputMap::GetAxis(std::string_view action) {
    return 0.0f;
}

Vec2 InputMap::GetAxis2D(std::string_view negX, std::string_view posX, std::string_view negY, std::string_view posY) {
    return Vec2{};
}

void InputMap::Update(const EventPump& pump) {
}

void InputMap::Bind(std::string_view context, std::string_view action, std::string_view binding) {
}

void InputMap::LoadBindings(const Json& inputSection, std::string& outWarnings) {
}

void InputMap::ClearBindings() {
}

void InputMap::InjectAction(std::string_view action, bool down) {
}

void InputMap::ClearInjectedActions() {
}

void InputMap::Snapshot(std::vector<BindingInfo>& out) {
}

} // namespace eng
