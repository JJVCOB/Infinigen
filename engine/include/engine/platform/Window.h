#pragma once

#include <engine/core/Subsystem.h>
#include <engine/platform/SdlHandles.h>
#include <string>

namespace eng {

class Window : public Subsystem {
public:
    Window() = default;

    bool Init(const BootConfig& config) override;
    void Shutdown() override;
    ~Window() override;
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool IsValid() const;
    int Width() const;
    int Height() const;
    void SetTitle(const char* title);
    void Clear(unsigned char r, unsigned char g, unsigned char b);
    void Present();

    void* NativeWindowHandle() const;
    void* NativeRendererHandle() const;

private:
    WindowPtr m_window;
    RendererPtr m_renderer;
    bool m_videoinitialized = false;
    std::string m_title = "Infinigen";
};

} // namespace eng
