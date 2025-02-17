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
    float x = prt->x;
    float y = prt->y;
    float m = prt->m;

    // If outside of bounds and NOT going in the right direction,
    // reverse direction (with some inhibiting factor)
    int within_width = x - m > 0 && x + m < WIN_WIDTH;
    int within_height = y - m > 0 && y + m < WIN_HEIGHT;
    if (!within_width)
    {
        if (x - m < 0)
        {
            prt->x = m;
        }
        else
        {
            prt->x = WIN_WIDTH - m;
        }
        prt->vx *= -RESISTANCE;
    }
    if (!within_height)
    {
        if (y - m < 0)
        {
            prt->y = m;
        }
        else
        {
            prt->y = WIN_HEIGHT - m;
        }
        prt->vy *= -RESISTANCE;
    }
}

// Perform a collision between two points.
// Physics from this link, collision is elastic
// (https://phys.libretexts.org/Courses/Joliet_Junior_College/Physics_201_-_Fall_2019v2/Book%3A_Custom_Physics_textbook_for_JJC/09%3A_Linear_Momentum_and_Collisions/9.16%3A_Collisions)
void collide(Particle* prt1, Particle* prt2)
{
    // Postions, masses and velocities of the two particles.
    float vx1 = prt1->vx, vy1 = prt1->vy, vx2 = prt2->vx, vy2 = prt2->vy;
    float m1 = prt1->m, m2 = prt2->m;
    float x1 = prt1->x, y1 = prt1->y, x2 = prt2->x, y2 = prt2->y;

    // Final velocities.
    float fvx1, fvy1, fvx2, fvy2;

    fvx1 = (m1 - m2) / (m1 + m2) * vx1 + (2 * m2) / (m1 + m2) * vx2;
    fvy1 = (m1 - m2) / (m1 + m2) * vy1 + (2 * m2) / (m1 + m2) * vy2;
    fvx2 = (m2 - m1) / (m1 + m2) * vx2 + (2 * m1) / (m1 + m2) * vx1;
    fvy2 = (m2 - m1) / (m1 + m2) * vy2 + (2 * m1) / (m1 + m2) * vy1;

    prt1->vx = fvx1;
    prt1->vy = fvy1;
    prt2->vx = fvx2;
    prt2->vy = fvy2;

    // Make sure they are not within each other after this, so adjust each's position
    // by their proportion of mass between the two.
    float adj1 = m1 / (m1 + m2);
    float adj2 = m2 / (m1 + m2);

    // Find the factor by which to multiply the positions
    float hypot = dist(x1, y1, x2, y2);
    float ratio = (m1 + m2 - hypot) / hypot;

    float dx1 = (x1 - x2) * ratio * adj1;
    float dy1 = (y1 - y2) * ratio * adj1;
    float dx2 = (x2 - x1) * ratio * adj2;
    float dy2 = (y2 - y1) * ratio * adj2;

    fprintf(stderr, "COLLISION!!! ratio: %f, dx1: %f, dy1: %f, dx2: %f, dy2: %f\n", ratio, dx1, dy1, dx2, dy2);

    prt1->x += dx1;
    prt1->y += dy1;
    prt2->x += dx2;
    prt2->y += dy2;
}

// The idx is the index of a point. This point is mutated to be equal to itself
// summed with its distances to the other points in the points array.
void iter_point(int idx, Particle* prts, int mouse_gravity)
{
    float sign_x, sign_y;
    float x, xi, y, yi, m, mi, distance, dvx, dvy, fx, fy;

    for (int i = 0; i < POINT_MAX + mouse_gravity; i++)
    {
        x = prts[idx].x;
        y = prts[idx].y;
        m = prts[idx].m;

        prts[idx].x = x + prts[idx].vx;
        prts[idx].y = y + prts[idx].vy;

        // fprintf(stderr, "pos: (%f, %f), vel: (%f, %f)\n",
        //                 x,
        //                 y,
        //                 prts[idx].vx,
        //                 prts[idx].vy);

        bounce_back(&prts[idx]);
        if (i == idx) continue;

        xi = prts[i].x;
        yi = prts[i].y;
        mi = prts[i].m;
        sign_x = sign(xi - x) == 0 ? 1 : sign(xi - x);
        sign_y = sign(yi - y) == 0 ? 1 : sign(yi - y);
        distance = dist(x, y, xi, yi);

        if (distance < m + mi)
        {
            collide(&prts[idx], &prts[i]);
        }

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
