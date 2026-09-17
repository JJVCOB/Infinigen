#pragma once

#include <engine/math/Vec2.h>
#include <engine/scene/EntityId.h>

namespace eng {

class Scene;

class DeferredOps {
public:
    static void QueueDestroy(EntityId id);
    static bool IsPendingDestroy(EntityId id);
    static void Apply(Scene& scene);
    static void Clear();
    static std::size_t PendingDestroyCount();
};

} // namespace eng
