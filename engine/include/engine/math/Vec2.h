#pragma once

#include <cmath>

namespace eng {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
    constexpr Vec2() = default;
    constexpr Vec2(float inX, float inY) : x(inX), y(inY) {}
    constexpr Vec2& operator+=(const Vec2& rhs) { x += rhs.x; y += rhs.y; return *this; }
    constexpr Vec2& operator-=(const Vec2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
    constexpr Vec2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    constexpr Vec2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }
    friend constexpr bool operator==(const Vec2&, const Vec2&) = default;
    constexpr float LengthSquared() const { return x * x + y * y; }
    float Length() const { return std::sqrt(LengthSquared()); }
    static constexpr float kNormalizeEpsilon = 1e-8f;

    Vec2 Normalized() const {
        const float lengthSq = LengthSquared();
        if (lengthSq < kNormalizeEpsilon) {
            return Vec2{0.0f, 0.0f};
        }
        const float inverse = 1.0f / std::sqrt(lengthSq);
        return Vec2{x * inverse, y * inverse};
    }

    void Normalize() { *this = Normalized(); }
    constexpr Vec2 Perpendicular() const { return Vec2{-y, x}; }
    static constexpr Vec2 Zero() { return Vec2{0.0f, 0.0f}; }
    static constexpr Vec2 One() { return Vec2{1.0f, 1.0f}; }
    static constexpr Vec2 UnitX() { return Vec2{1.0f, 0.0f}; }
    static constexpr Vec2 UnitY() { return Vec2{0.0f, 1.0f}; }
};

constexpr Vec2 operator+(const Vec2& a, const Vec2& b) { return Vec2{a.x + b.x, a.y + b.y}; }
constexpr Vec2 operator-(const Vec2& a, const Vec2& b) { return Vec2{a.x - b.x, a.y - b.y}; }
constexpr Vec2 operator-(const Vec2& v) { return Vec2{-v.x, -v.y}; }
constexpr Vec2 operator*(const Vec2& v, float s) { return Vec2{v.x * s, v.y * s}; }
constexpr Vec2 operator*(float s, const Vec2& v) { return Vec2{v.x * s, v.y * s}; }
constexpr Vec2 operator/(const Vec2& v, float s) { return Vec2{v.x / s, v.y / s}; }

constexpr Vec2 Scale(const Vec2& a, const Vec2& b) { return Vec2{a.x * b.x, a.y * b.y}; }
constexpr float Dot(const Vec2& a, const Vec2& b) { return a.x * b.x + a.y * b.y; }
constexpr float Cross(const Vec2& a, const Vec2& b) { return a.x * b.y - a.y * b.x; }
constexpr float DistanceSquared(const Vec2& a, const Vec2& b) { return (b - a).LengthSquared(); }
inline float Distance(const Vec2& a, const Vec2& b) { return (b - a).Length(); }
constexpr Vec2 Lerp(const Vec2& a, const Vec2& b, float t) { return a + (b - a) * t; }
inline constexpr float kDefaultEpsilon = 1e-4f;

inline bool ApproxEqual(float a, float b, float epsilon = kDefaultEpsilon) { return std::fabs(a - b) <= epsilon; }
inline bool ApproxEqual(const Vec2& a, const Vec2& b, float epsilon = kDefaultEpsilon) { return ApproxEqual(a.x, b.x, epsilon) && ApproxEqual(a.y, b.y, epsilon); }

inline constexpr float kPi = 3.14159265358979323846f;
inline constexpr float kTwoPi = kPi * 2.0f;
inline constexpr float kDegToRad = kPi / 180.0f;
inline constexpr float kRadToDeg = 180.0f / kPi;

inline Vec2 FromAngle(float radians, float length = 1.0f) { return Vec2{std::cos(radians) * length, std::sin(radians) * length}; }
inline float AngleOf(const Vec2& v) { return std::atan2(v.y, v.x); }

} // namespace eng
