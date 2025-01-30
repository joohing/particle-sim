#include </opt/homebrew/Cellar/sdl2/2.30.10/include/SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "state.h"

// Controls the rate of change of the position of particles.
float DX = 0.01;
float DY = 0.01;

// Seed before calling with srand(time(NULL))
int rng(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

//  0 if equal
//  1 if i > j
// -1 if i < j
int sign(int i, int j)
{
    if (i == j)
    {
        return 0;
    }
    else if (i > j)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

void print_points(SDL_Point points[])
{
    for (int i = 0; i < POINT_MAX; i++)
    {
        printf("p%d: (%d, %d)\n", i, points[i].x, points[i].y);
    }
}

// The idx is the index of a point. This point is mutated to be equal to itself
// summed with its distances to the other points in the points array.
void iter_point(int idx, SDL_Point *points[])
{
    int x, xi, y, yi, i;

    for (i = 0; i < POINT_MAX; i++)
    {
        if (i == idx) continue;

        x = points[idx]->x;
        xi = points[i]->x;
        y = points[idx]->y;
        yi = points[i]->y;

        points[idx]->x = (x - xi) * sign(x, xi);
        points[idx]->y = (y - yi) * sign(y, yi);
    }
}

// Mutate the state to simulate one step in the particle simulation.
void iter_state(State *state)
{
    for (int i = 0; i < POINT_MAX; i++)
    {
        iter_point(i, state->particles);
    }
}

State* get_sample_state()
{
    srand(time(NULL));

    SDL_Point p0, p1, p2, p3, p4, p5, p6, p7;

    p0.x = rng(0, 500);
    p1.x = rng(0, 500);
    p2.x = rng(0, 500);
    p3.x = rng(0, 500);
    p4.x = rng(0, 500);
    p5.x = rng(0, 500);
    p6.x = rng(0, 500);
    p7.x = rng(0, 500);

    p0.y = rng(0, 500);
    p1.y = rng(0, 500);
    p2.y = rng(0, 500);
    p3.y = rng(0, 500);
    p4.y = rng(0, 500);
    p5.y = rng(0, 500);
    p6.y = rng(0, 500);
    p7.y = rng(0, 500);

    SDL_Point prts[POINT_MAX];
    prts[0] = p0;
    prts[1] = p1;
    prts[2] = p2;
    prts[3] = p3;
    prts[4] = p4;
    prts[5] = p5;
    prts[6] = p6;
    prts[7] = p7;

    State* state_mem = calloc(1, sizeof(State));
    memcpy(state_mem->particles, prts, sizeof(SDL_Point) * POINT_MAX);

    return state_mem;
}
