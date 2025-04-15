// The gravitational constant (https://en.wikipedia.org/wiki/Gravitational_constant)
// but a bit larger
static const double G = 0.0006674;

// Whether or not to simulate gravity from Earth
static const int ENABLE_EARTH_GRAVITY = 0;

// Whether or not to simulate gravity from Earth
static const int ENABLE_INTERPARTICULAR_GRAVITY = 1;

// Factor to multiply speed by when hitting the edge of the window.
static const float RESISTANCE = 1;

// Amount of points to make.
static const int POINT_MAX = 15;

// The mass controls acceleration of other points towards this one,
// as well as the radius. When generating random points they will get
// a random number between these two numbers.
static const int MIN_MASS = 10;
static const int MAX_MASS = 25;

// How many pixels make up one meter
static const int PIXELS_PER_METER = 3;

// How quick the start-up speed should be
static const int VX_MIN = -3;
static const int VX_MAX = 3;
static const int VY_MIN = -3;
static const int VY_MAX = 3;

// Screen size.
static const int WIN_WIDTH = 1200;
static const int WIN_HEIGHT = 1000;

// Tolerance for error in e.g. energy preservation.
static const double EPS = 0.0003;

// Bounds on the randomly generated start position.
static const int X_MIN = WIN_WIDTH / 4;
static const int X_MAX = WIN_WIDTH * 3 / 4;
static const int Y_MIN = WIN_HEIGHT / 4;
static const int Y_MAX = WIN_HEIGHT * 3 / 4;

// How large the steps are for each simulation step.
static const float ITER_SCALE = 1;

// This makes the particles get some acceleration towards the cursor.
static const int MOUSE_MASS = 100;

// How many ticks between each frame, at minimum.
static const int FRAMETIME = 10;

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
State* get_test_state();
void free_state_struct(State* state);
void print_particles(Particle prts[]);
void iter_state(State* state, int mouse_gravity);
