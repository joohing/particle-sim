#include </opt/homebrew/Cellar/sdl2/2.30.10/include/SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "state.h"
#include <math.h>
#include "jonamath.h"

// Controls the rate of change of the position of particles.
double DX = 1;
double DY = 1;

// Seed before calling with srand(time(NULL))
int rng(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

// Absolute distance
double dist(float x1, float y1, float x2, float y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
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
    double x = prt->x;
    double y = prt->y;
    double m = prt->m;

    int within_width = x - m >= 0 && x + m <= WIN_WIDTH;
    int within_height = y - m >= 0 && y + m <= WIN_HEIGHT;
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
// Collision is elastic. Read (https://en.wikipedia.org/wiki/Elastic_collision), two-dimensional case
void collide(Particle* prt1, Particle* prt2)
{
    // Final velocities.
    double fvx1, fvy1, fvx2, fvy2;

    // Postions, masses, and velocities of the two particles.
    double x1 = prt1->x, y1 = prt1->y, x2 = prt2->x, y2 = prt2->y;
    double m1 = prt1->m, m2 = prt2->m;
    double vx1 = prt1->vx, vy1 = prt1->vy, vx2 = prt2->vx, vy2 = prt2->vy;
    double dst = dist(x1, y1, x2, y2);

    fvx1 = vx1 - (2 * m2) / (m1 + m2) * ((vx1 - vx2) * (x1 - x2)) / (dst * dst) * (x1 - x2);
    fvy1 = vy1 - (2 * m2) / (m1 + m2) * ((vy1 - vy2) * (y1 - y2)) / (dst * dst) * (y1 - y2);

    fvx2 = vx2 - (2 * m1) / (m1 + m2) * ((vx2 - vx1) * (x2 - x1)) / (dst * dst) * (x2 - x1);
    fvy2 = vy2 - (2 * m1) / (m1 + m2) * ((vy2 - vy1) * (y2 - y1)) / (dst * dst) * (y2 - y1);

    fprintf(stderr, "Collision!\n"
                    "Initial vels:\n  prt1: (%f, %f)\n  prt2: (%f, %f)\n"
                    "Final vels:\n  prt1: (%f, %f)\n  prt2: (%f, %f)\n"
                    "Masses:\n  prt1: %f\n  prt2: %f\n"
                    "Distance:\n  %f\n",
                     vx1, vy1, vx2, vy2,
                     fvx1, fvy1, fvx2, fvy2,
                     m1, m2, dst);

    prt1->vx = fvx1;
    prt1->vy = fvy1;
    prt2->vx = fvx2;
    prt2->vy = fvy2;

    // Make sure they are not within each other after this, so adjust each's position
    // by their proportion of mass between the two.
    double adj1 = m1 / (m1 + m2);
    double adj2 = m2 / (m1 + m2);

    // Find the factor by which to multiply the positions
    double hypot = dst;
    double ratio = (m1 + m2 - hypot) / hypot;

    double dx1 = (x1 - x2) * ratio * adj1;
    double dy1 = (y1 - y2) * ratio * adj1;
    double dx2 = (x2 - x1) * ratio * adj2;
    double dy2 = (y2 - y1) * ratio * adj2;

    // fprintf(stderr, "COLLISION!!! ratio: %f, dx1: %f, dy1: %f, dx2: %f, dy2: %f\n", ratio, dx1, dy1, dx2, dy2);

    prt1->x += dx1;
    prt1->y += dy1;
    prt2->x += dx2;
    prt2->y += dy2;
}

// The idx is the index of a point. This point is mutated to be equal to itself
// summed with its distances to the other points in the points array.
void iter_point(int idx, Particle* prts, int mouse_gravity)
{
    double sign_x, sign_y;
    double x, xi, y, yi, m, mi, distance, dvx, dvy, fx, fy;

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
        if (EARTH_GRAVITY_ONLY)
        {
            prts[idx].vy += 0.00981;
            continue;
        }
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
