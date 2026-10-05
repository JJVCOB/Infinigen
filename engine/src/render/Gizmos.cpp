#include <engine/core/Config.h>
#include <engine/render/Camera.h>
#include <engine/render/Gizmos.h>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace eng {
namespace {

enum class Shape { Line, Box, FilledBox, Circle, Text };

struct GizmoCommand {
    Vec2 a{};
    Vec2 b{};
    float remaining = 0.0f;
    Color color{};
    std::string text;
    Shape shape = Shape::Line;
    GizmoSpace space = GizmoSpace::World;
    GizmoCategory category = GizmoCategory::Default;
};

std::vector<GizmoCommand> g_commands;

bool g_enabled = true;
bool g_categoryEnabled[static_cast<int>(GizmoCategory::Count)] = {true, true, true, true, true};
int g_circleSegments = 24;

GizmoCommand& Enqueue() {
    if (g_commands.capacity() == 0) {
        g_commands.reserve(1024);
    }
    g_commands.emplace_back();
    return g_commands.back();
}

bool Visible(const GizmoCommand& command) {
    if (!g_enabled) {
        return false;
    }
    const int slot = static_cast<int>(command.category);
    return slot < 0 || slot >= static_cast<int>(GizmoCategory::Count) || g_categoryEnabled[slot];
}

Vec2 ToScreen(const Camera& camera, const GizmoCommand& command, Vec2 point) {
    return (command.space == GizmoSpace::World) ? camera.WorldToScreen(point) : point;
}

} // namespace

const char* ToString(GizmoCategory category) {
    switch (category) {
    case GizmoCategory::Default:
        return "Default";
    case GizmoCategory::Grid:
        return "Grid";
    case GizmoCategory::Axes:
        return "Origin axes";
    case GizmoCategory::Bounds:
        return "Bounds";
    case GizmoCategory::Colliders:
        return "Colliders";
    case GizmoCategory::Count:
        break;
    }
    return "?";
}

void Gizmos::Line(Vec2 a, Vec2 b, Color color, float lifetimeSeconds, GizmoSpace space,
                  GizmoCategory category) {
    GizmoCommand& command = Enqueue();
    command.shape = Shape::Line;
    command.a = a;
    command.b = b;
    command.color = color;
    command.remaining = lifetimeSeconds;
    command.space = space;
    command.category = category;
}

void Gizmos::Box(const AABB& box, Color color, float lifetimeSeconds, GizmoSpace space,
                 GizmoCategory category) {
    GizmoCommand& command = Enqueue();
    command.shape = Shape::Box;
    command.a = box.min;
    command.b = box.max;
    command.color = color;
    command.remaining = lifetimeSeconds;
    command.space = space;
    command.category = category;
}

void Gizmos::FilledBox(const AABB& box, Color color, float lifetimeSeconds, GizmoSpace space,
                       GizmoCategory category) {
    GizmoCommand& command = Enqueue();
    command.shape = Shape::FilledBox;
    command.a = box.min;
    command.b = box.max;
    command.color = color;
    command.remaining = lifetimeSeconds;
    command.space = space;
    command.category = category;
}

void Gizmos::Circle(Vec2 centre, float radius, Color color, float lifetimeSeconds, GizmoSpace space,
                    GizmoCategory category) {
    GizmoCommand& command = Enqueue();
    command.shape = Shape::Circle;
    command.a = centre;
    command.b = Vec2{radius, 0.0f};
    command.color = color;
    command.remaining = lifetimeSeconds;
    command.space = space;
    command.category = category;
}

void Gizmos::Text(Vec2 position, const std::string& text, Color color, float lifetimeSeconds,
                  GizmoSpace space, GizmoCategory category) {
    GizmoCommand& command = Enqueue();
    command.shape = Shape::Text;
    command.a = position;
    command.color = color;
    command.remaining = lifetimeSeconds;
    command.space = space;
    command.category = category;
    command.text = text;
}

void Gizmos::TransformedBox(const Mat3& worldMatrix, Vec2 halfExtents, Color color,
                            float lifetimeSeconds, GizmoCategory category) {
    const Vec2 corners[4] = {
        worldMatrix.TransformPoint(Vec2{-halfExtents.x, -halfExtents.y}),
        worldMatrix.TransformPoint(Vec2{halfExtents.x, -halfExtents.y}),
        worldMatrix.TransformPoint(Vec2{halfExtents.x, halfExtents.y}),
        worldMatrix.TransformPoint(Vec2{-halfExtents.x, halfExtents.y}),
    };
    for (int i = 0; i < 4; ++i) {
        Line(corners[i], corners[(i + 1) % 4], color, lifetimeSeconds, GizmoSpace::World, category);
    }
}

