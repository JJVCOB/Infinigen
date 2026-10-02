#include <engine/core/Log.h>
#include <engine/math/Random.h>
#include <engine/math/Vec2.h>
#include <cmath>
#include <utility>

namespace eng {

int Random::NextInt(int lo, int hiInclusive) {
    if (lo > hiInclusive) {
        ENGINE_LOG_WARN(Channels::kCore, "Random::NextInt called with lo={} greater than hi={}, swapping them", lo, hiInclusive);
        std::swap(lo, hiInclusive);
    }
    std::uniform_int_distribution<int> distribution(lo, hiInclusive);
    return distribution(m_engine);
}

float Random::NextFloat01() {
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(m_engine);
}

float Random::NextRange(float lo, float hi) {
    if (lo > hi) {
        std::swap(lo, hi);
    }
    std::uniform_real_distribution<float> distribution(lo, hi);
    return distribution(m_engine);
}

bool Random::NextBool() {
    std::bernoulli_distribution distribution(0.5);
    return distribution(m_engine);
}

Random::UnitVector Random::NextDirection() {
    const float angle = NextRange(0.0f, kTwoPi);
    return UnitVector{std::cos(angle), std::sin(angle)};
}

Random& GlobalRandom() {
    static Random instance;
    return instance;
}

} // namespace eng
