#include <engine/core/Log.h>
#include <engine/fs/FileSystem.h>
#include <engine/scene/ScriptComponent.h>
#include <engine/scene/ScriptLibrary.h>
#include <SDL3/SDL.h>

namespace eng {
namespace {

SDL_SharedObject* g_handle = nullptr;
std::string g_loadedPath;

} // namespace

std::string ScriptLibrary::DefaultVirtualPath() {
#if defined(_WIN32)
    return ".build/userContent.dll";
#else
    return ".build/userContent.so";
#endif
}

bool ScriptLibrary::Init(const BootConfig&) {
    std::string error;
    return Load(DefaultVirtualPath(), error);
}

void ScriptLibrary::Shutdown() {
    Unload();
}

bool ScriptLibrary::Load(std::string_view virtualPath, std::string& outError) {
    Unload();

    if (!FileSystem::Exists(virtualPath)) {
        outError.clear();
        ENGINE_LOG_INFO(Channels::kScene,
                        "no compiled scripts found at '{}' - write one in the Assets "
                        "panel and the editor will build it",
                        virtualPath);
        return true;
    }

    const std::string realPath = FileSystem::Resolve(virtualPath);

    g_handle = SDL_LoadObject(realPath.c_str());
    if (g_handle == nullptr) {
        outError = std::string("could not load '") + realPath + "': " + SDL_GetError();
        ENGINE_LOG_ERROR(Channels::kScene, "{}", outError);
        return false;
    }

    g_loadedPath.assign(virtualPath);

    ENGINE_LOG_INFO(Channels::kScene, "loaded {} script(s) from '{}'", ScriptRegistry::Count(),
                    virtualPath);
    ScriptRegistry::ForEachEntry([](const char* name, const ScriptRegistry::Entry& e) {
        const std::string hooks = DescribeHooks(e.hooks);
        if (e.hooks.start == nullptr && e.hooks.update == nullptr && e.hooks.destroy == nullptr &&
            !e.hooks.AnyCollision()) {
            ENGINE_LOG_WARN(Channels::kScene, "    script '{}' has {}", name, hooks);
        } else {
            ENGINE_LOG_INFO(Channels::kScene, "    script '{}' - {}", name, hooks);
        }
    });

    ScriptSystem::RebindAll();

    outError.clear();
    return true;
}

void ScriptLibrary::Unload() {
    if (g_handle == nullptr) {
        ScriptSystem::UnbindAll();
        ScriptRegistry::Clear();
        return;
    }

    ScriptSystem::UnbindAll();
    ScriptRegistry::Clear();

    SDL_UnloadObject(g_handle);
    g_handle = nullptr;

    ENGINE_LOG_INFO(Channels::kScene, "unloaded the scripts from '{}'", g_loadedPath);
    g_loadedPath.clear();
}

bool ScriptLibrary::IsLoaded() {
    return g_handle != nullptr;
}

const std::string& ScriptLibrary::LoadedPath() {
    return g_loadedPath;
}

std::size_t ScriptLibrary::ScriptCount() {
    return ScriptRegistry::Count();
}

} // namespace eng