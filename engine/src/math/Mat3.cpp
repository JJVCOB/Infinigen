#include <engine/math/Mat3.h>

namespace eng {

Mat3 Mat3::Identity() {
    return Mat3{};
}

Mat3 Mat3::Translation(Vec2 t) {
    return Mat3{};
}

Mat3 Mat3::Rotation(float radians) {
    return Mat3{};
}

Mat3 Mat3::Scaling(Vec2 s) {
    return Mat3{};
}

Mat3 Mat3::FromTRS(Vec2 translation, float radians, Vec2 scale) {
    return Mat3{};
}

Vec2 Mat3::TransformPoint(Vec2 point) const {
    return Vec2{};
}

Vec2 Mat3::TransformVector(Vec2 direction) const {
    return Vec2{};
}

Mat3 Mat3::Inverse() const {
    return Mat3{};
}

Vec2 Mat3::GetTranslation() const {
    return Vec2{};
}

Vec2 Mat3::GetScale() const {
    return Vec2{};
}

float Mat3::GetRotation() const {
    return 0.0f;
}

Mat3 operator*(const Mat3& a, const Mat3& b) {
    return Mat3{};
}

bool ApproxEqual(const Mat3& a, const Mat3& b, float epsilon) {
    return false;
}

} // namespace eng
