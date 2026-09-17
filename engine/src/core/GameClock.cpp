#include <engine/core/GameClock.h>

namespace eng {

void GameClock::Init() {
}

void GameClock::SetFixedStepSeconds(float seconds) {
}

void GameClock::SetTimeScale(float scale) {
}

void GameClock::SetMaxStepsPerFrame(int steps) {
}

int GameClock::BeginFrame(double realDeltaSeconds) {
    return 0;
}

void GameClock::OnStepConsumed() {
}

} // namespace eng
