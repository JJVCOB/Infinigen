#include <engine/core/Log.h>
#include <engine/platform/Window.h>
#include <engine/render/Renderer.h>
#include <SDL3/SDL.h>
#include <algorithm>

namespace eng {
namespace {

SDL_Renderer* g_renderer = nullptr;
Window* g_window = nullptr;
RenderTarget* g_target = nullptr;
float g_textScale = 2.0f;

void ApplyColor(Color c) {
    SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(g_renderer, c.r, c.g, c.b, c.a);
}

} // namespace

bool Renderer::Init(Window& window) {
    if (!window.IsValid()) {
        ENGINE_LOG_ERROR(Channels::kRender, "The renderer was not given a valid window");
        return false;
    }
    g_window = &window;
    g_renderer = static_cast<SDL_Renderer*>(window.NativeRendererHandle());
    ENGINE_LOG_INFO(Channels::kRender, "Renderer is ready: {}", SDL_GetRendererName(g_renderer));
    return IsValid();
}

void Renderer::Shutdown() {
    g_renderer = nullptr;
    g_window = nullptr;
    ENGINE_LOG_INFO(Channels::kRender, "Renderer has been shut down");
}

bool Renderer::IsValid() { return g_renderer != nullptr; }
void* Renderer::NativeRendererHandle() { return g_renderer; }

Vec2 Renderer::OutputSize() {
    if (g_renderer == nullptr) { return Vec2{0.0f, 0.0f}; }
    if (g_target != nullptr && g_target->IsValid()) { return Vec2{static_cast<float>(g_target->Width()), static_cast<float>(g_target->Height())}; }

    int w, h = 0;
    SDL_GetCurrentRenderOutputSize(g_renderer, &w, &h);
    return Vec2{static_cast<float>(w), static_cast<float>(h)};
}

RenderTarget::~RenderTarget() = default;
void* RenderTarget::NativeTexture() const { return m_texture.get(); }

bool RenderTarget::Resize(int width, int height) {
    width = std::max(width, 1);
    height = std::max(height, 1);

    if (m_texture != nullptr && width == m_width && height == m_height) { return true; } // already the right size
    if (g_renderer == nullptr) { return false; } 

    m_texture.reset(SDL_CreateTexture(g_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height));

    if (m_texture == nullptr) {
        ENGINE_LOG_ERROR(Channels::kRender, "Could not create a {}x{} view: {}", width, height, SDL_GetError());
        m_width, m_height = 0;
        return false;
    }

    SDL_SetTextureScaleMode(m_texture.get(), SDL_SCALEMODE_PIXELART);
    m_width = width;
    m_height = height;
    return true;
}

void Renderer::SetRenderTarget(RenderTarget* target) {
    if (g_renderer == nullptr) { return; }

    SDL_Texture* texture = (target != nullptr && target->IsValid()) ? static_cast<SDL_Texture*>(target->NativeTexture()) : nullptr;

    if (!SDL_SetRenderTarget(g_renderer, texture)) {
        ENGINE_LOG_ERROR(Channels::kRender, "Could not change drawing target: {}", SDL_GetError());
        return;
    }
    g_target = (texture != nullptr) ? target : nullptr;
}

RenderTarget* Renderer::CurrentRenderTarget() { return g_target; }

void Renderer::Clear(Color color) {
    if (g_renderer == nullptr) { return; }
    ApplyColor(color);
    SDL_RenderClear(g_renderer);
}

void Renderer::Present() {
    if (g_window != nullptr) { g_window->Present(); }
}

void Renderer::DrawLine(Vec2 a, Vec2 b, Color color) {
    if (g_renderer == nullptr) { return; }
    ApplyColor(color);
    SDL_RenderLine(g_renderer, a.x, a.y, b.x, b.y);
}

void Renderer::DrawRect(Vec2 min, Vec2 max, Color color) {
    if (g_renderer == nullptr) { return; }
    ApplyColor(color);
    SDL_FRect rect{min.x, min.y, max.x - min.x, max.y - min.y};
    SDL_RenderRect(g_renderer, &rect);
}

void Renderer::DrawFilledRect(Vec2 min, Vec2 max, Color color) {
    if (g_renderer == nullptr) { return; }
    ApplyColor(color);
    SDL_FRect rect{min.x, min.y, max.x - min.x, max.y - min.y};
    SDL_RenderFillRect(g_renderer, &rect);
}

void Renderer::DrawPoint(Vec2 p, Color color) {
    if (g_renderer == nullptr) { return; }
    ApplyColor(color);
    SDL_RenderPoint(g_renderer, p.x, p.y);
}

void Renderer::SetTextScale(float scale) { g_textScale = (scale > 0.0f) ? scale : 1.0f; }
float Renderer::TextScale() { return g_textScale; }
float Renderer::TextLineHeight() { return static_cast<float>(SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * g_textScale + 2.0f; }
float Renderer::TextCharWidth() { return static_cast<float>(SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) * g_textScale; }

void Renderer::DrawText(Vec2 topLeft, const char* text, Color color) {
}

void Renderer::DrawSprite(const TextureRef& texture, Vec2 centre, Vec2 size, float rotationDegrees, Color tint) {
}

} // namespace eng
