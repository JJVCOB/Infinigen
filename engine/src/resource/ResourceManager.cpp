#include <engine/core/Log.h>
#include <engine/fs/FileSystem.h>
#include <engine/platform/SdlHandles.h>
#include <engine/render/Renderer.h>
#include <engine/resource/ResourceManager.h>
#include <SDL3/SDL.h>
#include <unordered_map>
#include <vector>

#define STBI_NO_STDIO
#include <stb_image.h>

namespace eng {

namespace {

std::unordered_map<std::string, std::weak_ptr<Texture>> g_cache;

TextureRef g_missing;

bool g_initialised = false;
bool g_loadingMissing = false;

TextureRef CreateTextureFromFile(std::string_view virtualPath, std::string& outError) {
    std::vector<unsigned char> bytes;
    if (!FileSystem::ReadFile(virtualPath, bytes, outError)) {
        return nullptr;
    }

    int width = 0;
    int height = 0;
    int channelsInFile = 0;
    unsigned char* pixels = stbi_load_from_memory(bytes.data(), static_cast<int>(bytes.size()),
                                                  &width, &height, &channelsInFile, 4);
    if (pixels == nullptr) {
        outError = stbi_failure_reason() != nullptr ? stbi_failure_reason()
                                                    : "the image could not be decoded";
        return nullptr;
    }

    auto* renderer = static_cast<SDL_Renderer*>(Renderer::NativeRendererHandle());
    if (renderer == nullptr) {
        stbi_image_free(pixels);
        outError = "there is no renderer yet, so textures cannot be created";
        return nullptr;
    }

    SDL_Texture* native = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);

    if (native == nullptr) {
        stbi_image_free(pixels);
        outError = SDL_GetError();
        return nullptr;
    }

    // The pitch is how many bytes one row of the image takes: four per pixel.
    const bool uploaded = SDL_UpdateTexture(native, nullptr, pixels, width * 4);

    stbi_image_free(pixels);

    if (!uploaded) {
        SDL_DestroyTexture(native);
        outError = SDL_GetError();
        return nullptr;
    }

    TextureRef texture = std::make_shared<Texture>();
    texture->path = std::string(virtualPath);
    texture->width = width;
    texture->height = height;
    texture->native = native;
    return texture;
}

} // namespace

Texture::~Texture() {
    if (native != nullptr) {
        SDL_DestroyTexture(static_cast<SDL_Texture*>(native));
        native = nullptr;
    }
}

bool ResourceManager::Init(const BootConfig&) {
    g_initialised = true;
    ENGINE_LOG_INFO(Channels::kResource, "resource manager ready");
    return true;
}

void ResourceManager::Shutdown() {
    g_missing.reset();
    PruneCache();

    for (const auto& [path, weak] : g_cache) {
        if (const TextureRef alive = weak.lock()) {
            ENGINE_LOG_WARN(Channels::kResource, "'{}' is still in use by {} thing(s) at shutdown",
                            path, alive.use_count() - 1);
        }
    }

    g_cache.clear();
    g_initialised = false;
    ENGINE_LOG_INFO(Channels::kResource, "resource manager shut down");
}

TextureRef ResourceManager::LoadTexture(std::string_view virtualPath) {
    const std::string key(virtualPath);

    if (g_cache.contains(key)) {
        if (TextureRef existing = g_cache.at(key).lock()) {
            return existing;
        }
        g_cache.erase(key);
    }

    std::string error;
    TextureRef texture = CreateTextureFromFile(virtualPath, error);
    if (!texture) {
        ENGINE_LOG_ERROR(Channels::kResource, "could not load '{}': {}", virtualPath, error);
        return MissingTexture();
    }

    ENGINE_LOG_INFO(Channels::kResource, "loaded '{}' ({}x{})", virtualPath, texture->width,
                    texture->height);

    g_cache[key] = texture;
    return texture;
}

TextureRef ResourceManager::MissingTexture() {
    if (g_missing || g_loadingMissing) {
        return g_missing;
    }

    g_loadingMissing = true;
    std::string error;
    g_missing = CreateTextureFromFile("textures/missing.bmp", error);
    if (g_missing) {
        g_missing->isPlaceholder = true;
    } else {
        ENGINE_LOG_WARN(Channels::kResource,
                        "the 'missing texture' placeholder could not be loaded itself "
                        "({}); failed sprites will draw as nothing at all",
                        error);
    }

    g_loadingMissing = false;
    return g_missing;
}

std::size_t ResourceManager::LoadedCount() {
    std::size_t count = 0;
    for (const auto& [path, weak] : g_cache) {
        if (!weak.expired()) {
            ++count;
        }
    }
    return count;
}

void ResourceManager::PruneCache() {
    std::erase_if(g_cache, [](const auto& entry) { return entry.second.expired(); });
}

} // namespace eng
