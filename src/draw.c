#include <stdlib.h>
#include <stdio.h>
#include </opt/homebrew/Cellar/sdl2/2.30.10/include/SDL2/SDL.h>
#include </opt/homebrew/Cellar/sdl2/2.30.10/include/SDL2/SDL_opengl.h>
#include "state.h"

int abs(int x)
{
    uint32_t temp = x >> 31;
    x ^= temp;
    x += temp & 1;
    return x;
}

void render_circles(SDL_Renderer *renderer, SDL_Point *particles)
{
    // The circles are drawn as a series of POINT_RADIUS line segments
    // on each side of the point + 1 in the middle. As each line seg
    // needs 2 points, we'll need twice that.
    int n_lines = (POINT_RADIUS * 2 + 1) * 2;
    int x1, x2, y1, y2;
    int flip = 1;
    SDL_Point p1, p2;
    SDL_Point lines[n_lines];

    /* fprintf(stderr, "PRINTING LINE SEGMENTS....\n"); */

    for (int i = 0; i < POINT_MAX; i++)
    {
        for (int j = 0; j < n_lines; j = j + 2)
        {
            /* fprintf(stderr, "# j = %d\n", j); */
            x1 = abs(j - POINT_RADIUS);
            x2 = abs(j - POINT_RADIUS + 1);
            y1 = POINT_RADIUS - x1;
            y2 = POINT_RADIUS - x2;
            p1.x = x1 + particles[i].x;
            p2.x = x2 + particles[i].x;
            p1.y = flip * y1 + particles[i].y;
            p2.y = flip * y2 + particles[i].y;

            /* fprintf(stderr, "x1: %d\n", x1); */
            /* fprintf(stderr, "x2: %d\n", x2); */
            /* fprintf(stderr, "y1: %d\n", y1); */
            /* fprintf(stderr, "y2: %d\n", y2); */
            lines[j] = p1;
            lines[j + 1] = p2;
            flip = -flip;
        }

        SDL_RenderDrawLines(renderer, lines, n_lines);
    }
}

// Render all the particles in the state to the backbuffer.
void render_state(SDL_Renderer *renderer, State *state)
{
    if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) != 0)
    {
        printf("Couldn't set render draw color. SDL_Error: %s\n", SDL_GetError());
        return;
    }

    int result = SDL_RenderDrawPoints(renderer, (SDL_Point *) state->particles, POINT_MAX);
    if (result != 0)
    {
        printf("Failed attempt at rendering, code %d\n", result);
    }

    render_circles(renderer, (SDL_Point*) state->particles);
}

// RenderPresent the backbuffer in the renderer to the window.
void draw_current_render(SDL_Renderer *renderer, SDL_Window *win)
{
    SDL_RenderPresent(renderer);

    // Clear backbuffer
    if (SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255) != 0)
    {
        printf("Couldn't set render draw color. SDL_Error: %s\n", SDL_GetError());
        return;
    }
    SDL_RenderClear(renderer);
}
