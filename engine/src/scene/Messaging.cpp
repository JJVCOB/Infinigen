#include <engine/core/Log.h>
#include <engine/scene/Messaging.h>
#include <engine/scene/Scene.h>
#include <algorithm>
#include <deque>
#include <memory>
#include <vector>

namespace eng {
namespace {

struct Subscription {
    SubscriptionId id = 0;
    std::string type;
    MessageHandler handler;
    bool alive = true;
};

std::vector<std::unique_ptr<Subscription>> g_subscriptions;
std::deque<Message> g_queue;

SubscriptionId g_nextId = 1;
bool g_dispatching = false;

void Compact() {
    std::erase_if(g_subscriptions,
                  [](const std::unique_ptr<Subscription>& s) { return s == nullptr || !s->alive; });
}

void DeliverTo(const Subscription& subscription, const Message& message) {
    if (!subscription.alive || !subscription.handler) {
        return;
    }
    if (subscription.type != message.type) {
        return;
    }
    subscription.handler(message);
}

bool TargetStillExists(const Message& message) {
    if (message.target.IsNull()) {
        return true;
    }
    Scene* scene = Scene::Active();
    if (scene == nullptr) {
        return false;
    }
    return scene->IsValid(message.target);
}

} // namespace

SubscriptionId MessageBus::SubscribeBroadcast(std::string_view type, MessageHandler handler) {
    auto subscription = std::make_unique<Subscription>();
    subscription->id = g_nextId++;
    subscription->type = std::string(type);
    subscription->handler = std::move(handler);

    const SubscriptionId id = subscription->id;
    g_subscriptions.push_back(std::move(subscription));
    return id;
}

void MessageBus::Unsubscribe(SubscriptionId id) {
    for (auto& subscription : g_subscriptions) {
        if (subscription != nullptr && subscription->id == id) {
            subscription->alive = false;
            break;
        }
    }
    if (!g_dispatching) {
        Compact();
    }
}

void MessageBus::Send(const Message& message) {
    g_queue.push_back(message);
}

void MessageBus::Dispatch() {
    g_dispatching = true;
    const std::size_t count = g_queue.size();

    for (std::size_t processed = 0; processed < count && !g_queue.empty(); ++processed) {
        const Message message = g_queue.front();
        g_queue.pop_front();

        if (!TargetStillExists(message)) {
            continue;
        }

        for (std::size_t i = 0; i < g_subscriptions.size(); ++i) {
            if (g_subscriptions[i] != nullptr) {
                DeliverTo(*g_subscriptions[i], message);
            }
        }
    }

    g_dispatching = false;
    Compact();
}

bool MessageBus::Init(const BootConfig&) {
    return true;
}

void MessageBus::Shutdown() {
    Clear();
}

void MessageBus::Clear() {
    g_subscriptions.clear();
    g_queue.clear();
    g_dispatching = false;
}

} // namespace eng
