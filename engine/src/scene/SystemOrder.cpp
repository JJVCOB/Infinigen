#include <engine/scene/SystemOrder.h>

namespace eng {

void SystemScheduler::Register(System* system) {
}

void SystemScheduler::Unregister(System* system) {
}

void SystemScheduler::Clear() {
}

void SystemScheduler::UpdateRange(int minOrder, int maxOrder, float deltaSeconds) {
}

void SystemScheduler::Simulate(float fixedStepSeconds) {
}

void SystemScheduler::RenderPass(float realDeltaSeconds) {
}

void SystemScheduler::LogOrder() {
}

void SystemScheduler::ForEach(const std::function<void(System&)>& fn) {
}

std::size_t SystemScheduler::Count() {
    return 0;
}

} // namespace eng
