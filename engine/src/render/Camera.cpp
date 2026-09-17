#include <engine/render/Camera.h>

namespace eng {

void Camera::SetZoom(float zoom) {
}

void Camera::Reset() {
}

Mat3 Camera::ViewMatrix() const {
    return Mat3::Identity();
}

Mat3 Camera::InverseViewMatrix() const {
    return Mat3::Identity();
}

Vec2 Camera::WorldToScreen(Vec2 world) const {
    return Vec2{};
}

Vec2 Camera::ScreenToWorld(Vec2 screen) const {
    return Vec2{};
}

Vec2 Camera::WorldToScreenVector(Vec2 world) const {
    return Vec2{};
}

AABB Camera::VisibleBounds() const {
    return AABB{};
}

} // namespace eng
