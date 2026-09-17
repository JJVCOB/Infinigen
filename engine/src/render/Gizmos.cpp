#include <engine/render/Gizmos.h>

namespace eng {

const char* ToString(GizmoCategory category) {
    return "Default";
}

void Gizmos::Line(Vec2 a, Vec2 b, Color color, float lifetimeSeconds, GizmoSpace space, GizmoCategory category) {
}

void Gizmos::Box(const AABB& box, Color color, float lifetimeSeconds, GizmoSpace space, GizmoCategory category) {
}

void Gizmos::FilledBox(const AABB& box, Color color, float lifetimeSeconds, GizmoSpace space, GizmoCategory category) {
}

void Gizmos::Circle(Vec2 centre, float radius, Color color, float lifetimeSeconds, GizmoSpace space, GizmoCategory category) {
}

void Gizmos::Text(Vec2 position, const std::string& text, Color color, float lifetimeSeconds, GizmoSpace space, GizmoCategory category) {
}

void Gizmos::TransformedBox(const Mat3& worldMatrix, Vec2 halfExtents, Color color, float lifetimeSeconds, GizmoCategory category) {
}

void Gizmos::Grid(float spacing, Color color, int halfLines) {
}

void Gizmos::OriginAxes(float length) {
}

void Gizmos::Render(Camera& camera) {
}

void Gizmos::EndFrame(float deltaSeconds) {
}

void Gizmos::Clear() {
}

void Gizmos::SetEnabled(bool on) {
}

bool Gizmos::IsEnabled() {
    return false;
}

void Gizmos::SetCategoryEnabled(GizmoCategory category, bool on) {
}

bool Gizmos::IsCategoryEnabled(GizmoCategory category) {
    return false;
}

void Gizmos::SetCircleSegments(int segments) {
}

int Gizmos::CircleSegments() {
    return 0;
}

bool Gizmos::Init(const BootConfig& config)
{
    return false;
}

void Gizmos::Shutdown() {}

} // namespace eng
