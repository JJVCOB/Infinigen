#include <engine/core/Log.h>
#include <engine/fs/FileSystem.h>
#include <SDL3/sdl.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace eng {
namespace {

namespace fs = std::filesystem;
std::string g_root;

bool LooksLikeRoot(const fs::path& directory) {
    std::error_code ec;
    return fs::is_directory(directory / "assets", ec);
}

} // namespace

bool FileSystem::Init(const BootConfig& config) {
    if (const char* overridePath = std::getenv("ENGINE_ASSET_ROOT"); overridePath != nullptr && overridePath[0] != '\0') {
        std::error_code ec;
        const fs::path candidate = fs::absolute(fs::path(overridePath), ec);
        if (LooksLikeRoot(candidate)) {
            g_root = candidate.string();
            ENGINE_LOG_INFO(Channels::kFileSys, "asset folder taken from ENGINE_ASSET_ROOT: '{}'", g_root);
            return true;
        }
        ENGINE_LOG_WARN(Channels::kFileSys, "ENGINE_ASSET_ROOT is set to '{}' but there is no 'assets' folder there; searching instead", overridePath);
    }

    fs::path start;
    if (const char* base = SDL_GetBasePath(); base != nullptr && base[0] != '\0') {
        start = fs::path(base);
    } else {
        std::error_code ec;
        start = fs::current_path(ec);
        ENGINE_LOG_WARN(Channels::kFileSys, "could not find the program's own folder; falling back to the current directory, which is not reliable");
    }

    std::error_code ec;
    fs::path current = fs::absolute(start, ec);
    for (int depth = 0; depth < 12; ++depth) {
        if (LooksLikeRoot(current)) {

        }
    }
    
    return false;
}

void FileSystem::Shutdown() {
}

const std::string& FileSystem::AssetRoot() {
    static const std::string root;
    return root;
}

std::string FileSystem::Resolve(std::string_view virtualPath) {
    return {};
}

bool FileSystem::Exists(std::string_view virtualPath) {
    return false;
}

bool FileSystem::ListFiles(std::string_view virtualDirectory, std::string_view extension, std::vector<std::string>& out) {
    return false;
}

bool FileSystem::ListDirectory(std::string_view virtualDirectory, std::vector<DirEntry>& out) {
    return false;
}

bool FileSystem::CreateDirectory(std::string_view virtualDirectory, std::string& outError) {
    return false;
}

bool FileSystem::ReadTextFile(std::string_view virtualPath, std::string& outText, std::string& outError) {
    return false;
}

bool FileSystem::ReadFile(std::string_view virtualPath, std::vector<unsigned char>& outBytes, std::string& outError) {
    return false;
}

bool FileSystem::WriteTextFile(std::string_view virtualPath, std::string_view text, std::string& outError) {
    return false;
}

} // namespace eng
