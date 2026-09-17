#pragma once

namespace eng {

class GameClock {
public:
    void Init();
    int BeginFrame(double realDeltaSeconds);
    void OnStepConsumed();
    void  SetTimeScale(float scale);
    float TimeScale() const { return m_timeScale; }

    void Pause() { m_paused = true; }
    void Resume() { m_paused = false; }
    void SetPaused(bool paused) { m_paused = paused; }
    bool IsPaused() const { return m_paused; }
    void RequestSingleStep() { m_singleStepRequested = true; }

    double RealSeconds() const { return m_realSeconds; }
    double GameSeconds() const { return m_gameSeconds; }

    float RealDeltaSeconds() const { return m_realDelta; }
    float FixedStepSeconds() const { return m_fixedStep; }
    void SetFixedStepSeconds(float seconds);

    unsigned long long TickCount() const { return m_ticks; }

    int MaxStepsPerFrame() const { return m_maxSteps; }
    void SetMaxStepsPerFrame(int steps);

private:
    float m_fixedStep = 1.0f / 60.0f;   // 60 simulation steps per second
    float m_timeScale = 1.0f;
    int m_maxSteps = 5;

    double m_accumulator = 0.0;   // real time collected but not yet simulated
    double m_realSeconds = 0.0;
    double m_gameSeconds = 0.0;
    float m_realDelta = 0.0f;

    unsigned long long m_ticks = 0;

    bool m_paused = false;
    bool m_singleStepRequested = false;
};

} // namespace eng
