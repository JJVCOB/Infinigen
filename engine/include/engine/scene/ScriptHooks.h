#pragma once

#include <engine/scene/EntityId.h>
#include <string>
#include <type_traits>

namespace eng {

class ScriptBehaviour;

namespace hooks {

template <class T> concept HasOnStart = requires(T& t) { t.OnStart(); };
template <class T> concept HasOnUpdate = requires(T& t, float dt) { t.OnUpdate(dt); };
template <class T> concept HasOnDestroy = requires(T& t) { t.OnDestroy(); };
template <class T>
concept HasOnCollisionEnter = requires(T& t, EntityId other) { t.OnCollisionEnter(other); };
template <class T>
concept HasOnCollisionStay = requires(T& t, EntityId other) { t.OnCollisionStay(other); };
template <class T>
concept HasOnCollisionExit = requires(T& t, EntityId other) { t.OnCollisionExit(other); };
template <class T> concept NamesOnStart = requires { &T::OnStart; };
template <class T> concept NamesOnUpdate = requires { &T::OnUpdate; };
template <class T> concept NamesOnDestroy = requires { &T::OnDestroy; };
template <class T> concept NamesOnCollisionEnter = requires { &T::OnCollisionEnter; };
template <class T> concept NamesOnCollisionStay = requires { &T::OnCollisionStay; };
template <class T> concept NamesOnCollisionExit = requires { &T::OnCollisionExit; };
template <class T> concept NamesUpdate = requires { &T::Update; };
template <class T> concept NamesFixedUpdate = requires { &T::FixedUpdate; };
template <class T> concept NamesStart = requires { &T::Start; };
template <class T> concept NamesAwake = requires { &T::Awake; };

} // namespace hooks

struct ScriptHooks {
    void (*start)(ScriptBehaviour*) = nullptr;
    void (*update)(ScriptBehaviour*, float) = nullptr;
    void (*destroy)(ScriptBehaviour*) = nullptr;
    void (*collisionEnter)(ScriptBehaviour*, EntityId) = nullptr;
    void (*collisionStay)(ScriptBehaviour*, EntityId) = nullptr;
    void (*collisionExit)(ScriptBehaviour*, EntityId) = nullptr;
    bool AnyCollision() const { return collisionEnter != nullptr || collisionStay != nullptr || collisionExit != nullptr; }
};

template <class T>
constexpr ScriptHooks MakeScriptHooks() {
    ScriptHooks h;
    if constexpr (hooks::HasOnStart<T>) {
        h.start = [](ScriptBehaviour* s) { static_cast<T*>(s)->OnStart(); };
    }
    if constexpr (hooks::HasOnUpdate<T>) {
        h.update = [](ScriptBehaviour* s, float dt) { static_cast<T*>(s)->OnUpdate(dt); };
    }
    if constexpr (hooks::HasOnDestroy<T>) {
        h.destroy = [](ScriptBehaviour* s) { static_cast<T*>(s)->OnDestroy(); };
    }
    if constexpr (hooks::HasOnCollisionEnter<T>) {
        h.collisionEnter = [](ScriptBehaviour* s, EntityId o) {
            static_cast<T*>(s)->OnCollisionEnter(o);
        };
    }
    if constexpr (hooks::HasOnCollisionStay<T>) {
        h.collisionStay = [](ScriptBehaviour* s, EntityId o) {
            static_cast<T*>(s)->OnCollisionStay(o);
        };
    }
    if constexpr (hooks::HasOnCollisionExit<T>) {
        h.collisionExit = [](ScriptBehaviour* s, EntityId o) {
            static_cast<T*>(s)->OnCollisionExit(o);
        };
    }
    return h;
}

std::string DescribeHooks(const ScriptHooks& hooks);

} // namespace eng

// i'm genuinely not even going to touch this bro

