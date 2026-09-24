#include <engine/scene/ScriptLibrary.h>

namespace eng {

bool ScriptLibrary::Init(const BootConfig& config) {
    return false;
}

void ScriptLibrary::Shutdown() {}

std::string ScriptLibrary::DefaultVirtualPath() {
#if defined(_WIN32)
    return ".build/userContent.dll";
#else
    return ".build/userContent.so";
#endif
}

bool ScriptLibrary::Load(std::string_view virtualPath, std::string& outError) {
    return false;
}

void ScriptLibrary::Unload() {
}

bool ScriptLibrary::IsLoaded() {
    return false;
}

const std::string& ScriptLibrary::LoadedPath() {
    static const std::string none;
    return none;
}

std::size_t ScriptLibrary::ScriptCount() {
    return 0;
}

} // namespace eng
