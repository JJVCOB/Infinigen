#pragma once

#include <engine/math/Mat3.h>
#include <vector>

namespace eng {

class Transform2D {
public:
    Transform2D() = default;
    ~Transform2D();
    Transform2D(const Transform2D&) = delete;
    Transform2D& operator=(const Transform2D&) = delete;
    Vec2  LocalPosition() const { return m_position; }
    float LocalRotation() const { return m_rotation; } // radians, anticlockwise
    Vec2  LocalScale() const { return m_scale; }
    void SetLocalPosition(Vec2 position) { m_position = position; }
    void SetLocalRotation(float radians) { m_rotation = radians; }
    void SetLocalScale(Vec2 scale) { m_scale = scale; }
    void Translate(Vec2 delta) { m_position += delta; }
    void Rotate(float radians) { m_rotation += radians; }
    Transform2D* Parent() const { return m_parent; }
    const std::vector<Transform2D*>& Children() const { return m_children; }
    void SetParent(Transform2D* parent, bool keepWorldTransform = false);
    void DetachChildren();
    int  Depth() const;
    bool IsDescendantOf(const Transform2D* candidate) const;
    Mat3 LocalMatrix() const;
    Mat3 WorldMatrix() const;
    Vec2  WorldPosition() const;
    float WorldRotation() const;
    Vec2  WorldScale() const;
    void SetWorldPosition(Vec2 world);
    Vec2 LocalToWorldPoint(Vec2 local) const;
    Vec2 WorldToLocalPoint(Vec2 world) const;
    Vec2 LocalToWorldVector(Vec2 local) const;
    Vec2 WorldToLocalVector(Vec2 world) const;

private:
    void AddChild(Transform2D* child);
    void RemoveChild(Transform2D* child);
    Vec2  m_position{0.0f, 0.0f};
    float m_rotation = 0.0f;
    Vec2  m_scale{1.0f, 1.0f};
    Transform2D* m_parent = nullptr;
    std::vector<Transform2D*> m_children;
};

} // namespace eng
