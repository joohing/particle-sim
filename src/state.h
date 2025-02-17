// The gravitational constant (https://en.wikipedia.org/wiki/Gravitational_constant)
// but a bit larger
static const float G = 0.00006674;

// Whether or not to simulate gravity from Earth only, meaning no gravity between particles
static const int EARTH_GRAVITY_ONLY = 1;

// Factor to multiply speed by when hitting the edge of the window.
static const float RESISTANCE = 1;

// Amount of points to make.
static const int POINT_MAX = 1;

// The mass controls acceleration of other points towards this one,
// as well as the radius. When generating random points they will get
// a random number between these two numbers.
static const int MIN_MASS = 2;
static const int MAX_MASS = 10;

// How quick the start-up speed should be
static const int VX_MIN = 1;
static const int VX_MAX = 1;
static const int VY_MIN = 1;
static const int VY_MAX = 1;

// How large the steps are for each simulation step.
static const float ITER_SCALE = 1;

// Screen size.
static const int WIN_WIDTH = 1500;
static const int WIN_HEIGHT = 1000;

// This makes the particles get some acceleration towards the cursor.
static const int MOUSE_MASS = 100;

// How many ticks between each frame, at minimum.
static const int FRAMETIME = 20;

typedef struct Particle {
    double x;
    double y;
    double vx;
    double vy;
    double m;
} Particle;

// This is where the state of the application is centralized. All subsequent states are
// deduced from the current one. The state is just the current particles' positions.
// Plus one to have the mouse as well.
typedef struct State {
    Particle prts[POINT_MAX + 1];
} State;

State* get_sample_state();
void free_state_struct(State* state);
void print_particles(Particle prts[]);
void iter_state(State* state, int mouse_gravity);
