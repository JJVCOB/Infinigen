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
    switch (state) {
    case ActionState::Idle:
        return "Idle";
    case ActionState::Pressed:
        return "Pressed";
    case ActionState::Held:
        return "Held";
    case ActionState::Released:
        return "Released";
    }
    return "?";
}

void InputMap::PushContext(std::string_view context) {
    g_stack.emplace_back(context);
}

void InputMap::PopContext() {
    if (g_stack.empty()) {
        ENGINE_LOG_WARN(Channels::kInput, "PopContext called when no context is active");
        return;
    }
    g_stack.pop_back();
}

void InputMap::ClearContexts() {
    g_stack.clear();
}

std::string InputMap::ActiveContext() {
    return g_stack.empty() ? std::string{} : g_stack.back();
}

std::size_t InputMap::ContextDepth() {
    return g_stack.size();
}

bool InputMap::IsPressed(std::string_view action) {
    const ActionEntry* entry = FindAction(action);
    return entry != nullptr && entry->state == ActionState::Pressed;
}

bool InputMap::IsHeld(std::string_view action) {
    const ActionEntry* entry = FindAction(action);
    return entry != nullptr && entry->state == ActionState::Held;
}

bool InputMap::IsReleased(std::string_view action) {
    const ActionEntry* entry = FindAction(action);
    return entry != nullptr && entry->state == ActionState::Released;
}

bool InputMap::IsDown(std::string_view action) {
    const ActionEntry* entry = FindAction(action);
    return entry != nullptr &&
           (entry->state == ActionState::Pressed || entry->state == ActionState::Held);
}

ActionState InputMap::GetState(std::string_view action) {
    const ActionEntry* entry = FindAction(action);
    return (entry != nullptr) ? entry->state : ActionState::Idle;
}

float InputMap::GetAxis(std::string_view action) {
    return IsDown(action) ? 1.0f : 0.0f;
}

Vec2 InputMap::GetAxis2D(std::string_view negX, std::string_view posX, std::string_view negY, std::string_view posY) {
    const Vec2 raw{GetAxis(posX) - GetAxis(negX), GetAxis(posY) - GetAxis(negY)};
    return raw.Normalized();
}

void InputMap::Update(const EventPump& pump) {
    for (auto& [contextName, context] : g_contexts) {
        for (auto& [actionName, action] : context.actions) {
            action.downLast = action.downNow;
        }
    }

    for (std::size_t i = 0; i < pump.Count(); ++i) {
        if (pump.WasConsumed(i)) {
            continue;
        }

        const RawEvent& event = pump.At(i);
        ActionEntry* entry = nullptr;

        switch (event.kind) {
        case RawEventKind::KeyDown:
            entry = FindOwningAction(Device::Key, event.code);
            if (entry != nullptr) {
                entry->downNow = true;
            }
            break;
        case RawEventKind::KeyUp:
            entry = FindOwningAction(Device::Key, event.code);
            if (entry != nullptr) {
                entry->downNow = false;
            }
            break;
        case RawEventKind::MouseButtonDown:
            entry = FindOwningAction(Device::MouseButton, event.code);
            if (entry != nullptr) {
                entry->downNow = true;
            }
            break;
        case RawEventKind::MouseButtonUp:
            entry = FindOwningAction(Device::MouseButton, event.code);
            if (entry != nullptr) {
                entry->downNow = false;
            }
            break;
        default:
            break;
        }
    }

    for (auto& [contextName, context] : g_contexts) {
        for (auto& [actionName, action] : context.actions) {
            if (action.downNow && !action.downLast) {
                action.state = ActionState::Pressed;
            } else if (action.downNow) {
                action.state = ActionState::Held;
            } else if (action.downLast) {
                action.state = ActionState::Released;
            } else {
                action.state = ActionState::Idle;
            }
        }
    }
}

void InputMap::Bind(std::string_view context, std::string_view action, std::string_view binding) {
    std::string warning;
    const Binding parsed = ParseBinding(binding, warning);
    if (!warning.empty()) {
        ENGINE_LOG_WARN(Channels::kInput, "{}", warning);
        return;
    }
    if (parsed.device == Device::None) {
        return;
    }
    g_contexts[std::string(context)].actions[std::string(action)].bindings.push_back(parsed);
}

void InputMap::LoadBindings(const Json& inputSection, std::string& outWarnings) {
    if (!inputSection.is_object()) {
        ENGINE_LOG_WARN(Channels::kInput,
                        "the settings file has no \"input\" section, so nothing is bound");
        return;
    }

    const auto contextsIt = inputSection.find("contexts");
    if (contextsIt == inputSection.end() || !contextsIt->is_object()) {
        outWarnings += "input section has no \"contexts\"\n";
        return;
    }

    for (const auto& [contextName, actions] : contextsIt->items()) {
        if (!actions.is_object()) {
            outWarnings += "input.contexts." + contextName + " should be a list of actions\n";
            continue;
        }

        Context& context = g_contexts[contextName];

        for (const auto& [actionName, bindings] : actions.items()) {
            ActionEntry& entry = context.actions[actionName];

            if (!bindings.is_array()) {
                outWarnings += "input.contexts." + contextName + "." + actionName +
                               " should be a list like [\"Key.A\"]\n";
                continue;
            }

            for (const Json& item : bindings) {
                if (!item.is_string()) {
                    continue;
                }
                std::string warning;
                const Binding parsed = ParseBinding(item.get<std::string>(), warning);
                if (!warning.empty()) {
                    // Named with the context and the action, so the message
                    // says which line of the file to go and look at.
                    const std::string full = contextName + "." + actionName + ": " + warning;
                    ENGINE_LOG_WARN(Channels::kInput, "{}", full);
                    outWarnings += full + "\n";
                    continue;
                }
                if (parsed.device != Device::None) {
                    entry.bindings.push_back(parsed);
                }
            }
        }

        ENGINE_LOG_INFO(Channels::kInput, "input context '{}': {} action(s)", contextName,
                        context.actions.size());
    }
}

void InputMap::InjectAction(std::string_view action, bool down) {
    if (ActionEntry* entry = FindAction(action); entry != nullptr) {
        entry->downNow = down;
    }
}

void InputMap::ClearInjectedActions() {
    for (auto& [contextName, context] : g_contexts) {
        for (auto& [actionName, entry] : context.actions) {
            entry.downNow = false;
        }
    }
}

void InputMap::ClearBindings() {
    g_contexts.clear();
    g_stack.clear();
}

void InputMap::Snapshot(std::vector<BindingInfo>& out) {
    out.clear();
    for (const auto& [contextName, context] : g_contexts) {
        for (const auto& [actionName, action] : context.actions) {
            if (action.bindings.empty()) {
                out.push_back({contextName, actionName, "<not bound>"});
                continue;
            }
            for (const Binding& binding : action.bindings) {
                std::string text;
                switch (binding.device) {
                case Device::Key:
                    text = std::string("Key.") + EventPump::KeyName(binding.code);
                    break;
                case Device::MouseButton:
                    text = "Mouse." + std::to_string(binding.code);
                    break;
                case Device::None:
                    text = "<none>";
                    break;
                }
                out.push_back({contextName, actionName, text});
            }
        }
    }
}

} // namespace eng
