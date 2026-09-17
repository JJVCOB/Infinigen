#include <engine/platform/SdlHandles.h>

namespace eng {

void SdlWindowDeleter::operator()(SDL_Window* window) const noexcept {
}

void SdlRendererDeleter::operator()(SDL_Renderer* renderer) const noexcept {
}

void SdlSurfaceDeleter::operator()(SDL_Surface* surface) const noexcept {
}

void SdlTextureDeleter::operator()(SDL_Texture* texture) const noexcept {
}

} // namespace eng
