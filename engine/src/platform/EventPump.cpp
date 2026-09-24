#include <engine/core/Log.h>
#include <engine/platform/EventPump.h>
#include <engine/tools/GuiHooks.h>
#include <SDL3/SDL.h>

namespace eng {

const char* ToString(RawEventKind kind) {
    switch (kind) {
        using enum RawEventKind;
        case None: return "None";
        case Quit: return "Quit";
        case KeyDown: return "KeyDown";
        case KeyUp: return "KeyUp";
        case MouseButtonDown: return "MouseButtonDown";
        case MouseButtonUp: return "MouseButtonUp";
        case MouseMove: return "MouseMove";
        case MouseWheel: return "MouseWheel";
        case WindowResized: return "WindowResized";
        case WindowFocusGained: return "WindowFocusGained";
        case WindowFocusLost: return "WindowFocusLost";
    }
    return "?";
}

void EventPump::Poll() {
    m_events.clear();
    m_consumed.clear();
    m_quitRequested, m_focusGained, m_focusLost = false;

    if (m_events.capacity() == 0) {
        m_events.reserve(64);
        m_consumed.reserve(64);
    }

    const GuiHooks& gui = GetGuiHooks();
    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent)) {
        const bool guiHandled = (gui.ProcessEvent != nullptr) && gui.ProcessEvent(&sdlEvent);
        RawEvent event;
        bool recognized = true;

        switch (sdlEvent.type) {
            case SDL_EVENT_QUIT:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                event.kind = RawEventKind::Quit;
                m_quitRequested = true;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (sdlEvent.key.repeat) {
                    recognized = false;
                    break;
                }
                event.kind = RawEventKind::KeyDown;
                event.code = static_cast<int>(sdlEvent.key.scancode);
                break;
            case SDL_EVENT_KEY_UP:
                event.kind = RawEventKind::KeyUp;
                event.code = static_cast<int>(sdlEvent.key.scancode);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                event.kind = RawEventKind::MouseButtonDown;
                event.code = static_cast<int>(sdlEvent.button.button);
                event.mouseX = sdlEvent.button.x;
                event.mouseY = sdlEvent.button.y;
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                event.kind = RawEventKind::MouseButtonUp;
                event.code = static_cast<int>(sdlEvent.button.button);
                event.mouseX = sdlEvent.button.x;
                event.mouseY = sdlEvent.button.y;
                break;
            case SDL_EVENT_MOUSE_MOTION:
                event.kind = RawEventKind::MouseMove;
                event.mouseX = sdlEvent.button.x;
                event.mouseY = sdlEvent.button.y;
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                event.kind = RawEventKind::MouseWheel;
                event.wheelY = sdlEvent.wheel.y;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                event.kind = RawEventKind::WindowResized;
                event.mouseX = static_cast<float>(sdlEvent.window.data1);
                event.mouseY = static_cast<float>(sdlEvent.window.data2);
                break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                event.kind = RawEventKind::WindowFocusGained;
                m_focusGained = true;
                break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                event.kind = RawEventKind::WindowFocusLost;
                m_focusLost = true;
                break;
            default:
                recognized = false;
                break;
        }

        if (!recognized) { continue; }

        bool consumed = false;
        switch (event.kind) { 
            using enum RawEventKind;
            case KeyDown:
            case KeyUp:
                consumed = guiHandled && gui.WantsKeyboard != nullptr && gui.WantsKeyboard();
                break;
            case MouseButtonDown:
            case MouseButtonUp:
            case MouseMove:
            case MouseWheel:
                consumed = guiHandled && gui.WantsMouse != nullptr && gui.WantsMouse();
                break;
            default:
                consumed = false;
                break;
        }

        m_events.push_back(event);
        m_consumed.push_back(consumed ? char{1} : char{0});
    }

    float x, y = 0.0f;
    SDL_GetMouseState(&x, &y);
    m_mouseX, m_mouseY = x, y;
    //m_mouseY = y;
}

std::size_t EventPump::Count() const { return m_events.size(); }

const RawEvent& EventPump::At(std::size_t index) const {
    if (index >= Count()) {
        ENGINE_LOG_WARN(Channels::kInput, "EventPump::At({}) is past the end of {} events.", index, Count());
        static const RawEvent kNone{};
        return kNone;
    }
    return m_events[index];
}

bool EventPump::QuitRequested() const {
    return m_quitRequested;
}

bool EventPump::WasConsumed(std::size_t index) const {
    return index < m_consumed.size() && m_consumed[index] != 0;
}

const char* EventPump::KeyName(int code) {
    const char* name = SDL_GetScancodeName(static_cast<SDL_Scancode>(code));
    return (name != nullptr && name[0] != '\0') ? name : "?";
}

int EventPump::KeyCodeFromName(const char* name) {
    return -1;
}

int EventPump::MouseButtonFromName(const char* name) {
    return -1;
}

} // namespace eng
