#pragma once

#include <engine/Engine.h>

namespace game {

class CollectorGame final : public eng::System {
public:
    bool Init();
    void Shutdown();
    void SetAutopilot(bool on) { m_autopilot = on; }
    bool IsFinished() const { return m_phase != Phase::Playing; }
    int Collected() const { return m_collected; }
    void Update(float deltaSeconds) override;
    const char* Name() const override { return "CollectorGame"; }
    int Order() const override { return eng::SystemStage::kGameplay; }

private:
    enum class Phase { Playing, Won, Lost };
    void OnCollected(eng::EntityId pickup);
    void DrawHud();
    void DriveAutopilot();
    static bool IsPickup(eng::Entity& entity);
    eng::EntityId m_player{};
    int m_collected = 0;
    int m_totalPickups = 0;
    float m_secondsLeft = 60.0f;
    Phase m_phase = Phase::Playing;
    eng::SubscriptionId m_subscription = 0;
    bool m_autopilot = false;
    static constexpr float kPlayerSpeed = 220.0f;
    static constexpr float kTimeLimit = 60.0f;
};

} // namespace game
