#include <engine/scene/DeferredOps.h>

namespace eng {

void DeferredOps::QueueDestroy(EntityId id) {
}

bool DeferredOps::IsPendingDestroy(EntityId id) {
    return false;
}

void DeferredOps::Apply(Scene& scene) {
}

void DeferredOps::Clear() {
}

std::size_t DeferredOps::PendingDestroyCount() {
    return 0;
}

} // namespace eng
