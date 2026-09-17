#pragma once

#include <engine/math/Overlap.h>
#include <engine/scene/Component.h>
#include <engine/scene/SystemOrder.h>
#include <string>
#include <vector>

namespace eng {

inline constexpr const char* kCollisionLayerAll = "All";

enum class ColliderShape { Box, Circle };

class ColliderComponent : public Component {
public:
    bool Deserialize(const Json& node, std::string& outError) override;
    bool Serialize(Json& out) const override;
    void OnAttach() override;
    void OnDetach() override;
    virtual ColliderShape Shape() const = 0;
    const std::string& Layer() const { return m_layer; }
    const std::vector<std::string>& CollidesWith() const { return m_collidesWith; }
    void SetLayer(std::string_view layer) { m_layer = std::string(layer); }
    void SetCollidesWith(std::vector<std::string> layers) { m_collidesWith = std::move(layers); }
    bool CaresAbout(const std::string& layer) const;
    bool IsTrigger() const { return m_trigger; }
    void SetTrigger(bool trigger) { m_trigger = trigger; }
    Vec2 Offset() const { return m_offset; }
    void SetOffset(Vec2 offset) { m_offset = offset; }
    virtual AABB WorldBounds() const = 0;

protected:
    std::string m_layer = "Default";
    std::vector<std::string> m_collidesWith{kCollisionLayerAll};
    Vec2 m_offset{0.0f, 0.0f};
    bool m_trigger = false;
};

class AABBColliderComponent final : public ColliderComponent {
public:
    static constexpr const char* kTypeName = "AABBColliderComponent";
    const char* TypeName() const override { return kTypeName; }
    ColliderShape Shape() const override { return ColliderShape::Box; }
    bool Deserialize(const Json& node, std::string& outError) override;
    bool Serialize(Json& out) const override;
    AABB WorldBounds() const override;
    Vec2 HalfExtents() const { return m_halfExtents; }
    void SetHalfExtents(Vec2 halfExtents) { m_halfExtents = halfExtents; }

private:
    Vec2 m_halfExtents{0.5f, 0.5f};
};

class CircleColliderComponent final : public ColliderComponent {
public:
    static constexpr const char* kTypeName = "CircleColliderComponent";
    const char*   TypeName() const override { return kTypeName; }
    ColliderShape Shape() const override    { return ColliderShape::Circle; }
    bool Deserialize(const Json& node, std::string& outError) override;
    bool Serialize(Json& out) const override;
    AABB WorldBounds() const override;
    Circle WorldCircle() const;
    float Radius() const { return m_radius; }
    void  SetRadius(float radius) { m_radius = radius; }

private:
    float m_radius = 0.5f;
};

class CollisionSystem final : public System {
public:
    void Update(float deltaSeconds) override;
    const char* Name() const override  { return "CollisionSystem"; }
    int Order() const override { return SystemStage::kCollision; }
    static void Register(ColliderComponent& collider);
    static void Unregister(ColliderComponent& collider);
    static void Clear();
    static std::size_t ColliderCount();
    static std::size_t ActivePairCount(); // how many pairs are touching now
    static void RegisterComponentTypes();
};

} // namespace eng
