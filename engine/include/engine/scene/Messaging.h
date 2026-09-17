#pragma once

#include <engine/scene/EntityId.h>
#include <engine/core/Subsystem.h>
#include <functional>
#include <string>

namespace eng {

struct Message {
    std::string type;
    EntityId sender{};
    EntityId target{};
    float f0 = 0.0f;
    float f1 = 0.0f;
    int i0 = 0;
    EntityId other{};
};

using MessageHandler = std::function<void(const Message&)>;
using SubscriptionId = unsigned int;

namespace MessageTypes {
inline constexpr const char* kCollisionEnter = "CollisionEnter";
inline constexpr const char* kCollisionStay = "CollisionStay";
inline constexpr const char* kCollisionExit = "CollisionExit";
} // namespace MessageTypes

class MessageBus : public Subsystem {
public:
    bool Init(const BootConfig& config) override;
    void Shutdown() override;
    static SubscriptionId SubscribeBroadcast(std::string_view type, MessageHandler handler);
    static void Unsubscribe(SubscriptionId id);
    static void Send(const Message& message);
    static void Dispatch();
    static void Clear();
};

} // namespace eng
