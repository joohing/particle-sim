#include <stdio.h>
#include <stdlib.h>
#include </opt/homebrew/Cellar/sdl2/2.30.10/include/SDL2/SDL.h>
#include "state.h"
#include "draw.h"

int main(int argc, char *argv[])
{
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Couldn't initialize SDL2 Video: %s\n", SDL_GetError());
        return -1;
    }

    // Create window
    SDL_Window *window = SDL_CreateWindow("Hello everybody I am under the water",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIN_WIDTH, WIN_HEIGHT,
        0);
    if (!window)
    {
        printf("Failed to create a window :( SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Get surface for window
    SDL_Surface *win_sf = SDL_GetWindowSurface(window);
    if (!win_sf)
    {
        printf("Failed to get window surface :(, SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Get renderer
    SDL_Renderer *renderer = SDL_GetRenderer(window);
    if (!renderer)
    {
        printf("Got null pointer from SDL_GetRenderer. SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Get surface from bitmap image
    char* file = "./samples/sample.bmp";
    SDL_Surface *img_sf = SDL_LoadBMP(file);
    if (!img_sf)
    {
        printf("Couldn't load image %s, SDL_Error: %s", file, SDL_GetError());
        return -1;
    }
    SDL_BlitSurface(img_sf, NULL, win_sf, NULL);

    // Main event loop
    int keep_win = 1;
    int mouse_x, mouse_y;
    int mouse_gravity = 1;
    SDL_Event e;
    Uint32 before, after;
    int delay;

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    State* state = get_sample_state();

    while (keep_win)
    {
        before = SDL_GetTicks();

        render_state(renderer, state);
        draw_current_render(renderer, window);
        iter_state(state, mouse_gravity);

        while (SDL_PollEvent(&e) > 0)
        {
            switch (e.type)
            {
                case SDL_QUIT:
                {
                    SDL_Quit();
                    keep_win = 0;
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    SDL_GetMouseState(&mouse_x, &mouse_y);
                    state->prts[POINT_MAX].x = mouse_x;
                    state->prts[POINT_MAX].y = mouse_y;
                    break;
                }
                case SDL_KEYDOWN:
                {
                    if (e.key.keysym.sym == SDLK_r)
                    {
                        state = get_sample_state();
                    }
                    if (e.key.keysym.sym == SDLK_ESCAPE)
                    {
                        SDL_Quit();
                        keep_win = 0;
                        break;
                    }
                }
            }

            mouse_gravity = keystates[SDL_SCANCODE_SPACE];
        }

        after = SDL_GetTicks();
        delay = before + FRAMETIME - after;

        if (delay > 0)
        {
            SDL_Delay(delay);
        }
    }

    return 0;
}
