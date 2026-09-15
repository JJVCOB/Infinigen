#pragma once

#include <engine/core/Config.h>
#include <engine/core/GameClock.h>
#include <engine/core/Json.h>
#include <engine/core/Log.h>
#include <engine/core/LogBuffer.h>
#include <engine/core/Subsystem.h>
#include <engine/fs/FileSystem.h>
#include <engine/input/InputMap.h>
#include <engine/math/Mat3.h>
#include <engine/math/Overlap.h>
#include <engine/math/Random.h>
#include <engine/math/Transform2D.h>
#include <engine/math/Vec2.h>
#include <engine/physics/Collider.h>
#include <engine/platform/EventPump.h>
#include <engine/platform/Window.h>
#include <engine/render/Camera.h>
#include <engine/render/Gizmos.h>
#include <engine/render/Renderer.h>
#include <engine/render/Texture.h>
#include <engine/resource/ResourceManager.h>
#include <engine/scene/Component.h>
#include <engine/scene/DeferredOps.h>
#include <engine/scene/Entity.h>
#include <engine/scene/EntityId.h>
#include <engine/scene/Messaging.h>
#include <engine/scene/Scene.h>
#include <engine/scene/ScriptComponent.h>
#include <engine/scene/ScriptLibrary.h>
#include <engine/scene/SpinComponent.h>
#include <engine/scene/SystemOrder.h>
#include <engine/tools/GuiHooks.h>

#include <functional>
#include <memory>
#include <string>

namespace eng {

class Engine {
public:
    struct Options {
        std::string configPath = "config/engine.json";
        std::string sceneOverride;
        std::function<bool()> guiInit;
        std::function<void()> guiShutdown;
    };

    static Engine& Get();
    bool Init(const Options& options);
    void Shutdown();
    bool BeginFrame();        // returns false when it is time to stop
    void Simulate();
    void RenderFrame();
    void PresentFrame();
    void RenderWorld(Camera& camera, bool includeGizmos);
    void Run();
    void RequestQuit() { m_quitRequested = true; }
    bool QuitRequested() const { return m_quitRequested; }
    int StepsThisFrame() const { return m_stepsThisFrame; }

    Window&           GetWindow();
    const EventPump&  Events() const { return m_events; }
    Camera&           GetCamera()    { return m_camera; }
    GameClock&        Clock()        { return m_clock; }
    Scene&            GetScene()     { return *m_scene; }
    const BootConfig& Config() const { return m_config; }

    bool LoadScene(std::string_view virtualPath, std::string& outError);
    bool SaveScene(std::string_view virtualPath, std::string& outError);
    bool EnterPlayMode(std::string& outError);
    void ExitPlayMode();
    bool IsInPlayMode() const { return m_inPlayMode; }

    bool IsInitialised() const { return m_initialised; }

private:
    Engine() = default;

    void RegisterBuiltinSubsystems(const Options& options);

    class RendererSubsystem : public Subsystem
    {
    public:
        bool Init(const BootConfig& config) override;
        void Shutdown() override;
    };

    class GuiSubsystem : public Subsystem {
    public:
        bool Init(const BootConfig& config) override;
        void Use(std::function<bool()> init, std::function<void()> shutdown);
        void Shutdown() override;

    private:
        std::function<bool()> m_init;
        std::function<void()> m_shutdown;
    };

    class InputSubsystem : public Subsystem {
    public:
        bool Init(const BootConfig& config) override;
        void Shutdown() override;
    };

    class SceneSubsystem : public Subsystem {
    public:
        bool Init(const BootConfig& config) override;
        void Shutdown() override;
    };

    class CollisionSubsystem : public Subsystem {
    public:
        bool Init(const BootConfig& config) override;
        void Shutdown() override;
    };

    Log m_log;
    FileSystem m_fileSystem;
    Window m_window;
    RendererSubsystem m_renderer;
    GuiSubsystem m_gui;

    //InputSubsystem m_input;
    //ResourceManager m_resources;
    //Gizmos m_gizmos;
    //MessageBus m_messaging;
    //ScriptLibrary m_scripts;
    //SceneSubsystem m_sceneSubsystem;
    //CollisionSubsystem m_collisionSubsystem;

    SubsystemStack m_subsystems;
    BootConfig m_config;
    Json m_configDocument = Json::object();
    //std::unique_ptr<Window> m_window;
    std::unique_ptr<Scene> m_scene;

    std::unique_ptr<CollisionSystem> m_collisionSystem;
    std::unique_ptr<SpinSystem> m_spinSystem;
    std::unique_ptr<ScriptSystem> m_scriptSystem;

    EventPump m_events;
    Camera m_camera;
    GameClock m_clock;

    double m_lastFrameTicks = 0.0;
    int m_stepsThisFrame = 0;
    bool m_initialised    = false;
    bool m_quitRequested  = false;
    bool m_inPlayMode     = false;

    std::string m_playModeSnapshot;
};

} // namespace eng