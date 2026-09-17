#pragma once

#include <engine/math/Mat3.h>
#include <engine/math/Overlap.h>
#include <engine/math/Vec2.h>
#include <engine/render/Renderer.h>
#include <engine/core/Subsystem.h>

namespace eng {

class Camera;

enum class GizmoSpace {
    World, // moves with the camera
    Screen, // stays where it is, for scores and timers
};

enum class GizmoCategory {
    Default,
    Grid,
    Axes,
    Bounds,
    Colliders,
    Count, // not a real category; it is how many there are
};

const char* ToString(GizmoCategory category);

class Gizmos : public Subsystem {
public:
    bool Init(const BootConfig& config) override;
    void Shutdown() override;
    static void Line(Vec2 a, Vec2 b, Color color, float lifetimeSeconds = 0.0f, GizmoSpace space = GizmoSpace::World, GizmoCategory category = GizmoCategory::Default);
    static void Box(const AABB& box, Color color, float lifetimeSeconds = 0.0f, GizmoSpace space = GizmoSpace::World, GizmoCategory category = GizmoCategory::Default);
    static void FilledBox(const AABB& box, Color color, float lifetimeSeconds = 0.0f, GizmoSpace space = GizmoSpace::World, GizmoCategory category = GizmoCategory::Default);
    static void Circle(Vec2 centre, float radius, Color color, float lifetimeSeconds = 0.0f, GizmoSpace space = GizmoSpace::World, GizmoCategory category = GizmoCategory::Default);
    static void Text(Vec2 position, const std::string& text, Color color, float lifetimeSeconds = 0.0f, GizmoSpace space = GizmoSpace::Screen, GizmoCategory category = GizmoCategory::Default);
    static void TransformedBox(const Mat3& worldMatrix, Vec2 halfExtents, Color color, float lifetimeSeconds = 0.0f, GizmoCategory category = GizmoCategory::Colliders);
    static void Grid(float spacing, Color color, int halfLines = 20);
    static void OriginAxes(float length = 100.0f);
    static void Render(Camera& camera);
    static void EndFrame(float deltaSeconds);
    static void Clear();
    static void SetEnabled(bool on);
    static bool IsEnabled();
    static void SetCategoryEnabled(GizmoCategory category, bool on);
    static bool IsCategoryEnabled(GizmoCategory category);
    static void SetCircleSegments(int segments);
    static int CircleSegments();
};

} // namespace eng
