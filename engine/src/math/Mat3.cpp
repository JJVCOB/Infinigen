#include <engine/core/Log.h>
#include <engine/math/Mat3.h>
#include <cmath>

namespace eng {

Mat3 Mat3::Identity() {
    Mat3 result;
    result.m[0][0] = 1.0f; result.m[0][1] = 0.0f; result.m[0][2] = 0.0f;
    result.m[1][0] = 0.0f; result.m[1][1] = 1.0f; result.m[1][2] = 0.0f;
    result.m[2][0] = 0.0f; result.m[2][1] = 0.0f; result.m[2][2] = 1.0f;
    return result;
}

Mat3 Mat3::Translation(Vec2 t) {
    Mat3 result = Identity();
    result.m[2][0] = t.x;
    result.m[2][1] = t.y;
    return result;
}

Mat3 Mat3::Rotation(float radians) {
    const float c = std::cos(radians);
    const float s = std::sin(radians);
    Mat3 result = Identity();
    result.m[0][0] = c; result.m[0][1] = s;
    result.m[1][0] = -s; result.m[1][1] = c;
    return result;
}

Mat3 Mat3::Scaling(Vec2 s) {
    Mat3 result = Identity();
    result.m[0][0] = s.x;
    result.m[1][1] = s.y;
    return result;
}

Mat3 Mat3::FromTRS(Vec2 translation, float radians, Vec2 scale) {
    const float c = std::cos(radians);
    const float s = std::sin(radians);
    Mat3 result;
    result.m[0][0] = scale.x * c; result.m[0][1] = scale.x * s; result.m[0][2] = 0.0f;
    result.m[1][0] = scale.y * -s; result.m[1][1] = scale.y * c; result.m[1][2] = 0.0f;
    result.m[2][0] = translation.x; result.m[2][1] = translation.y; result.m[2][2] = 1.0f;
    return result;
}

Vec2 Mat3::TransformPoint(Vec2 point) const {
    return Vec2{point.x * m[0][0] + point.y * m[1][0] + m[2][0],
                point.x * m[0][1] + point.y * m[1][1] + m[2][1]};
}

Vec2 Mat3::TransformVector(Vec2 direction) const {
    return Vec2{direction.x * m[0][0] + direction.y * m[1][0],
                direction.x * m[0][1] + direction.y * m[1][1]};
}

Mat3 Mat3::Inverse() const {
    const float a = m[0][0]; const float b = m[0][1];
    const float c = m[1][0]; const float d = m[1][1];

    const float determinant = a * d - b * c;
    if (std::fabs(determinant) < 1e-12f) {
        ENGINE_LOG_WARN(Channels::kCore, "Mat3::Inverse called on a matrix that cannot be undone "
                                         "(is something scaled to zero?); returning identity");
        return Identity();
    }

    const float invDet = 1.0f / determinant;
    Mat3 result = Identity();
    result.m[0][0] = d * invDet; result.m[0][1] = -b * invDet;
    result.m[1][0] = -c * invDet; result.m[1][1] = a * invDet;

    const float tx = m[2][0];
    const float ty = m[2][1];
    result.m[2][0] = -(tx * result.m[0][0] + ty * result.m[1][0]);
    result.m[2][1] = -(tx * result.m[0][1] + ty * result.m[1][1]);

    return result;
}

Vec2 Mat3::GetTranslation() const {
    return Vec2{m[2][0], m[2][1]};
}

Vec2 Mat3::GetScale() const {
    const Vec2 rowX{m[0][0], m[0][1]};
    const Vec2 rowY{m[1][0], m[1][1]};
    return Vec2{rowX.Length(), rowY.Length()};
}

float Mat3::GetRotation() const {
    return std::atan2(m[0][1], m[0][0]);
}

Mat3 operator*(const Mat3& a, const Mat3& b) {
    Mat3 result;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            result.m[row][col] = a.m[row][0] * b.m[0][col] + a.m[row][1] * b.m[1][col] + a.m[row][2] * b.m[2][col];
        }
    }
    return result;
}

bool ApproxEqual(const Mat3& a, const Mat3& b, float epsilon) {
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (!ApproxEqual(a.m[row][col], b.m[row][col], epsilon)) {
                return false;
            }
        }
    }
    return true;
}

} // namespace eng
