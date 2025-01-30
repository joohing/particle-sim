static const int POINT_MAX = 8;
static const int POINT_RADIUS = 3;

// This is where the state of the application is centralized. All subsequent states are
// deduced from the current one. The state is just the current particles' positions.
typedef struct State {
    SDL_Point *particles[POINT_MAX];
} State;

State* get_sample_state();
void print_points(SDL_Point points[]);
