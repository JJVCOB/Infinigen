#include <engine/Engine.h>

namespace eng {

Engine& Engine::Get() {
    static Engine instance;
    return instance;
}

Window& Engine::GetWindow() {
    return m_window;
}

bool Engine::RendererSubsystem::Init(const BootConfig&) {
    Engine& engine = Engine::Get();
    if (!Renderer::Init(engine.m_window))
    {
        return false;
    }
    engine.m_camera.SetViewportSize(Renderer::OutputSize());
    return true;
}

void Engine::RendererSubsystem::Shutdown() {
    Renderer::Shutdown();
}

bool Engine::GuiSubsystem::Init(const BootConfig&) {
    return m_init ? m_init() : true;
}

void Engine::GuiSubsystem::Use(std::function<bool()> init, std::function<void()> shutdown) {
    m_init = std::move(init);
    m_shutdown = std::move(shutdown);
}

void Engine::GuiSubsystem::Shutdown() {
    if (m_shutdown) {
        m_shutdown();
    }
}

bool Engine::InputSubsystem::Init(const BootConfig&) {
    const Json& document = Engine::Get().m_configDocument;

    std::string warnings;
    if (document.contains("input"))
    {
        InputMap::LoadBindings(document["input"], warnings);
    }
    else
    {
        ENGINE_LOG_WARN(Channels::kInput, "the settings file has no \"input\" section, so no controls are bound.");
    }
    InputMap::PushContext("gameplay");
    return true;
}

void Engine::InputSubsystem::Shutdown() {
    InputMap::ClearBindings();
}

bool Engine::SceneSubsystem::Init(const BootConfig&) {
    Engine& engine = Engine::Get();

    ComponentFactory::RegisterBuiltins();
    CollisionSystem::RegisterComponentTypes();
    SpinSystem::RegisterComponentTypes();
    ScriptSystem::RegisterComponentTypes();

    engine.m_spinSystem = std::make_unique<SpinSystem>();
    engine.m_scriptSystem = std::make_unique<ScriptSystem>();
    SystemScheduler::Register(engine.m_spinSystem.get());
    SystemScheduler::Register(engine.m_scriptSystem.get());

    engine.m_scene = std::make_unique<Scene>();
    Scene::SetActive(engine.m_scene.get());
    return true;
}

void Engine::SceneSubsystem::Shutdown() {
    Engine& engine = Engine::Get();

    if (engine.m_scene != nullptr) {
        engine.m_scene->Unload();
    }
    if (engine.m_spinSystem != nullptr) {
        SystemScheduler::Unregister(engine.m_spinSystem.get());
        engine.m_spinSystem.reset();
    }
    if (engine.m_scriptSystem != nullptr) {
        SystemScheduler::Unregister(engine.m_scriptSystem.get());
    }
    SpinSystem::Clear();
    ScriptSystem::Clear();
    SpriteRenderSystem::Clear();
    Scene::SetActive(nullptr);
    engine.m_scene.reset();
}

bool Engine::CollisionSubsystem::Init(const BootConfig&) {
    Engine& engine = Engine::Get();
    engine.m_collisionSystem = std::make_unique<CollisionSystem>();
    SystemScheduler::Register(engine.m_collisionSystem.get());
    ScriptSystem::SubscribeToCollisions();
    return true;
}

void Engine::CollisionSubsystem::Shutdown() {
    Engine& engine = Engine::Get();
    if (engine.m_collisionSystem != nullptr) {
        SystemScheduler::Unregister(engine.m_collisionSystem.get());
    }
    CollisionSystem::Clear();
    engine.m_collisionSystem.reset();
}

void Engine::RegisterBuiltinSubsystems(const Options& options)
{
    m_subsystems.Add("Log", m_log);
    m_subsystems.Add("FileSystem", m_fileSystem);
    m_subsystems.Add("Window", m_window);
    m_subsystems.Add("Renderer", m_renderer);
    if (options.guiInit) {
        m_gui.Use(options.guiInit, options.guiShutdown);
        m_subsystems.Add("EditorGui", m_gui);
    }
    //m_subsystems.Add("Input", m_input);
    //m_subsystems.Add("Resources", m_resources);
    //m_subsystems.Add("Gizmos", m_gizmos);
    //m_subsystems.Add("Messaging", m_messaging);
    //m_subsystems.Add("Scripts", m_scripts);
    //m_subsystems.Add("Scene", m_sceneSubsystem);
    //m_subsystems.Add("Collision", m_collisionSubsystem);
}

bool Engine::Init(const Options& options) {
    return false;
}

void Engine::Shutdown() {}

bool Engine::LoadScene(std::string_view virtualPath, std::string& outError) {
    return false;
}

bool Engine::SaveScene(std::string_view virtualPath, std::string& outError) {
    return false;
}

bool Engine::EnterPlayMode(std::string& outError) {
    return false;
}

void Engine::ExitPlayMode() {}

bool Engine::BeginFrame() {
    return false;
}

void Engine::Simulate() {}

void Engine::RenderWorld(Camera& camera, bool includeGizmos) {}

void Engine::RenderFrame() {}

void Engine::PresentFrame() {}

void Engine::Run()
{
    while (BeginFrame())
    {
        Simulate();
        RenderFrame();
        PresentFrame();
    }
}

} // namespace eng