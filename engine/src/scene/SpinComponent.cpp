#include <engine/core/Log.h>
#include <engine/math/Transform2D.h>
#include <engine/scene/SpinComponent.h>
#include <algorithm>

namespace eng {
namespace {

std::vector<SpinComponent*> g_spins;

} // namespace

SpinComponent::~SpinComponent() {
    SpinSystem::Unregister(*this);
}

bool SpinComponent::Deserialize(const Json& node, std::string& outError) {
    const bool hasRadians = HasKey(node, "radiansPerSecond");
    const bool hasDegrees = HasKey(node, "degreesPerSecond");

    if (hasRadians && hasDegrees) {
        outError = "SpinComponent gives both radiansPerSecond and degreesPerSecond; "
                   "using radiansPerSecond and ignoring the other";
        m_radiansPerSecond = ReadFloat(node, "radiansPerSecond", 0.0f, kTypeName);
        return false;
    }

    if (hasRadians) {
        m_radiansPerSecond = ReadFloat(node, "radiansPerSecond", 0.0f, kTypeName);
    } else if (hasDegrees) {
        m_radiansPerSecond = ReadFloat(node, "degreesPerSecond", 0.0f, kTypeName) * kDegToRad;
    } else {
        outError = "SpinComponent needs either radiansPerSecond or degreesPerSecond";
        return false;
    }

    return true;
}

bool SpinComponent::Serialize(Json& out) const {
    out["radiansPerSecond"] = m_radiansPerSecond;
    return true;
}

void SpinComponent::OnAttach() {
    SpinSystem::Register(*this);
}

void SpinComponent::OnDetach() {
    SpinSystem::Unregister(*this);
}

void SpinSystem::Register(SpinComponent& spin) {
    g_spins.push_back(&spin);
}

void SpinSystem::Unregister(SpinComponent& spin) {
    std::erase(g_spins, &spin);
}

void SpinSystem::Clear() {
    g_spins.clear();
}
std::size_t SpinSystem::Count() {
    return g_spins.size();
}

void SpinSystem::Update(float deltaSeconds) {
    for (std::size_t i = 0; i < g_spins.size(); ++i) {
        SpinComponent* spin = g_spins[i];
        if (spin == nullptr) {
            continue;
        }
        Transform2D* transform = spin->OwnerTransform();
        if (transform == nullptr) {
            continue;
        }

        transform->Rotate(spin->RadiansPerSecond() * deltaSeconds);
    }
}

void SpinSystem::RegisterComponentTypes() {
    ComponentFactory::Register(SpinComponent::kTypeName, []() -> std::unique_ptr<Component> {
        return std::make_unique<SpinComponent>();
    });
}

} // namespace eng