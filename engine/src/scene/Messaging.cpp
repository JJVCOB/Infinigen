#include <engine/scene/Messaging.h>

namespace eng {

SubscriptionId MessageBus::SubscribeBroadcast(std::string_view type, MessageHandler handler) {
    return 0;
}

bool MessageBus::Init(const BootConfig& config)
{
    return false;
}
void MessageBus::Shutdown() {}

void MessageBus::Unsubscribe(SubscriptionId id) {
}

void MessageBus::Send(const Message& message) {
}

void MessageBus::Dispatch() {
}

void MessageBus::Clear() {
}

} // namespace eng
