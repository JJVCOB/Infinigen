#pragma once

#include <engine/scene/Component.h>
#include <engine/scene/SystemOrder.h>

namespace eng {

class SpinComponent final : public Component {
public:
    static constexpr const char* kTypeName = "SpinComponent";
    ~SpinComponent() override;
    const char* TypeName() const override { return kTypeName; }
    bool Deserialize(const Json& node, std::string& outError) override;
    bool Serialize(Json& out) const override;
    void OnAttach() override;
    void OnDetach() override;
    float RadiansPerSecond() const { return m_radiansPerSecond; }
    void  SetRadiansPerSecond(float rate) { m_radiansPerSecond = rate; }

private:
    float m_radiansPerSecond = 0.0f;
};

class SpinSystem final : public System {
public:
    void Update(float deltaSeconds) override;
    const char* Name() const override  { return "SpinSystem"; }
    int Order() const override { return SystemStage::kMovement; }
    static void Register(SpinComponent& spin);
    static void Unregister(SpinComponent& spin);
    static void Clear();
    static std::size_t Count();
    static void RegisterComponentTypes();
};

} // namespace eng
