#pragma once

#include <functional>

namespace eng {

namespace SystemStage {
inline constexpr int kInput = 100;
inline constexpr int kGameplay = 200;
inline constexpr int kMovement = 300;
inline constexpr int kCollision = 400;
inline constexpr int kCollisionResponse = 500;
inline constexpr int kDeferred = 600;
inline constexpr int kCamera = 700;
inline constexpr int kRender = 800;
inline constexpr int kGizmos = 900;

inline constexpr int kFirstRenderStage = kRender;
} // namespace SystemStage

class System {
public:
    virtual ~System() = default;
    virtual void Update(float deltaSeconds) = 0;
    virtual const char* Name() const  = 0;
    virtual int Order() const = 0;
};

class SystemScheduler {
public:
    static void Register(System* system);
    static void Unregister(System* system);
    static void Clear();
    static void UpdateRange(int minOrder, int maxOrder, float deltaSeconds);
    static void Simulate(float fixedStepSeconds);
    static void RenderPass(float realDeltaSeconds);
    static void LogOrder();
    static void ForEach(const std::function<void(System&)>& fn);
    static std::size_t Count();
};

} // namespace eng
