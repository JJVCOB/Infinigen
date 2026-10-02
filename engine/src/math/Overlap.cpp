#include <engine/math/Overlap.h>
#include <algorithm>

namespace eng {

void AABB::Encapsulate(Vec2 point) {
    min.x = std::min(min.x, point.x); min.y = std::min(min.y, point.y);
    max.x = std::max(max.x, point.x); max.y = std::max(max.y, point.y);
}

bool Overlaps(const AABB& a, const AABB& b) {
    if (a.max.x < b.min.x || b.max.x < a.min.x) { return false; }
    if (a.max.y < b.min.y || b.max.y < a.min.y) { return false; }
    return true;
}

bool Overlaps(const Circle& a, const Circle& b) {
    const float reach = a.radius + b.radius;
    return DistanceSquared(a.center, b.center) <= reach * reach;
}

bool Overlaps(const AABB& box, const Circle& circle) {
    const Vec2 closest = ClosestPointOnAABB(box, circle.center);
    return DistanceSquared(closest, circle.center) <= circle.radius * circle.radius;
}

bool Overlaps(const Circle& circle, const AABB& box) {
    return Overlaps(box, circle);
}

bool Contains(const AABB& box, Vec2 point) {
    return point.x >= box.min.x && point.x <= box.max.x && point.y >= box.min.y &&
           point.y <= box.max.y;
}

bool Contains(const Circle& circle, Vec2 point) {
    return DistanceSquared(circle.center, point) <= circle.radius * circle.radius;
}

Vec2 ClosestPointOnAABB(const AABB& box, Vec2 point) {
    return Vec2{};
}

} // namespace eng
