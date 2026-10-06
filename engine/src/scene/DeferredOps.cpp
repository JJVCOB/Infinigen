#include <engine/core/Log.h>
#include <engine/scene/DeferredOps.h>
#include <engine/scene/Scene.h>
#include <algorithm>
#include <set>
#include <vector>

namespace eng {
namespace {

std::vector<EntityId> g_destroys;
std::set<EntityId> g_pendingDestroy;

} // namespace

void DeferredOps::QueueDestroy(EntityId id) {
    if (id.IsNull()) {
        ENGINE_LOG_WARN(Channels::kScene, "Entity ID was null.");
        return;
    }
    if (!g_pendingDestroy.insert(id).second) {
        return;
    }
    g_destroys.push_back(id);
}

bool DeferredOps::IsPendingDestroy(EntityId id) {
    return !id.IsNull() && g_pendingDestroy.contains(id);
}

void DeferredOps::Apply(Scene& scene) {
    std::vector<EntityId> destroys;
    destroys.swap(g_destroys);

    for (const EntityId id : destroys) {
        if (scene.IsValid(id)) {
            scene.DestroyEntityImmediate(id);
            ENGINE_LOG_INFO(Channels::kScene, "Destroyed entity, ID: {}", id.index);
        }
    }

    g_pendingDestroy.clear();
}

void DeferredOps::Clear() {
    g_destroys.clear();
    g_pendingDestroy.clear();
}

std::size_t DeferredOps::PendingDestroyCount() {
    return g_destroys.size();
}

} // namespace eng