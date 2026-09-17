#pragma once

#include <engine/core/Json.h>
#include <engine/math/Transform2D.h>
#include <engine/render/Renderer.h>
#include <engine/render/Texture.h>
#include <engine/scene/Entity.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace eng {

class Camera;
class Entity;
class Scene;

class Component {
public:
    virtual ~Component() = default;
    virtual const char* TypeName() const = 0;
    virtual bool Deserialize(const Json& node, std::string& outError) = 0;

    virtual bool Serialize(Json& out) const {
        (void)out; // silences "unused parameter" without naming it
        return false;
    }

    virtual void OnAttach() {}
    virtual void OnDetach() {}
    Entity* Owner() const { return m_owner; }
    EntityId OwnerId() const;
    Scene* GetScene() const;
    Transform2D* OwnerTransform() const;

private:
    friend class Entity;
    Entity* m_owner = nullptr;
};

class ComponentFactory {
public:
    using CreateFn = std::unique_ptr<Component> (*)();
    static void Register(std::string_view typeName, CreateFn create);
    static std::unique_ptr<Component> Create(std::string_view typeName);
    static bool IsRegistered(std::string_view typeName);
    static void ForEachType(const std::function<void(const char*)>& fn);
    static void RegisterBuiltins();
};

class TransformComponent final : public Component {
public:
    static constexpr const char* kTypeName = "TransformComponent";
    const char* TypeName() const override { return kTypeName; }
    bool Deserialize(const Json& node, std::string& outError) override;
    bool Serialize(Json& out) const override;
    Transform2D& Transform() { return m_transform; }
    const Transform2D& Transform() const { return m_transform; }

private:
    Transform2D m_transform;
};

class SpriteComponent;

class SpriteRenderSystem {
public:
    static void Register(SpriteComponent& sprite);
    static void Unregister(SpriteComponent& sprite);
    static void Render(Camera& camera);
    static std::size_t Count();
    static void Clear();
};

class SpriteComponent final : public Component {
public:
    static constexpr const char* kTypeName = "SpriteComponent";
    ~SpriteComponent() override;
    const char* TypeName() const override { return kTypeName; }
    bool Deserialize(const Json& node, std::string& outError) override;
    bool Serialize(Json& out) const override;
    void OnAttach() override;
    void OnDetach() override;
    const TextureRef&  GetTexture() const { return m_texture; }
    const std::string& TexturePath() const { return m_texturePath; }
    Color Tint() const  { return m_tint; }
    int Layer() const { return m_layer; }
    Vec2 PixelSize() const { return m_pixelSize; }
    void SetTint(Color tint);
    void SetLayer(int layer);
    void SetTexture(std::string_view virtualPath);

private:
    TextureRef m_texture;
    std::string m_texturePath;
    Color m_tint  = Color::White();
    int m_layer = 0;
    Vec2 m_pixelSize{0.0f, 0.0f};
};

} // namespace eng