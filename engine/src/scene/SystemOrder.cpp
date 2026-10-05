#include <engine/core/Log.h>
#include <engine/scene/SystemOrder.h>
#include <algorithm>
#include <vector>

namespace eng {
namespace {

std::vector<System*> g_systems;

bool g_needsSort = false;

void SortIfNeeded() {
    if (!g_needsSort) {
        return;
    }
    std::stable_sort(g_systems.begin(), g_systems.end(),
                     [](const System* a, const System* b) { return a->Order() < b->Order(); });
    g_needsSort = false;
}

} // namespace

void SystemScheduler::Register(System* system) {
    if (system == nullptr) {
        return;
    }
    g_systems.push_back(system);
    g_needsSort = true;
}

void SystemScheduler::Unregister(System* system) {
    std::erase(g_systems, system);
}

void SystemScheduler::Clear() {
    g_systems.clear();
    g_needsSort = false;
}

void SystemScheduler::UpdateRange(int minOrder, int maxOrder, float deltaSeconds) {
    SortIfNeeded();

    std::vector<System*> running = g_systems;

    for (System* system : running) {
        const int order = system->Order();
        if (order < minOrder || order >= maxOrder) {
            continue;
        }
        system->Update(deltaSeconds);
    }
}

void SystemScheduler::Simulate(float fixedStepSeconds) {
    UpdateRange(0, SystemStage::kFirstRenderStage, fixedStepSeconds);
}

void SystemScheduler::RenderPass(float realDeltaSeconds) {
    UpdateRange(SystemStage::kFirstRenderStage, 1'000'000, realDeltaSeconds);
}

void SystemScheduler::LogOrder() {
    SortIfNeeded();

    ENGINE_LOG_INFO(Channels::kScene, "systems update in this order:");
    for (const System* system : g_systems) {
        const bool perFrame = system->Order() >= SystemStage::kFirstRenderStage;
        ENGINE_LOG_INFO(Channels::kScene, "  {:>4}  {}  ({})", system->Order(), system->Name(),
                        perFrame ? "per frame" : "per fixed step");
    }
}

void SystemScheduler::ForEach(const std::function<void(System&)>& fn) {
    SortIfNeeded();
    for (System* system : g_systems) {
        fn(*system);
    }
}

std::size_t SystemScheduler::Count() {
    return g_systems.size();
}

} // namespace eng