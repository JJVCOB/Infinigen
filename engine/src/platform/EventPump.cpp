#include <engine/platform/EventPump.h>

namespace eng {

const char* ToString(RawEventKind kind) {
    return "None";
}

void EventPump::Poll() {
}

std::size_t EventPump::Count() const {
    return 0;
}

const RawEvent& EventPump::At(std::size_t index) const {
    static const RawEvent none{};
    return none;
}

bool EventPump::QuitRequested() const {
    return false;
}

bool EventPump::WasConsumed(std::size_t index) const {
    return false;
}

const char* EventPump::KeyName(int code) {
    return "";
}

int EventPump::KeyCodeFromName(const char* name) {
    return -1;
}

int EventPump::MouseButtonFromName(const char* name) {
    return -1;
}

} // namespace eng
