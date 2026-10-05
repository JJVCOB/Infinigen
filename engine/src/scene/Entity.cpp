#include <engine/core/Log.h>
#include <engine/scene/Component.h>
#include <engine/scene/Entity.h>
#include <engine/scene/Scene.h>
#include <algorithm>

namespace eng {

Entity::~Entity() {
    DestroyInternal();
}

void Entity::DestroyInternal() {
    if (!m_alive && m_components.empty()) {
        return;
    }
    for (auto it = m_components.rbegin(); it != m_components.rend(); ++it) {
        if (*it != nullptr) {
            (*it)->OnDetach();
        }
    }

    while (!m_components.empty()) {
        m_components.pop_back();
    }

    m_alive = false;
}

void Entity::SetName(std::string_view name) {
    m_name.assign(name);
}

Component* Entity::AddComponent(std::string_view typeName) {
    std::unique_ptr<Component> component = ComponentFactory::Create(typeName);
    if (component == nullptr) {
        ENGINE_LOG_ERROR(Channels::kScene, "entity '{}': there is no component type called '{}'",
                         m_name, typeName);
        return nullptr;
    }
    return AddComponent(std::move(component));
}

Component* Entity::AddComponent(std::unique_ptr<Component> component) {
    if (component == nullptr) {
        return nullptr;
    }

    Component* raw = component.get();
    raw->m_owner = this;

    m_components.push_back(std::move(component));

    raw->OnAttach();
    return raw;
}

Component* Entity::FindComponent(std::string_view typeName) {
    for (const std::unique_ptr<Component>& component : m_components) {
        if (component != nullptr && component->TypeName() == typeName) {
            return component.get();
        }
    }
    return nullptr;
}

const Component* Entity::FindComponent(std::string_view typeName) const {
    for (const std::unique_ptr<Component>& component : m_components) {
        if (component != nullptr && component->TypeName() == typeName) {
            return component.get();
        }
    }
    return nullptr;
}

bool Entity::RemoveComponent(std::string_view typeName) {
    const auto it =
        std::find_if(m_components.begin(), m_components.end(),
                     [typeName](const std::unique_ptr<Component>& component) {
                         return component != nullptr && component->TypeName() == typeName;
                     });
    if (it == m_components.end()) {
        return false;
    }
    (*it)->OnDetach();
    m_components.erase(it);
    return true;
}

Component* Entity::ComponentAt(std::size_t index) {
    return (index < m_components.size()) ? m_components[index].get() : nullptr;
}

void Entity::ForEachComponent(const std::function<void(Component&)>& fn) {
    for (std::size_t i = 0; i < m_components.size(); ++i) {
        if (m_components[i] != nullptr) {
            fn(*m_components[i]);
        }
    }
}

Transform2D& Entity::Transform() {
    TransformComponent* component = Find<TransformComponent>();
    if (component == nullptr) {
        component =
            static_cast<TransformComponent*>(AddComponent(std::make_unique<TransformComponent>()));
    }
    return component->Transform();
}

const Transform2D& Entity::Transform() const {
    return const_cast<Entity*>(this)->Transform();
}

} // namespace eng