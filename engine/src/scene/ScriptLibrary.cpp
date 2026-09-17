#include <engine/scene/ScriptLibrary.h>

namespace eng {

std::string ScriptLibrary::DefaultVirtualPath() {
    return {};
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

bool ScriptLibrary::Init(const BootConfig& config) {
    return false;
}

void ScriptLibrary::Shutdown() {}

} // namespace eng
