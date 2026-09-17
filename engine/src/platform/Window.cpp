#include <engine/platform/Window.h>
#include <engine/core/Config.h>
#include <engine/core/Log.h>
#include <SDL3/SDL.h>

namespace eng {

bool Window::Init(const BootConfig& config) {
    m_title = config.windowTitle.empty() ? "Infinigen" : config.windowTitle;
    const int width = config.windowWidth;
    const int height = config.windowHeight;
    
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        ENGINE_LOG_ERROR(Channels::kPlatform, "Could not start SDL video: {}", SDL_GetError());
        return false;
    }

    m_videoInitialised = true;
    SDL_Window* rawWindow = nullptr;
    SDL_Renderer* rawRenderer = nullptr;

    if (!SDL_CreateWindowAndRenderer(m_title.c_str(), width, height, SDL_WINDOW_RESIZABLE, &rawWindow, &rawRenderer)) {
        ENGINE_LOG_ERROR(Channels::kPlatform, "Could not create the SDL window: {}", SDL_GetError());
        m_window.reset(rawWindow);
        m_renderer.reset(rawRenderer);
        return false;
    }

    m_window.reset(rawWindow);
    m_renderer.reset(rawRenderer);

    if (!SDL_SetRenderVSync(m_renderer.get(), 1)) {
        ENGINE_LOG_WARN(Channels::kPlatform, "VSync is not available: {}", SDL_GetError());
    }

    ENGINE_LOG_INFO(Channels::kPlatform, "Window created: {}x{} \"{}\" (drawing with {})", width, height, m_title, SDL_GetRendererName(m_renderer.get()));
    return true;
}

void Window::Shutdown() {
    if (m_window == nullptr && m_renderer == nullptr && !m_videoInitialised) {
        return;
    }

    m_renderer.reset();
    m_window.reset();

    if (m_videoInitialised) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        m_videoInitialised = false;
    }

    ENGINE_LOG_INFO(Channels::kPlatform, "Window closed");
}

Window::~Window() {
    Shutdown();
}

bool Window::IsValid() const {
    return m_window != nullptr && m_renderer != nullptr;
}

int Window::Width() const {
    int w, h = 0;
    if (m_window != nullptr) {SDL_GetWindowSize(m_window.get(), &w, &h);}
    return w;
}

int Window::Height() const {
    int w, h = 0;
    if (m_window != nullptr) {SDL_GetWindowSize(m_window.get(), &w, &h);}
    return h;
}

void Window::SetTitle(const char* title) {
    if (m_window == nullptr || title == nullptr) {return;}
    m_title = title;
    SDL_SetWindowTitle(m_window.get(), m_title.c_str());
}

void Window::Clear(unsigned char r, unsigned char g, unsigned char b) {
    if (m_renderer == nullptr) {return;}
    SDL_SetRenderDrawColor(m_renderer.get(), r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(m_renderer.get());
}

void Window::Present() {
    if (m_renderer == nullptr) {return;}
    SDL_RenderPresent(m_renderer.get());
}

void* Window::NativeWindowHandle() const {return m_window.get();}
void* Window::NativeRendererHandle() const {return m_renderer.get();}

} // namespace eng
