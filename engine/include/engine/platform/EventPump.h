#pragma once

#include <vector>

namespace eng {

enum class RawEventKind {
    None,
    Quit, // the user asked to close the program
    KeyDown,
    KeyUp,
    MouseButtonDown,
    MouseButtonUp,
    MouseMove,
    MouseWheel,
    WindowResized,
    WindowFocusGained, // the user switched back to this window
    WindowFocusLost, // the user switched away from it
};

const char* ToString(RawEventKind kind);

struct RawEvent {
    int code = 0; // which key, or which mouse button
    float mouseX = 0.0f; // in window pixels, measured from the top-left
    float mouseY = 0.0f;
    float wheelY = 0.0f; // positive is scroll up
    RawEventKind kind = RawEventKind::None;
};

class EventPump {
public:
    void Poll();
    std::size_t Count() const;
    const RawEvent& At(std::size_t index) const;
    bool QuitRequested() const;
    bool FocusGainedThisFrame() const { return m_focusGained; }
    bool FocusLostThisFrame() const { return m_focusLost; }
    bool WasConsumed(std::size_t index) const;
    float MouseX() const { return m_mouseX; }
    float MouseY() const { return m_mouseY; }
    static const char* KeyName(int code);
    static int KeyCodeFromName(const char* name);
    static int MouseButtonFromName(const char* name);

private:
    std::vector<RawEvent> m_events;
    std::vector<char> m_consumed;
    float m_mouseX = 0.0f;
    float m_mouseY = 0.0f;
    bool m_quitRequested = false;
    bool m_focusGained = false;
    bool m_focusLost = false;
};

} // namespace eng
