#include <engine/resource/ResourceManager.h>

namespace eng {

Texture::~Texture() {
}

bool ResourceManager::Init(const BootConfig& config) {
    return false;
}

void ResourceManager::Shutdown() {
}

TextureRef ResourceManager::LoadTexture(std::string_view virtualPath) {
    return nullptr;
}

TextureRef ResourceManager::MissingTexture() {
    return nullptr;
}

std::size_t ResourceManager::LoadedCount() {
    return 0;
}

void ResourceManager::PruneCache() {
}

} // namespace eng
