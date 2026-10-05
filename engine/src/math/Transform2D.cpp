#include <engine/core/Log.h>
#include <engine/math/Transform2D.h>
#include <algorithm>

namespace eng {

Transform2D::~Transform2D() {
    DetachChildren();
    if (m_parent != nullptr) {
        m_parent->RemoveChild(this);
        m_parent = nullptr;
    }
}

void Transform2D::AddChild(Transform2D* child) {
    m_children.push_back(child);
}

void Transform2D::RemoveChild(Transform2D* child) {
    std::erase(m_children, child);
}

void Transform2D::SetParent(Transform2D* parent, bool keepWorldTransform) {
    if (parent == m_parent) {
        return;
    }

    if (parent != nullptr) {
        if (parent == this || parent->IsDescendantOf(this)) {
            ENGINE_LOG_ERROR(Channels::kScene,
                             "refused to reparent a transform under itself or one of "
                             "its own children - that would make a loop");
            return;
        }
    }

    const Mat3 worldBefore = keepWorldTransform ? WorldMatrix() : Mat3::Identity();

    if (m_parent != nullptr) {
        m_parent->RemoveChild(this);
    }
    m_parent = parent;
    if (m_parent != nullptr) {
        m_parent->AddChild(this);
    }

    if (keepWorldTransform) {
        const Mat3 parentWorld = (m_parent != nullptr) ? m_parent->WorldMatrix() : Mat3::Identity();
        const Mat3 local = worldBefore * parentWorld.Inverse();
        m_position = local.GetTranslation();
        m_rotation = local.GetRotation();
        m_scale = local.GetScale();
    }
}

void Transform2D::DetachChildren() {
    const std::vector<Transform2D*> children = m_children;
    for (Transform2D* child : children) {
        child->SetParent(nullptr, true);
    }
    m_children.clear();
}

int Transform2D::Depth() const {
    int depth = 0;
    for (const Transform2D* node = m_parent; node != nullptr; node = node->m_parent) {
        ++depth;
    }
    return depth;
}

bool Transform2D::IsDescendantOf(const Transform2D* candidate) const {
    if (candidate == nullptr) {
        return false;
    }
    for (const Transform2D* node = m_parent; node != nullptr; node = node->m_parent) {
        if (node == candidate) {
            return true;
        }
    }
    return false;
}

Mat3 Transform2D::LocalMatrix() const {
    return Mat3::FromTRS(m_position, m_rotation, m_scale);
}

Mat3 Transform2D::WorldMatrix() const {
    Mat3 result = LocalMatrix();
    for (const Transform2D* node = m_parent; node != nullptr; node = node->m_parent) {
        result = result * node->LocalMatrix();
    }
    return result;
}

Vec2 Transform2D::WorldPosition() const {
    return WorldMatrix().GetTranslation();
}

float Transform2D::WorldRotation() const {
    float total = m_rotation;
    for (const Transform2D* node = m_parent; node != nullptr; node = node->m_parent) {
        total += node->m_rotation;
    }
    return total;
}

Vec2 Transform2D::WorldScale() const {
    return WorldMatrix().GetScale();
}

void Transform2D::SetWorldPosition(Vec2 world) {
    if (m_parent == nullptr) {
        m_position = world;
        return;
    }
    m_position = m_parent->WorldMatrix().Inverse().TransformPoint(world);
}

Vec2 Transform2D::LocalToWorldPoint(Vec2 local) const {
    return WorldMatrix().TransformPoint(local);
}

Vec2 Transform2D::WorldToLocalPoint(Vec2 world) const {
    return WorldMatrix().Inverse().TransformPoint(world);
}

Vec2 Transform2D::LocalToWorldVector(Vec2 local) const {
    return WorldMatrix().TransformVector(local);
}

Vec2 Transform2D::WorldToLocalVector(Vec2 world) const {
    return WorldMatrix().Inverse().TransformVector(world);
}

} // namespace eng
