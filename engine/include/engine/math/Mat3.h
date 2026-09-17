#pragma once

#include <engine/math/Vec2.h>

namespace eng {

struct Mat3 {
    float m[3][3]{};
    static Mat3 Identity();
    static Mat3 Translation(Vec2 t); // move by t
    static Mat3 Rotation(float radians); // turn anticlockwise
    static Mat3 Scaling(Vec2 s); // resize
    static Mat3 FromTRS(Vec2 translation, float radians, Vec2 scale);
    Vec2 TransformPoint(Vec2 point) const;
    Vec2 TransformVector(Vec2 direction) const;
    Mat3 Inverse() const;
    Vec2 GetTranslation() const;
    Vec2 GetScale() const;
    float GetRotation() const; // radians, anticlockwise
};

Mat3 operator*(const Mat3& a, const Mat3& b);
bool ApproxEqual(const Mat3& a, const Mat3& b, float epsilon = kDefaultEpsilon);

} // namespace eng
