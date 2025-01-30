#include <stdio.h>
#include <stdlib.h>
#include </opt/homebrew/Cellar/sdl2/2.30.10/include/SDL2/SDL.h>
#include "state.h"
#include "draw.h"

int main(int argc, char *argv[])
{
    // Init SDL
    int init_code = SDL_Init(SDL_INIT_VIDEO);
    if (init_code < 0)
    {
        printf("Couldn't initialize SDL2 Video: %s\n", SDL_GetError());
        return init_code;
    }

    // Create window
    SDL_Window *window = SDL_CreateWindow("Hello everybody I am under the water",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        512, 512,
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
    SDL_Event e;

    // Sample state, remember free(state.particles)
    State* state = get_sample_state();
    print_points((SDL_Point *) state->particles);

    while (keep_win)
    {
        render_state(renderer, state);
        draw_current_render(renderer, window);

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
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    printf("Mouse at (%d, %d)\n", x, y);
                    break;
                }
            }
        }
    }

    return 0;
}
