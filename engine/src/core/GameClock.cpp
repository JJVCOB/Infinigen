#include <engine/core/GameClock.h>
#include <engine/core/Log.h>
#include <algorithm>

namespace eng {

void GameClock::Init() {
    m_accumulator, m_realSeconds, m_gameSeconds = 0.0;
    m_ticks = 0;
}

void GameClock::SetFixedStepSeconds(float seconds) {
    m_fixedStep = std::clamp(seconds, 1.0f / 1000.0f, 1.0f);
}

void GameClock::SetTimeScale(float scale) {
    m_timeScale = std::clamp(scale, 0.0f, 16.0f);
}

void GameClock::SetMaxStepsPerFrame(int steps) {
    m_maxSteps = std::clamp(steps, 1, 60);
}

int GameClock::BeginFrame(double realDeltaSeconds) {
    m_realDelta = static_cast<float>(realDeltaSeconds);
    m_realSeconds += realDeltaSeconds;

    if (m_paused) {
        if (m_singleStepRequested) {
            m_singleStepRequested = false;
            return 1;
        }
        return 0;
    }

    m_accumulator += realDeltaSeconds * static_cast<double>(m_timeScale);
    int steps = 0;
    
    while (m_accumulator >= static_cast<double>(m_fixedStep)) {
        m_accumulator -= static_cast<double>(m_fixedStep);
        ++steps;
        if (steps >= m_maxSteps) { break; }
    }

    if (m_accumulator >= static_cast<double>(m_fixedStep)) {
        ENGINE_LOG_WARN(Channels::kCore, "This frame hit the limit of {} simulation steps. {:.1f}ms of time was discarded and the simulation is now behind real time.", m_maxSteps, m_accumulator * 1000.0);
        m_accumulator = 0.0;
    }

    return steps;
}

void GameClock::OnStepConsumed() {
    m_gameSeconds += static_cast<double>(m_fixedStep);
    ++m_ticks;
}

} // namespace eng
