#include <engine/math/Transform2D.h>

namespace eng {

Transform2D::~Transform2D() {
}

void Transform2D::AddChild(Transform2D* child) {
}

void Transform2D::RemoveChild(Transform2D* child) {
}

void Transform2D::SetParent(Transform2D* parent, bool keepWorldTransform) {
}

void Transform2D::DetachChildren() {
}

int Transform2D::Depth() const {
    return 0;
}

bool Transform2D::IsDescendantOf(const Transform2D* candidate) const {
    return false;
}

Mat3 Transform2D::LocalMatrix() const {
    return Mat3::Identity();
}

Mat3 Transform2D::WorldMatrix() const {
    return Mat3::Identity();
}

Vec2 Transform2D::WorldPosition() const {
    return Vec2{};
}

float Transform2D::WorldRotation() const {
    return 0.0f;
}

Vec2 Transform2D::WorldScale() const {
    return Vec2{1.0f, 1.0f};
}

void Transform2D::SetWorldPosition(Vec2 world) {
}

Vec2 Transform2D::LocalToWorldPoint(Vec2 local) const {
    return Vec2{};
}

Vec2 Transform2D::WorldToLocalPoint(Vec2 world) const {
    return Vec2{};
}

Vec2 Transform2D::LocalToWorldVector(Vec2 local) const {
    return Vec2{};
}

Vec2 Transform2D::WorldToLocalVector(Vec2 world) const {
    return Vec2{};
}

} // namespace eng
