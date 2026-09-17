#pragma once

#include <engine/core/Json.h>
#include <engine/math/Vec2.h>
#include <string>
#include <string_view>
#include <vector>

namespace eng {

class EventPump;

enum class ActionState {
    Idle,
    Pressed, // went down THIS frame
    Held, // still down, was already down last frame
    Released, // came up THIS frame
};

const char* ToString(ActionState state);

class InputMap {
public:
    static void PushContext(std::string_view context);
    static void PopContext();
    static void ClearContexts();
    static std::string ActiveContext(); // the one on top
    static std::size_t ContextDepth();

    static bool IsPressed(std::string_view action); // only on the frame it went down
    static bool IsHeld(std::string_view action);
    static bool IsReleased(std::string_view action);
    static bool IsDown(std::string_view action); // pressed OR held
    static ActionState GetState(std::string_view action);

    static float GetAxis(std::string_view action);
    static Vec2 GetAxis2D(std::string_view negX, std::string_view posX, std::string_view negY, std::string_view posY);

    static void Update(const EventPump& pump);
    static void LoadBindings(const Json& inputSection, std::string& outWarnings);
    static void Bind(std::string_view context, std::string_view action, std::string_view binding);
    static void ClearBindings();
    static void InjectAction(std::string_view action, bool down);
    static void ClearInjectedActions();

    struct BindingInfo {
        std::string context;
        std::string action;
        std::string binding;
    };
    static void Snapshot(std::vector<BindingInfo>& out);
};

} // namespace eng
