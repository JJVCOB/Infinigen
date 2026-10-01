#include <engine/platform/SdlHandles.h>
#include <SDL3/SDL.h>

namespace eng {

void SdlWindowDeleter::operator()(SDL_Window* window) const noexcept {
    SDL_DestroyWindow(window);
}

void SdlRendererDeleter::operator()(SDL_Renderer* renderer) const noexcept {
    SDL_DestroyRenderer(renderer);
}

void SdlSurfaceDeleter::operator()(SDL_Surface* surface) const noexcept {
    SDL_DestroySurface(surface);
}

void SdlTextureDeleter::operator()(SDL_Texture* texture) const noexcept {
    SDL_DestroyTexture(texture);
}

} // namespace eng
