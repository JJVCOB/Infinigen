#pragma once

#include <engine/math/Vec2.h>

namespace eng {

struct AABB {
    Vec2 min; // bottom-left
    Vec2 max; // top-right
    static constexpr AABB FromCenterHalfExtents(Vec2 center, Vec2 halfExtents) { return AABB{Vec2{center.x - halfExtents.x, center.y - halfExtents.y}, Vec2{center.x + halfExtents.x, center.y + halfExtents.y}}; }
    static constexpr AABB FromMinMax(Vec2 lo, Vec2 hi) { return AABB{lo, hi}; }
    constexpr Vec2 Center() const { return Vec2{(min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f}; }
    constexpr Vec2 Size() const { return Vec2{max.x - min.x, max.y - min.y}; }
    constexpr Vec2 Extents() const { return Vec2{Size().x * 0.5f, Size().y * 0.5f}; }
    constexpr bool IsValid() const { return min.x <= max.x && min.y <= max.y; }
    void Encapsulate(Vec2 point);
};

struct Circle {
    Vec2  center;
    float radius = 0.0f;
};

bool Overlaps(const AABB& a, const AABB& b);
bool Overlaps(const Circle& a, const Circle& b);
bool Overlaps(const AABB& box, const Circle& circle);
bool Overlaps(const Circle& circle, const AABB& box);
bool Contains(const AABB& box, Vec2 point);
bool Contains(const Circle& circle, Vec2 point);
Vec2 ClosestPointOnAABB(const AABB& box, Vec2 point);

} // namespace eng
