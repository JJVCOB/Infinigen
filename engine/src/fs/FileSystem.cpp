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
            ENGINE_LOG_INFO(Channels::kFileSys, "Asset folder taken from ENGINE_ASSET_ROOT: '{}'", g_root);
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
        ENGINE_LOG_WARN(Channels::kFileSys, "Could not find the program's own folder; falling back to the current directory, which is not reliable");
    }

    std::error_code ec;
    fs::path current = fs::absolute(start, ec);
    for (int depth = 0; depth < 12; ++depth) {
        if (LooksLikeRoot(current)) {
            g_root = current.string();
            ENGINE_LOG_INFO(Channels::kFileSys, "Assets found in {}", g_root);
            return true;
        }
        if (!current.has_parent_path() || current.parent_path() == current) {
            break;
        }
        current = current.parent_path();
    }

    ENGINE_LOG_ERROR(Channels::kFileSys, "Could not find an 'assets' folder above '{}'. Can not load. ", start.string());
    g_root = start.string();
    return false;
}

void FileSystem::Shutdown() {
    g_root.clear();
    ENGINE_LOG_INFO(Channels::kFileSys, "File system has shut down.");
}

const std::string& FileSystem::AssetRoot() {return g_root;}

std::string FileSystem::Resolve(std::string_view virtualPath) {
    fs::path path = fs::path(g_root) / "assets" / fs::path(std::string(virtualPath));
    if (virtualPath.starts_with("config/") || virtualPath.starts_with("logs/") || virtualPath.starts_with(".build/") || virtualPath == ".build") {
        path = fs::path(g_root) / fs::path(std::string(virtualPath));
    }
    return path.lexically_normal().string();
}

bool FileSystem::Exists(std::string_view virtualPath) {
    std::error_code ec;
    return fs::exists(Resolve(virtualPath), ec);
}

bool FileSystem::ListFiles(std::string_view virtualDirectory, std::string_view extension, std::vector<std::string>& out) {
    out.clear();

    const std::string real = Resolve(virtualDirectory);
    std::error_code ec;

    if (!fs::is_directory(real, ec)) {
        ENGINE_LOG_WARN(Channels::kFileSys, "'{}' is not a folder (looked in '{}')", virtualDirectory, real);
        return false;
    }

    std::string prefix(virtualDirectory);
    if (!prefix.empty() && prefix.back() != '/') {
        prefix.push_back('/');
    }

    for (const fs::directory_entry& entry : fs::directory_iterator(real, ec)) {
        if (!entry.is_regular_file(ec)) {
            continue;
        }
        const std::string name = entry.path().filename().string();
        if (!extension.empty() && !name.ends_with(extension)) {
            continue;
        }
        out.push_back(prefix + name);
    }

    std::sort(out.begin(), out.end());
    return true;
}

bool FileSystem::ListDirectory(std::string_view virtualDirectory, std::vector<DirEntry>& out) {
    out.clear();
    const std::string real = Resolve(virtualDirectory);
    std::error_code ec;

    if (!fs::is_directory(real, ec)) {
        return false;
    }

    std::string prefix(virtualDirectory);
    if (!prefix.empty() && prefix.back() != '/') {
        prefix.push_back('/');
    }

    for (const auto& entry : fs::directory_iterator(real, ec)) {
        DirEntry item;
        item.name = entry.path().filename().string();

        if (item.name.empty() || item.name.front() == '.') {
            continue;
        }

        item.isDirectory = entry.is_directory(ec);
        if (!item.isDirectory && !entry.is_regular_file(ec)) {
            continue;
        }

        item.virtualPath = prefix + item.name;
        if (!item.isDirectory) {
            item.byteSize = static_cast<unsigned long long>(entry.file_size(ec));
            if (ec) {
                item.byteSize = 0;
                ec.clear();
            }
        }

        out.push_back(std::move(item));
    }

    std::sort(out.begin(), out.end(), [](const DirEntry& a, const DirEntry& b) {
        if (a.isDirectory != b.isDirectory) {
            return a.isDirectory;
        }
        return a.name < b.name;
    });
    
    return true;
}

bool FileSystem::CreateDirectory(std::string_view virtualDirectory, std::string& outError) {
    const std::string real = Resolve(virtualDirectory);
    std::error_code ec;
    fs::create_directories(real, ec);

    if (ec) {
        outError = "Could not create '" + std::string(virtualDirectory) + "': " + ec.message();
        return false;
    }
    outError.clear();

    return true;
}

bool FileSystem::ReadTextFile(std::string_view virtualPath, std::string& outText, std::string& outError) {
    const std::string real = Resolve(virtualPath);
    std::ifstream file(real);
    if (!file) {
        outError = "Could not open '" + std::string(virtualPath) + "' (searched in '" + real + "')";
        return false;
    }

    std::ostringstream contents;
    contents << file.rdbuf();
    outText = contents.str();

    outError.clear();
    return true;
}

bool FileSystem::ReadFile(std::string_view virtualPath, std::vector<unsigned char>& outBytes, std::string& outError) {
    const std::string real = Resolve(virtualPath);
    std::ifstream file(real, std::ios::binary | std::ios::ate);

    if (!file) {
        outError = "Could not open '" + std::string(virtualPath) + "' (searched in '" + real + "')";
        return false;
    }

    const std::streamsize size = file.tellg();

    if (size < 0) {
        outError = "Could not measure file size of: '" + real + "'";
        return false;
    }

    file.seekg(0, std::ios::beg);
    outBytes.resize(static_cast<std::size_t>(size));
    
    if (size > 0 && !file.read(reinterpret_cast<char*>(outBytes.data()), size)) {
        outError = "Read failed in: '" + real + "'";
        outBytes.clear();
        return false;
    }
    
    outError.clear();
    return true;
}

bool FileSystem::WriteTextFile(std::string_view virtualPath, std::string_view text, std::string& outError) {
    const std::string real = Resolve(virtualPath);
    std::error_code ec;
    fs::create_directories(fs::path(real).parent_path(), ec);
    std::ofstream file(real, std::ios::trunc);

    if (!file) {
        outError = "Could not open '" + real + "' for writing";
        return false;
    }

    file.write(text.data(), static_cast<std::streamsize>(text.size()));

    if (!file) {
        outError = "Writing to '" + real + "' failed";
        return false;
    }

    outError.clear();
    return true;
}

} // namespace eng
