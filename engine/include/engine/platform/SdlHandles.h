#pragma once

#include <memory>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;

namespace eng {

struct SdlWindowDeleter { void operator()(SDL_Window* window) const noexcept; };
struct SdlRendererDeleter { void operator()(SDL_Renderer* renderer) const noexcept; };
struct SdlSurfaceDeleter { void operator()(SDL_Surface* surface) const noexcept; };
struct SdlTextureDeleter { void operator()(SDL_Texture* texture) const noexcept; };

using WindowPtr = std::unique_ptr<SDL_Window, SdlWindowDeleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, SdlRendererDeleter>;
using SurfacePtr = std::unique_ptr<SDL_Surface, SdlSurfaceDeleter>;
using TexturePtr = std::unique_ptr<SDL_Texture, SdlTextureDeleter>;

} // namespace eng