void Gizmos::Grid(float spacing, Color color, int halfLines) {
    if (spacing <= 0.0f) {
        return;
    }
    const float extent = spacing * static_cast<float>(halfLines);
    for (int i = -halfLines; i <= halfLines; ++i) {
        const float offset = spacing * static_cast<float>(i);
        Line(Vec2{offset, -extent}, Vec2{offset, extent}, color, 0.0f, GizmoSpace::World,
             GizmoCategory::Grid);
        Line(Vec2{-extent, offset}, Vec2{extent, offset}, color, 0.0f, GizmoSpace::World,
             GizmoCategory::Grid);
    }
}

void Gizmos::OriginAxes(float length) {
    Line(Vec2{0.0f, 0.0f}, Vec2{length, 0.0f}, Color::Red(), 0.0f, GizmoSpace::World,
         GizmoCategory::Axes);
    Line(Vec2{0.0f, 0.0f}, Vec2{0.0f, length}, Color::Green(), 0.0f, GizmoSpace::World,
         GizmoCategory::Axes);
}

void Gizmos::Render(Camera& camera) {
    for (const GizmoCommand& command : g_commands) {
        if (!Visible(command)) {
            continue;
        }

        switch (command.shape) {
        case Shape::Line:
            Renderer::DrawLine(ToScreen(camera, command, command.a),
                               ToScreen(camera, command, command.b), command.color);
            break;

        case Shape::Box:
        case Shape::FilledBox: {
            const Vec2 p0 = ToScreen(camera, command, command.a);
            const Vec2 p1 = ToScreen(camera, command, command.b);

            const Vec2 lo{std::min(p0.x, p1.x), std::min(p0.y, p1.y)};
            const Vec2 hi{std::max(p0.x, p1.x), std::max(p0.y, p1.y)};

            if (command.shape == Shape::Box) {
                Renderer::DrawRect(lo, hi, command.color);
            } else {
                Renderer::DrawFilledRect(lo, hi, command.color);
            }
            break;
        }

        case Shape::Circle: {
            const Vec2 centre = ToScreen(camera, command, command.a);

            const float radius = (command.space == GizmoSpace::World)
                                     ? camera.WorldToScreenVector(Vec2{command.b.x, 0.0f}).x
                                     : command.b.x;

            const int segments = std::max(3, g_circleSegments);
            Vec2 previous{centre.x + radius, centre.y};
            for (int i = 1; i <= segments; ++i) {
                const float angle = kTwoPi * static_cast<float>(i) / static_cast<float>(segments);
                const Vec2 next{centre.x + std::cos(angle) * radius,
                                centre.y + std::sin(angle) * radius};
                Renderer::DrawLine(previous, next, command.color);
                previous = next;
            }
            break;
        }

        case Shape::Text:
            Renderer::DrawText(ToScreen(camera, command, command.a), command.text.c_str(),
                               command.color);
            break;
        }
    }
}

void Gizmos::EndFrame(float deltaSeconds) {
    for (GizmoCommand& command : g_commands) {
        command.remaining -= deltaSeconds;
    }

    std::erase_if(g_commands,
                  [](const GizmoCommand& command) { return command.remaining <= 0.0f; });
}

void Gizmos::Clear() {
    g_commands.clear();
}

void Gizmos::SetEnabled(bool on) {
    g_enabled = on;
}
bool Gizmos::IsEnabled() {
    return g_enabled;
}

void Gizmos::SetCategoryEnabled(GizmoCategory category, bool on) {
    const int slot = static_cast<int>(category);
    if (slot >= 0 && slot < static_cast<int>(GizmoCategory::Count)) {
        g_categoryEnabled[slot] = on;
    }
}

bool Gizmos::IsCategoryEnabled(GizmoCategory category) {
    const int slot = static_cast<int>(category);
    if (slot < 0 || slot >= static_cast<int>(GizmoCategory::Count)) {
        return true;
    }
    return g_categoryEnabled[slot];
}

bool Gizmos::Init(const BootConfig& config) {
    SetCircleSegments(config.gizmoCircleSegments);
    return true;
}

void Gizmos::Shutdown() {
    Clear();
}

void Gizmos::SetCircleSegments(int segments) {
    g_circleSegments = std::clamp(segments, 3, 128);
}

int Gizmos::CircleSegments() {
    return g_circleSegments;
}

} // namespace eng