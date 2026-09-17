#include <engine/math/Overlap.h>

namespace eng {

void AABB::Encapsulate(Vec2 point) {
}

bool Overlaps(const AABB& a, const AABB& b) {
    return false;
}

bool Overlaps(const Circle& a, const Circle& b) {
    return false;
}

bool Overlaps(const AABB& box, const Circle& circle) {
    return false;
}

bool Overlaps(const Circle& circle, const AABB& box) {
    return false;
}

bool Contains(const AABB& box, Vec2 point) {
    return false;
}

bool Contains(const Circle& circle, Vec2 point) {
    return false;
}

Vec2 ClosestPointOnAABB(const AABB& box, Vec2 point) {
    return Vec2{};
}

} // namespace eng
