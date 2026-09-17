#pragma once

#include <random>

namespace eng {

class Random {
public:
    static constexpr unsigned int kDefaultSeed = 12345u;
    Random() : Random(kDefaultSeed) {}
    explicit Random(unsigned int seed) : m_engine(seed), m_seed(seed) {}
    int NextInt(int lo, int hiInclusive);
    float NextFloat01();
    float NextRange(float lo, float hi);
    bool NextBool();
    struct UnitVector { float x, y; };
    UnitVector NextDirection();
    void Reseed(unsigned int seed) { m_engine.seed(seed); m_seed = seed; }
    unsigned int Seed() const { return m_seed; }

private:
    std::mt19937 m_engine;
    unsigned int m_seed = kDefaultSeed;
};

Random& GlobalRandom();

} // namespace eng
