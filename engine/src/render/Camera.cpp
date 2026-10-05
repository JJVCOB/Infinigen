#include <engine/render/Camera.h>
#include <algorithm>

namespace eng {

void Camera::SetZoom(float zoom) {
    m_zoom = std::clamp(zoom, 0.01f, 1000.0f);
}

void Camera::Reset() {
    m_position = Vec2{0.0f, 0.0f};
    m_zoom = 1.0f;
}

Mat3 Camera::ViewMatrix() const {
    const Vec2 half{m_viewport.x * 0.5f, m_viewport.y * 0.5f};

    return Mat3::Translation(-m_position) * Mat3::Scaling(Vec2{m_zoom, -m_zoom}) *
           Mat3::Translation(half);
}

Mat3 Camera::InverseViewMatrix() const {
    return ViewMatrix().Inverse();
}

Vec2 Camera::WorldToScreen(Vec2 world) const {
    return ViewMatrix().TransformPoint(world);
}

Vec2 Camera::ScreenToWorld(Vec2 screen) const {
    return InverseViewMatrix().TransformPoint(screen);
}

Vec2 Camera::WorldToScreenVector(Vec2 world) const {
    return ViewMatrix().TransformVector(world);
}

AABB Camera::VisibleBounds() const {
    const Mat3 inverse = InverseViewMatrix();

    const Vec2 corners[4] = {
        inverse.TransformPoint(Vec2{0.0f, 0.0f}),
        inverse.TransformPoint(Vec2{m_viewport.x, 0.0f}),
        inverse.TransformPoint(Vec2{0.0f, m_viewport.y}),
        inverse.TransformPoint(Vec2{m_viewport.x, m_viewport.y}),
    };

    AABB bounds{corners[0], corners[0]};
    for (int i = 1; i < 4; ++i) {
        bounds.Encapsulate(corners[i]);
    }
    return bounds;
}

} // namespace eng
