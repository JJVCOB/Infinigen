#pragma once

#include <engine/math/Mat3.h>
#include <engine/math/Overlap.h>

namespace eng {

class Camera {
public:
    float Zoom() const { return m_zoom; }
    Vec2 Position() const { return m_position; }
    Vec2 ViewportSize() const { return m_viewport; }
    void SetPosition(Vec2 position) { m_position = position; }
    void Move(Vec2 delta) { m_position += delta; }
    void SetZoom(float zoom);
    void SetViewportSize(Vec2 sizePixels) { m_viewport = sizePixels; }
    void Reset();
    Mat3 ViewMatrix() const;
    Mat3 InverseViewMatrix() const;
    Vec2 WorldToScreen(Vec2 world) const;
    Vec2 ScreenToWorld(Vec2 screen) const;
    Vec2 WorldToScreenVector(Vec2 world) const;
    AABB VisibleBounds() const;

private:
    Vec2 m_position{0.0f, 0.0f};
    Vec2 m_viewport{1280.0f, 720.0f};
    float m_zoom = 1.0f;
};

} // namespace eng