#define ENGINE_REGISTER_SCRIPT(Type)                                                   \
    /* --- is this even a script? ------------------------------------------ */        \
    static_assert(std::is_base_of_v<::eng::ScriptBehaviour, Type>,                     \
                  #Type " must inherit from eng::ScriptBehaviour: "                     \
                        "class " #Type " : public eng::ScriptBehaviour { ... };");      \
    static_assert(std::is_default_constructible_v<Type>,                               \
                  #Type " needs a constructor that takes no arguments, because the "    \
                        "engine creates it for you when a scene loads.");               \
    /* --- right name, wrong signature -------------------------------------- */       \
    static_assert(!(::eng::hooks::NamesOnUpdate<Type> &&                                \
                    !::eng::hooks::HasOnUpdate<Type>),                                  \
                  #Type "::OnUpdate has the wrong signature. It must be: "              \
                        "void OnUpdate(float deltaSeconds)");                           \
    static_assert(!(::eng::hooks::NamesOnStart<Type> && !::eng::hooks::HasOnStart<Type>),\
                  #Type "::OnStart has the wrong signature. It must be: "               \
                        "void OnStart()");                                              \
    static_assert(!(::eng::hooks::NamesOnDestroy<Type> &&                               \
                    !::eng::hooks::HasOnDestroy<Type>),                                 \
                  #Type "::OnDestroy has the wrong signature. It must be: "             \
                        "void OnDestroy()");                                            \
    static_assert(!(::eng::hooks::NamesOnCollisionEnter<Type> &&                        \
                    !::eng::hooks::HasOnCollisionEnter<Type>),                          \
                  #Type "::OnCollisionEnter has the wrong signature. It must be: "      \
                        "void OnCollisionEnter(eng::EntityId other)");                  \
    static_assert(!(::eng::hooks::NamesOnCollisionStay<Type> &&                         \
                    !::eng::hooks::HasOnCollisionStay<Type>),                           \
                  #Type "::OnCollisionStay has the wrong signature. It must be: "       \
                        "void OnCollisionStay(eng::EntityId other)");                   \
    static_assert(!(::eng::hooks::NamesOnCollisionExit<Type> &&                         \
                    !::eng::hooks::HasOnCollisionExit<Type>),                           \
                  #Type "::OnCollisionExit has the wrong signature. It must be: "       \
                        "void OnCollisionExit(eng::EntityId other)");                   \
    /* --- the names other engines use --------------------------------------- */      \
    static_assert(!::eng::hooks::NamesUpdate<Type>,                                     \
                  "This engine calls it OnUpdate(float), not Update. Rename " #Type     \
                  "::Update to OnUpdate.");                                             \
    static_assert(!::eng::hooks::NamesFixedUpdate<Type>,                                \
                  "This engine has one update, and it is already fixed-rate. Rename "   \
                  #Type "::FixedUpdate to OnUpdate(float).");                           \
    static_assert(!::eng::hooks::NamesStart<Type>,                                      \
                  "This engine calls it OnStart(), not Start. Rename " #Type            \
                  "::Start to OnStart.");                                               \
    static_assert(!::eng::hooks::NamesAwake<Type>,                                      \
                  "This engine has no Awake. The first hook is OnStart(), which runs "  \
                  "on the first simulation step after the script is attached.");        \
    /* --- the catch-all, deliberately LAST ---------------------------------- */       \
    /* Every check above names one specific mistake, and a specific message is */       \
    /* worth more than a general one - so this general one is the last to fire. */      \
    static_assert(::eng::hooks::HasOnStart<Type> || ::eng::hooks::HasOnUpdate<Type> ||  \
                      ::eng::hooks::HasOnDestroy<Type> ||                               \
                      ::eng::hooks::HasOnCollisionEnter<Type> ||                        \
                      ::eng::hooks::HasOnCollisionStay<Type> ||                         \
                      ::eng::hooks::HasOnCollisionExit<Type>,                           \
                  #Type " has none of the lifecycle functions, so nothing would ever "  \
                        "call it. Two things to check: the SPELLING (OnStart, "         \
                        "OnUpdate, OnDestroy, OnCollisionEnter/Stay/Exit), and that "   \
                        "they are PUBLIC - the engine calls them from outside the "     \
                        "class, so a private one cannot be seen.");                     \
    namespace {                                                                        \
    struct Type##_Registrar {                                                          \
        Type##_Registrar() {                                                           \
            ::eng::ScriptRegistry::Register(                                           \
                #Type,                                                                 \
                []() -> std::unique_ptr<::eng::ScriptBehaviour> {                      \
                    return std::make_unique<Type>();                                   \
                },                                                                     \
                ::eng::MakeScriptHooks<Type>(),                                        \
                __FILE__);                                                             \
        }                                                                              \
    };                                                                                 \
    const Type##_Registrar g_##Type##_registrar;                                       \
    }


