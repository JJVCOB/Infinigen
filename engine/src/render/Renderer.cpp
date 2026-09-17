#include <engine/render/Renderer.h>

namespace eng {

bool Renderer::Init(Window& window) {
    return false;
}

void Renderer::Shutdown() {
}

bool Renderer::IsValid() {
    return false;
}

void* Renderer::NativeRendererHandle() {
    return nullptr;
}

Vec2 Renderer::OutputSize() {
    return Vec2{};
}

RenderTarget::~RenderTarget() {
}

void* RenderTarget::NativeTexture() const {
    return nullptr;
}

bool RenderTarget::Resize(int width, int height) {
    return false;
}

void Renderer::SetRenderTarget(RenderTarget* target) {
}

RenderTarget* Renderer::CurrentRenderTarget() {
    return nullptr;
}

void Renderer::Clear(Color color) {
}

void Renderer::Present() {
}

void Renderer::DrawLine(Vec2 a, Vec2 b, Color color) {
}

void Renderer::DrawRect(Vec2 min, Vec2 max, Color color) {
}

void Renderer::DrawFilledRect(Vec2 min, Vec2 max, Color color) {
}

void Renderer::DrawPoint(Vec2 p, Color color) {
}

void Renderer::SetTextScale(float scale) {
}

float Renderer::TextScale() {
    return 1.0f;
}

float Renderer::TextLineHeight() {
    return 0.0f;
}

float Renderer::TextCharWidth() {
    return 0.0f;
}

void Renderer::DrawText(Vec2 topLeft, const char* text, Color color) {
}

void Renderer::DrawSprite(const TextureRef& texture, Vec2 centre, Vec2 size, float rotationDegrees, Color tint) {
}

} // namespace eng
