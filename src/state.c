#include </opt/homebrew/Cellar/sdl2/2.30.10/include/SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "state.h"
#include <math.h>
#include "jonamath.h"

// Controls the rate of change of the position of particles.
float DX = 0.01;
float DY = 0.01;

// Seed before calling with srand(time(NULL))
int rng(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

// Absolute distance
int dist(float x1, float y1, float x2, float y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

void print_particles(Particle prts[])
{
    for (int i = 0; i < POINT_MAX; i++)
    {
        printf("particle %d: pos (%f, %f), vel (%f, %f), mass %f\n",
                i,
                prts[i].x,
                prts[i].y,
                prts[i].vx,
                prts[i].vy,
                prts[i].m);
    }
}

// Mutate particle position and velocity when it bounces off the wall.
void bounce_back(Particle* prt)
{
    int x = prt->x;
    int y = prt->y;
    int vx = prt->vx;
    int vy = prt->vy;
    int m = prt->m;

    // If outside of bounds and NOT going in the right direction,
    // reverse direction (with some inhibiting factor)
    int within_width = x + m > 0 && x + m < WIN_WIDTH;
    int within_height = y + m > 0 && y + m < WIN_HEIGHT;
    if (!within_width)
    {
        if (x < 0)
        {
            prt->x = -x;
        }
        else
        {
            prt->x = WIN_WIDTH - m;
        }
        prt->vx *= -RESISTANCE;
    }
    if (!within_height)
    {
        if (y < 0)
        {
            prt->y = -y;
        }
        else
        {
            prt->y = WIN_HEIGHT - m;
        }
        prt->vy *= -RESISTANCE;
    }
}

// The idx is the index of a point. This point is mutated to be equal to itself
// summed with its distances to the other points in the points array.
void iter_point(int idx, Particle* prts, int mouse_gravity)
{
    int sign_x, sign_y, within_width, within_height;
    float x, xi, y, yi, distance, dvx, dvy, fx, fy, upd_x, upd_y;

    for (int i = 0; i < POINT_MAX + mouse_gravity; i++)
    {
        x = prts[idx].x;
        y = prts[idx].y;

        prts[idx].x = x + prts[idx].vx;
        prts[idx].y = y + prts[idx].vy;

        fprintf(stderr, "pos: (%f, %f), vel: (%f, %f)\n",
                        x,
                        y,
                        prts[idx].vx,
                        prts[idx].vy);

        bounce_back(&prts[idx]);
        if (i == idx) continue;

        xi = prts[i].x;
        yi = prts[i].y;
        sign_x = sign(xi - x) == 0 ? 1 : sign(xi - x);
        sign_y = sign(yi - y) == 0 ? 1 : sign(yi - y);
        distance = dist(x, y, xi, yi);

        fx = G * prts[i].m * prts[idx].m / (distance * distance);
        fy = G * prts[i].m * prts[idx].m / (distance * distance);
        dvx = ITER_SCALE * sign_x * fx;
        dvy = ITER_SCALE * sign_y * fy;

        prts[idx].vx = prts[idx].vx + dvx;
        prts[idx].vy = prts[idx].vy + dvy;
    }
}

// Mutate the state to simulate one step in the particle simulation.
// mouse_gravity is whether or not the mouse should have gravity right now.
void iter_state(State *state, int mouse_gravity)
{
    for (int i = 0; i < POINT_MAX; i++)
    {
        iter_point(i, (Particle *) state->prts, mouse_gravity);
    }
}

State* get_sample_state()
{
    srand(time(NULL));

    // One extra particle is allocated to simulate the mouse having mass
    Particle* prts = calloc(POINT_MAX + 1, sizeof(Particle));
    prts[POINT_MAX].m = MOUSE_MASS;

    for (int i = 0; i < POINT_MAX; i++) {
        prts[i].vx = rng(VX_MIN, VX_MAX);
        prts[i].vy = rng(VY_MIN, VY_MAX);
        prts[i].m = rng(MIN_MASS, MAX_MASS);
        prts[i].x = rng(WIN_WIDTH / 4, WIN_WIDTH * 3 / 4);
        prts[i].y = rng(WIN_HEIGHT / 4, WIN_HEIGHT * 3 / 4);
    }

    State* state_mem = calloc(1, sizeof(State));
    memcpy(state_mem->prts, prts, sizeof(Particle) * (POINT_MAX + 1));

    return state_mem;
}
