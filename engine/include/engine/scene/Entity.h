#pragma once

#include <engine/scene/EntityId.h>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace eng {

class Component;
class Scene;
class Transform2D;

class Entity {
public:
    Entity() = default;
    ~Entity();
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    EntityId Id() const { return m_id; }
    const std::string& Name() const { return m_name; }
    Scene* GetScene() const { return m_scene; }
    void SetName(std::string_view name);
    Component* AddComponent(std::string_view typeName);
    Component* AddComponent(std::unique_ptr<Component> component);
    Component* FindComponent(std::string_view typeName);
    const Component* FindComponent(std::string_view typeName) const;
    template <typename T>
    T* Find() { return static_cast<T*>(FindComponent(T::kTypeName)); }
    template <typename T>
    const T* Find() const { return static_cast<const T*>(FindComponent(T::kTypeName)); }
    bool RemoveComponent(std::string_view typeName);
    std::size_t ComponentCount() const { return m_components.size(); }
    Component* ComponentAt(std::size_t index);
    void ForEachComponent(const std::function<void(Component&)>& fn);
    Transform2D& Transform();
    const Transform2D& Transform() const;
    bool IsAlive() const { return m_alive; }

private:
    friend class Scene;
    void DestroyInternal();
    EntityId m_id{};
    std::string m_name;
    Scene* m_scene = nullptr;
    std::vector<std::unique_ptr<Component>> m_components;
    bool m_alive = false;
};

} // namespace eng
