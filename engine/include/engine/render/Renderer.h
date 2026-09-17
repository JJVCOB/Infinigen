#pragma once

#include <engine/math/Vec2.h>
#include <engine/platform/SdlHandles.h>
#include <engine/render/Texture.h>

namespace eng {

class Window;

struct Color {
    unsigned char r = 255, g = 255, b = 255, a = 255;
    static constexpr Color White() { return {255, 255, 255, 255}; }
    static constexpr Color Black() { return {  0,   0,   0, 255}; }
    static constexpr Color Red() { return {235,  64,  52, 255}; }
    static constexpr Color Green() { return { 76, 205,  86, 255}; }
    static constexpr Color Blue() { return { 66, 135, 245, 255}; }
    static constexpr Color Yellow() { return {245, 205,  66, 255}; }
    static constexpr Color Cyan() { return { 66, 233, 245, 255}; }
    static constexpr Color Magenta() { return {245,  66, 233, 255}; }
    static constexpr Color Orange() { return {245, 145,  66, 255}; }
    static constexpr Color Grey() { return {128, 128, 128, 255}; }
    constexpr Color WithAlpha(unsigned char alpha) const { return Color{r, g, b, alpha}; }
    friend constexpr bool operator==(const Color&, const Color&) = default;
};

class RenderTarget {
public:
    RenderTarget() = default;
    ~RenderTarget();
    RenderTarget(const RenderTarget&) = delete;
    RenderTarget& operator = (const RenderTarget&) = delete;
    bool Resize(int width, int height);
    int Width() const { return m_width; }
    int Height() const { return m_height; }
    bool IsValid() const { return m_texture != nullptr; }
    void* NativeTexture() const;

private:
    friend class Renderer;
    TexturePtr m_texture;
    int m_width  = 0;
    int m_height = 0;
};

class Renderer {
public:
    static bool Init(Window& window);
    static void Shutdown();
    static bool IsValid();
    static Vec2 OutputSize();
    static void Clear(Color color);
    static void Present();
    static void SetRenderTarget(RenderTarget* target);
    static RenderTarget* CurrentRenderTarget();
    static void DrawLine(Vec2 a, Vec2 b, Color color);
    static void DrawRect(Vec2 min, Vec2 max, Color color); // outline only
    static void DrawFilledRect(Vec2 min, Vec2 max, Color color);
    static void DrawPoint(Vec2 p, Color color);
    static void DrawText(Vec2 topLeft, const char* text, Color color);
    static float TextLineHeight();
    static float TextCharWidth();
    static void SetTextScale(float scale);
    static float TextScale();
    static void DrawSprite(const TextureRef& texture, Vec2 centre, Vec2 size, float rotationDegrees, Color tint);
    static void* NativeRendererHandle();
};

} // namespace eng
