#include <engine/core/Log.h>
#include <engine/input/InputMap.h>
#include <engine/platform/EventPump.h>
#include <algorithm>
#include <map>

namespace eng {

namespace {

enum class Device {None, Key, MouseButton};

struct Binding {
    Device device = Device::None;
    int code = 0;
};

struct ActionEntry {
    std::vector<Binding> bindings;
    ActionState state = ActionState::Idle;
    bool downNow, downLast = false;
};

struct Context {
    std::map<std::string, ActionEntry> actions;
};

std::map<std::string, Context> g_contexts;
std::vector<std::string> g_stack;

ActionEntry* FindOwningAction(Device device, int code) {
    for (auto it = g_stack.rbegin(); it != g_stack.rend(); it++) {
        const auto contextIt = g_contexts.find(*it);
        if (contextIt == g_contexts.end()) {
            continue;
        }

        for (auto& [name, action] : contextIt -> second.actions) {
            for (const Binding& binding : action.bindings) {
                if (binding.device == device && binding.code == code) {
                    return &action;
                }
            }
        }
    }

    return nullptr;
}

ActionEntry* FindAction(std::string_view action) {
    const std::string key(action);
    
    for (auto it = g_stack.rbegin(); it != g_stack.rend(); it++) {
        const auto contextIt = g_contexts.find(*it);
        if (contextIt == g_contexts.end()) {
            continue;
        }

        const auto actionIt = contextIt->second.actions.find(key);
        if (actionIt != contextIt->second.actions.end()) {
            return &actionIt->second;
        }
    }

    return nullptr;
}

Binding ParseBinding(std::string_view text, std::string& outWarning) {
    Binding binding;

    const std::size_t dot = text.find('.');
    if (dot == std::string_view::npos) {
        outWarning = "binding '" + std::string(text) +
                     "' is missing its device prefix (expected Key or Mouse)";
        return binding;
    }

    const std::string_view device = text.substr(0, dot);
    const std::string name(text.substr(dot + 1));

    if (device == "Key") {
        const int code = EventPump::KeyCodeFromName(name.c_str());
        if (code < 0) {
            outWarning = "There is no key called '" + name + "'";
            return binding;
        }
        binding.device = Device::Key;
        binding.code = code;
        return binding;
    }

    if (device == "Mouse") {
        const int code = EventPump::MouseButtonFromName(name.c_str());
        if (code < 0) {
            outWarning =
                "There is no mouse button called '" + name + "' (try Left, Right, or Middle)";
            return binding;
        }
        binding.device = Device::MouseButton;
        binding.code = code;
        return binding;
    }

    outWarning = "Unknown device '" + std::string(device) + "' in a binding";
    return binding;
}

} // namespace

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
