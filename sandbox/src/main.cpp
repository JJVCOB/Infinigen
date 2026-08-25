// =============================================================================
//  WEEK 1 TEMPLATE - you implement this.
//
//  The naive 'while' loop. Poll, clear, present, repeat. It is naive in a 
//  specific, nameable way: it runs the simulation as fast as the machine 
//  allows, so the game runs at a different speed on different hardware. We 
//  will implement a fixed timestep and an accumulator. Until then, naive is 
//  correct; do not try to fix it early.
// =============================================================================

#include <engine/platform/Window.h>
#include <SDL3/SDL.h>

//define _CRTDBG_MAP_ALLOC
#include <print>

//int x[100];
//
//int main()
//{
//#ifdef __SANITIZE_ADDRESS__
//    std::print("MSVC AddressSanitizer enabled");
//#else
//    std::print("MSVC AddressSanitizer not enabled");
//#endif
//
//
//    std::print("Hello!\n");
//    x[100] = 5; // Boom!
//    return 0;
//}

int main(int argc, char** argv) {
    (void)argc; (void)argv;   // Week 1 stretch goal 3 gives these a purpose.

    // TODO(week1): Initialize a SDL3 Window. Check if it IsValid(). Bail with a message
    // and a non-zero exit code if it failed.

    std::string wTitle = "My Epic Game";

    eng::Window window(wTitle.c_str(), 1280, 720);

    if (!window.IsValid())
    {
        std::print(stderr, "Could not make window\n");
        return 1;
    }

    bool running = true;
    int color = 0;
    int maxfps = 60;
    bool capfps = true;
    int fps = 0;
    int lastTime = 0;

    while (running) {
        int currentTick = SDL_GetTicks();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_EVENT_QUIT:
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    running = false;
                    break;
                default:
                    // Do nothing for other events
                    break;
            }
        }

        // Blue
        //window.Clear(62, 54, 209);
        // Random Color
        //window.Clear(rand() % 255, rand() % 255, rand() % 255);
        // Smooth Changing Color
        color += 1;
        window.Clear(color % 255, color/2 % 255, color/3 % 255);
        window.Present();

        if (capfps) // Cap FPS if cap is turned on
        {
            SDL_Delay(1000 / maxfps);
        }

        fps++; // Add to FPS
        //int deltaTime = SDL_GetTicks() - currentTick; // Get Delta Time

        if (currentTick > lastTime + 1000) // Check, reset & display FPS
        {
            lastTime = currentTick;
            std::string fpsStr = wTitle + " | FPS: " + std::format("{}", fps);
            window.SetTitle(fpsStr.c_str());
            fps = 0;
        }
    }

    std::printf("Clean exit.\n");
    return 0;
}
