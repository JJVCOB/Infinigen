#include <engine/math/Random.h>

namespace eng {

int Random::NextInt(int lo, int hiInclusive) {
    return lo;
}

float Random::NextFloat01() {
    return 0.0f;
}

float Random::NextRange(float lo, float hi) {
    return lo;
}

bool Random::NextBool() {
    return false;
}

Random::UnitVector Random::NextDirection() {
    return Random::UnitVector{1.0f, 0.0f};
}

Random& GlobalRandom() {
    static Random instance;
    return instance;
}

} // namespace eng
