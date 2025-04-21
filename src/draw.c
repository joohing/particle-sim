#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "state.h"

void render_circles(SDL_Renderer *renderer, Particle *prts)
{
    int x1, x2, y1, y2, r, n_lines;
    SDL_Point p1, p2;

    for (int i = 0; i < POINT_MAX; i++)
    {
        r = prts[i].m;
        n_lines = (r * 2 + 1) * 2;
        SDL_Point lines[n_lines];
        for (int j = 0; j < n_lines; j = j + 2)
        {
            x1 = j / 2 - r;
            x2 = j / 2 - r + 1;
            y1 = sqrt(r * r - x1 * x1);
            y2 = sqrt(r * r - x2 * x2);

            p1.x = x1 + prts[i].x;
            p2.x = x2 + prts[i].x;
            p1.y = y1 + prts[i].y;
            p2.y = (-1) * y2 + prts[i].y;

            lines[j] = p1;
            lines[j + 1] = p2;
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

    SDL_Point points[POINT_MAX];
    for (int i = 0; i < POINT_MAX; i++) {
        points[i].x = state->prts[i].x;
        points[i].y = state->prts[i].y;
    }

    int result = SDL_RenderDrawPoints(renderer, points, POINT_MAX);
    if (result != 0)
    {
        printf("Failed attempt at rendering, code %d\n", result);
    }

    render_circles(renderer, state->prts);
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
