#include "SDL2/SDL.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "state.h"
#include <math.h>
#include "jonamath.h"

// Controls the rate of change of the position of particles.
double DX = 10;
double DY = 10;

// Seed before calling with srand(time(NULL))
int rng(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

// Convert meters to pixels and pixels to meters using PIXELS_PER_METER
// When doing gravity calculations, use meters.
double m_to_pix(double m) { return m * PIXELS_PER_METER; }
double pix_to_m(double pix) { return pix / PIXELS_PER_METER; }

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

    // Positions, masses, and velocities of the two particles.
    double x1 = prt1->x, y1 = prt1->y, x2 = prt2->x, y2 = prt2->y;
    double m1 = prt1->m, m2 = prt2->m;
    double vx1 = prt1->vx, vy1 = prt1->vy, vx2 = prt2->vx, vy2 = prt2->vy;

    // Displacements
    double dst = dist(x1, y1, x2, y2);

    // Dot products
    double dot1 = ((vx1 - vx2) * (x1 - x2)) + ((vy1 - vy2) * (y1 - y2));
    double dot2 = ((vx2 - vx1) * (x2 - x1)) + ((vy2 - vy1) * (y2 - y1));

    fvx1 = vx1 - (2.0 * m2) / (m1 + m2) * dot1 / (dst * dst) * (x1 - x2);
    fvy1 = vy1 - (2.0 * m2) / (m1 + m2) * dot2 / (dst * dst) * (y1 - y2);

    fvx2 = vx2 - (2.0 * m1) / (m1 + m2) * dot1 / (dst * dst) * (x2 - x1);
    fvy2 = vy2 - (2.0 * m1) / (m1 + m2) * dot2 / (dst * dst) * (y2 - y1);

    // Absolute velocity for each particle before and after collision.
    double tinit_vel1 = dist(prt1->vx, prt1->vy, 0, 0);
    double tinit_vel2 = dist(prt2->vx, prt2->vy, 0, 0);
    double tafter_vel1 = dist(fvx1, fvy1, 0, 0);
    double tafter_vel2 = dist(fvx2, fvy2, 0, 0);

    // Total energy in the two particles before and after.
    double energy_init = 0.5 * m1 * tinit_vel1 * tinit_vel1
                       + 0.5 * m2 * tinit_vel2 * tinit_vel2;
    double energy_after = 0.5 * m1 * tafter_vel1 * tafter_vel1
                        + 0.5 * m2 * tafter_vel2 * tafter_vel2;

    double delta_energy = energy_init - energy_after;
    int no_energy_loss = -EPS < delta_energy && delta_energy < EPS;
    if (no_energy_loss == 0)
    {
        fprintf(stderr, "Energy loss.\n  before: %f\n  after: %f\n", energy_init, energy_after);
    }

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

    prt1->x += dx1;
    prt1->y += dy1;
    prt2->x += dx2;
    prt2->y += dy2;
}

// Detect if point prts[idx] collided with any points in prts, and apply collision if
// it did.
void detect_collision(int idx, Particle* prts, int mouse_gravity)
{
    double x, y, m, mi, distance;

    x = prts[idx].x;
    y = prts[idx].y;
    m = prts[idx].m;

    for (int i = idx + 1; i < POINT_MAX + mouse_gravity; i++)
    {
        distance = dist(x, y, prts[i].x, prts[i].y);
        mi = prts[i].m;

        if (distance < m + mi)
        {
            collide(&prts[idx], &prts[i]);
        }
    }
}

// Update velocity of prts[idx] according to the positions and masses of other points
// in prts.
void iter_velocity(int idx, Particle* prts, int mouse_gravity)
{
    double x_disp, y_disp;
    double x, xi, y, yi, fx, fy, m, mi;

    prts[idx].x = prts[idx].x + prts[idx].vx;
    prts[idx].y = prts[idx].y + prts[idx].vy;

    x = prts[idx].x;
    y = prts[idx].y;

    if (ENABLE_EARTH_GRAVITY)
    {
        prts[idx].vy += 0.00981;
    }

    if (ENABLE_INTERPARTICULAR_GRAVITY == 0) return;

    for (int i = 0; i < POINT_MAX + mouse_gravity; i++)
    {
        if (i == idx) continue;

        xi = prts[i].x;
        yi = prts[i].y;
        m = prts[idx].m;
        mi = prts[i].m;
        x_disp = xi - x;
        y_disp = yi - y;

        fx = G * m * mi / x_disp;
        fy = G * m * mi / y_disp;

        prts[idx].vx = prts[idx].vx + fx;
        prts[idx].vy = prts[idx].vy + fy;
    }
}

// Mutate the state to simulate one step in the particle simulation.
// mouse_gravity is whether or not the mouse should have gravity right now.
void iter_state(State *state, int mouse_gravity)
{
    int i;
    for (i = 0; i < POINT_MAX; i++)
    {
        iter_velocity(i, (Particle *) state->prts, mouse_gravity);
    }
    for (i = 0; i < POINT_MAX; i++)
    {
        detect_collision(i, (Particle *) state->prts, mouse_gravity);
    }
    for (i = 0; i < POINT_MAX; i++)
    {
        bounce_back(&state->prts[i]);
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
        prts[i].x = rng(X_MIN, X_MAX);
        prts[i].y = rng(Y_MIN, Y_MAX);
    }

    State* state_mem = calloc(1, sizeof(State));
    memcpy(state_mem->prts, prts, sizeof(Particle) * (POINT_MAX + 1));

    return state_mem;
}

State* get_test_state()
{
    srand(time(NULL));

    Particle* prts = calloc(POINT_MAX + 1, sizeof(Particle));
    prts[POINT_MAX].m = MOUSE_MASS;

    prts[0].vx = 0;
    prts[0].vy = 0;
    prts[0].m = 15;
    prts[0].x = 625;
    prts[0].y = 625;

    prts[1].vx = 0;
    prts[1].vy = 0;
    prts[1].m = 10;
    prts[1].x = 500;
    prts[1].y = 500;

    State* state_mem = calloc(1, sizeof(State));
    memcpy(state_mem->prts, prts, sizeof(Particle) * (POINT_MAX + 1));

    return state_mem;
}
