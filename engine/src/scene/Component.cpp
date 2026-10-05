#include <engine/core/Log.h>
#include <engine/render/Camera.h>
#include <engine/resource/ResourceManager.h>
#include <engine/scene/Component.h>
#include <engine/scene/Scene.h>
#include <algorithm>
#include <map>

namespace eng {
namespace {

std::map<std::string, ComponentFactory::CreateFn>& FactoryTable() {
    static std::map<std::string, ComponentFactory::CreateFn> table;
    return table;
}

std::vector<SpriteComponent*> g_sprites;

} // namespace

EntityId Component::OwnerId() const {
    return (m_owner != nullptr) ? m_owner->Id() : EntityId{};
}

Scene* Component::GetScene() const {
    return (m_owner != nullptr) ? m_owner->GetScene() : nullptr;
}

Transform2D* Component::OwnerTransform() const {
    return (m_owner != nullptr) ? &m_owner->Transform() : nullptr;
}

void ComponentFactory::Register(std::string_view typeName, CreateFn create) {
    FactoryTable()[std::string(typeName)] = create;
}

std::unique_ptr<Component> ComponentFactory::Create(std::string_view typeName) {
    const auto it = FactoryTable().find(std::string(typeName));
    if (it == FactoryTable().end()) {
        return nullptr;
    }
    return it->second();
}

bool ComponentFactory::IsRegistered(std::string_view typeName) {
    return FactoryTable().contains(std::string(typeName));
}

void ComponentFactory::ForEachType(const std::function<void(const char*)>& fn) {
    for (const auto& [name, create] : FactoryTable()) {
        fn(name.c_str());
    }
}

bool TransformComponent::Deserialize(const Json& node, std::string& outError) {
    m_transform.SetLocalPosition(ReadVec2(node, "position", Vec2{0.0f, 0.0f}, kTypeName));
    m_transform.SetLocalRotation(ReadFloat(node, "rotation", 0.0f, kTypeName));
    m_transform.SetLocalScale(ReadVec2(node, "scale", Vec2{1.0f, 1.0f}, kTypeName));

    outError.clear();
    return true;
}

bool TransformComponent::Serialize(Json& out) const {
    WriteVec2(out, "position", m_transform.LocalPosition());
    out["rotation"] = m_transform.LocalRotation();
    WriteVec2(out, "scale", m_transform.LocalScale());
    return true;
}

SpriteComponent::~SpriteComponent() {
    // Nothing to release by hand. m_texture is a shared_ptr, so it lets go of
    // its share of the texture automatically here - and if this was the last
    // sprite using that picture, the picture unloads itself.
}

bool SpriteComponent::Deserialize(const Json& node, std::string& outError) {
    if (!HasKey(node, "texture")) {
        outError = "SpriteComponent needs a \"texture\" field naming an image file";
        return false;
    }
    m_texturePath = ReadString(node, "texture", "", kTypeName);
    if (m_texturePath.empty()) {
        outError = "SpriteComponent's \"texture\" must be text, e.g. "
                   "\"textures/player.bmp\"";
        return false;
    }

    if (HasKey(node, "tint")) {
        const Json& tint = node["tint"];
        if (tint.is_array() && tint.size() == 4) {
            auto channel = [](const Json& value) {
                return static_cast<unsigned char>(
                    std::clamp(value.is_number() ? value.get<float>() : 255.0f, 0.0f, 255.0f));
            };
            m_tint = Color{channel(tint[0]), channel(tint[1]), channel(tint[2]), channel(tint[3])};
        } else {
            ENGINE_LOG_WARN(Channels::kScene, "SpriteComponent.tint should be four numbers like "
                                              "[255, 255, 255, 255]; using white");
        }
    }

    m_layer = ReadInt(node, "layer", 0, kTypeName);
    m_pixelSize = ReadVec2(node, "size", Vec2{0.0f, 0.0f}, kTypeName);

    outError.clear();
    return true;
}

bool SpriteComponent::Serialize(Json& out) const {
    out["texture"] = m_texturePath;

    Json tint = Json::array();
    tint.push_back(static_cast<int>(m_tint.r));
    tint.push_back(static_cast<int>(m_tint.g));
    tint.push_back(static_cast<int>(m_tint.b));
    tint.push_back(static_cast<int>(m_tint.a));
    out["tint"] = std::move(tint);
    out["layer"] = m_layer;

    if (m_pixelSize.x > 0.0f && m_pixelSize.y > 0.0f) {
        WriteVec2(out, "size", m_pixelSize);
    }
    return true;
}

void SpriteComponent::OnAttach() {
    if (!m_texturePath.empty()) {
        m_texture = ResourceManager::LoadTexture(m_texturePath);
    }
    SpriteRenderSystem::Register(*this);
}

void SpriteComponent::OnDetach() {
    SpriteRenderSystem::Unregister(*this);
    m_texture.reset();
}

void SpriteComponent::SetTint(Color tint) {
    m_tint = tint;
}

void SpriteComponent::SetLayer(int layer) {
    m_layer = layer;
}

void SpriteComponent::SetTexture(std::string_view virtualPath) {
    m_texture = ResourceManager::LoadTexture(virtualPath);
    m_texturePath.assign(virtualPath);
}

void SpriteRenderSystem::Register(SpriteComponent& sprite) {
    g_sprites.push_back(&sprite);
}

void SpriteRenderSystem::Unregister(SpriteComponent& sprite) {
    std::erase(g_sprites, &sprite);
}

void SpriteRenderSystem::Render(Camera& camera) {
    if (g_sprites.empty()) {
        return;
    }

    std::stable_sort(
        g_sprites.begin(), g_sprites.end(),
        [](const SpriteComponent* a, const SpriteComponent* b) { return a->Layer() < b->Layer(); });

    for (const SpriteComponent* sprite : g_sprites) {
        Transform2D* transform = sprite->OwnerTransform();
        if (transform == nullptr || !sprite->GetTexture()) {
            continue;
        }

        const Mat3 world = transform->WorldMatrix();
        const Vec2 centre = camera.WorldToScreen(world.GetTranslation());
        const Vec2 scale = world.GetScale();
        const float rotation = world.GetRotation();

        Vec2 sizePixels = sprite->PixelSize();
        if (sizePixels.x <= 0.0f || sizePixels.y <= 0.0f) {
            sizePixels = Vec2{static_cast<float>(sprite->GetTexture()->width),
                              static_cast<float>(sprite->GetTexture()->height)};
        }

        const Vec2 onScreen{sizePixels.x * scale.x * camera.Zoom(),
                            sizePixels.y * scale.y * camera.Zoom()};

        Renderer::DrawSprite(sprite->GetTexture(), centre, onScreen, rotation * kRadToDeg,
                             sprite->Tint());
    }
}

std::size_t SpriteRenderSystem::Count() {
    return g_sprites.size();
}

void SpriteRenderSystem::Clear() {
    g_sprites.clear();
}

void ComponentFactory::RegisterBuiltins() {
    Register(TransformComponent::kTypeName,
             []() -> std::unique_ptr<Component> { return std::make_unique<TransformComponent>(); });
    Register(SpriteComponent::kTypeName,
             []() -> std::unique_ptr<Component> { return std::make_unique<SpriteComponent>(); });
}

} // namespace eng