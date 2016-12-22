#ifndef ARGUMENTS_HH
#define ARGUMENTS_HH

#include <string>
#include <random>
using namespace std;

enum Pacman_AI_Agent {
    PATHFINDING, INPUT, RANDOM, RL
};

const string LAYOUT_FOLDER = "./layouts/";

// ## DEFAULT VALUES ##

/** SIMULATION ARGUMENTS **/

// Note that here a round is considered to be the time it takes
// for pacman to move one cell, which is not the same as an actual game round

const string DFL_LAYOUT_PATH = LAYOUT_FOLDER + "originalClassic.lay";

const Pacman_AI_Agent DFL_PACMAN_AI_AGENT = RL;

const float DFL_PACMAN_SPEED = 0.5;
const float DFL_GHOST_SPEED = 0.85; // Relative to pacman
const float DFL_GHOST_AFRAID_SPEED_FRACTION = 0.65; // Relative to their current speed

// The duration in rounds of a scatter mode cycle will be divided
// by this factor at the end of every cycle
const float DFL_SCATTER_CYCLE_FACTOR = 1.03;
// The duration in rounds of a chase mode cycle will be multiplied
// by this factor at the end of every cycle
const float DFL_CHASE_CYCLE_FACTOR = 1.1;
// Initial duration of a scatter cycle in rounds.

const int DFL_INITIAL_SCATTER_CYCLE_ROUNDS = 30;
// Same for chase cycle
const int DFL_INITIAL_CHASE_CYCLE_ROUNDS = 30;

// Standard deviation of the normal distribution used to decide the number
// of ghost mode cycles when there is a change in the mode
const float DFL_CYCLE_ROUNDS_STDEV = 4;

const int DFL_N_ROUNDS_POWERPILL = 35;
const int DFL_N_ROUNDS_GHOST_REVIVE = 15;

// For a value of X, ghost #0 will start moving at round 1, ghost #1 will start
// at round X, ghost #2 will start at round 2*X, ghost #3 will start at round 3*X,
// and so on. Note that round here also means the time it takes for pacman to move one cell
const int DFL_N_ROUNDS_BETWEEN_GHOSTS_START = 4;

const int DFL_PLAYS = 100000;

const size_t DFL_RANDOM_SEED = time(0);

/** NEURAL NETWORK ARGUMENTS **/

const int DFL_N_HIDDEN_LAYERS = 1;
const int DFL_N_HIDDEN_NEURONS = 100;
const double DFL_MIN_WEIGHT_INIT = -0.3;
const double DFL_MAX_WEIGHT_INIT = 0.3;
const double DFL_LEARNING_RATE = 0.0003;


/** Q-LEARNING ARGUMENTS **/

const double DFL_REWARD_PILL = 12;
const double DFL_REWARD_POWERPILL = 3;
const double DFL_REWARD_KILL_GHOST = 20;
const double DFL_REWARD_WIN = 50;
const double DFL_REWARD_LOSE = -350;
const double DFL_REWARD_REVERSE = -6;
const double DFL_REWARD_STEP = -10;

const double DFL_DISCOUNT_FACTOR = 0.95;

typedef unsigned int uint;
void error(const string& msg) {
    perror(msg.c_str());
    exit(1);
}

/*
    Steps to add a new argument with name "argument"

    1. Add default value constant: const string DFL_ARGUMENT = <default value>
    2. Declare member: static <type> argument; inside Arguments class
    3. Implement member: <type> argument; after Arguments class declaration
    4. Assign default value inside init function: Arguments::argument = DFL_ARGUMENT;
    5. Detect and parse argument inside assign_argument function: if (key == "argument") argument = <parse>(value);
*/


class Arguments {
public:

    static string layout_path;
    static float ghost_speed; // not relative
    static float pacman_speed;
    static float ghost_afraid_speed_fraction;
    static int n_rounds_powerpill;
    static int n_rounds_ghost_revive;
    static float scatter_cycle_factor;
    static float chase_cycle_factor;
    static int initial_scatter_cycle_rounds;
    static int initial_chase_cycle_rounds;
    static float cycle_rounds_stdev;
    static int n_rounds_between_ghosts_start;
    static Pacman_AI_Agent pacman_ai_agent;
    static int plays;
    static uint random_seed;
    static mt19937_64 random_generator;
    static int n_hidden_layers;
    static int n_hidden_neurons;
    static double min_weight_init;
    static double max_weight_init;
    static double learning_rate;
    static double reward_pill;
    static double reward_powerpill;
    static double reward_kill_ghost;
    static double reward_win;
    static double reward_lose;
    static double reward_reverse;
    static double reward_step;
    static double discount_factor;

    static void init(int argc, char* argv[]);

    static void parse_key_value(const string& arg, string& key, string& value);

    static void assign_argument(const string& key, const string& value);

    static void treat_arg(const string& arg);

    static void postprocess();
};

string Arguments::layout_path;
float Arguments::ghost_speed;
float Arguments::pacman_speed;
float Arguments::ghost_afraid_speed_fraction;
int Arguments::n_rounds_powerpill;
int Arguments::n_rounds_ghost_revive;
float Arguments::scatter_cycle_factor;
float Arguments::chase_cycle_factor;
int Arguments::initial_scatter_cycle_rounds;
int Arguments::initial_chase_cycle_rounds;
float Arguments::cycle_rounds_stdev;
int Arguments::n_rounds_between_ghosts_start;
Pacman_AI_Agent Arguments::pacman_ai_agent;
int Arguments::plays;
uint Arguments::random_seed;
mt19937_64 Arguments::random_generator;
int Arguments::n_hidden_layers;
int Arguments::n_hidden_neurons;
double Arguments::min_weight_init;
double Arguments::max_weight_init;
double Arguments::learning_rate;
double Arguments::reward_pill;
double Arguments::reward_powerpill;
double Arguments::reward_kill_ghost;
double Arguments::reward_win;
double Arguments::reward_lose;
double Arguments::reward_reverse;
double Arguments::reward_step;
double Arguments::discount_factor;

void Arguments::init(int argc, char* argv[]) {
    Arguments::layout_path = DFL_LAYOUT_PATH;
    Arguments::ghost_speed = DFL_GHOST_SPEED;
    Arguments::pacman_speed = DFL_PACMAN_SPEED;
    Arguments::ghost_afraid_speed_fraction = DFL_GHOST_AFRAID_SPEED_FRACTION;
    Arguments::n_rounds_powerpill = DFL_N_ROUNDS_POWERPILL;
    Arguments::n_rounds_ghost_revive = DFL_N_ROUNDS_GHOST_REVIVE;
    Arguments::scatter_cycle_factor = DFL_SCATTER_CYCLE_FACTOR;
    Arguments::chase_cycle_factor = DFL_CHASE_CYCLE_FACTOR;
    Arguments::initial_scatter_cycle_rounds = DFL_INITIAL_SCATTER_CYCLE_ROUNDS;
    Arguments::initial_chase_cycle_rounds = DFL_INITIAL_CHASE_CYCLE_ROUNDS;
    Arguments::cycle_rounds_stdev = DFL_CYCLE_ROUNDS_STDEV;
    Arguments::n_rounds_between_ghosts_start = DFL_N_ROUNDS_BETWEEN_GHOSTS_START;
    Arguments::pacman_ai_agent = DFL_PACMAN_AI_AGENT;
    Arguments::plays = DFL_PLAYS;
    Arguments::random_seed = DFL_RANDOM_SEED;
    Arguments::n_hidden_layers = DFL_N_HIDDEN_LAYERS;
    Arguments::n_hidden_neurons = DFL_N_HIDDEN_NEURONS;
    Arguments::min_weight_init = DFL_MIN_WEIGHT_INIT;
    Arguments::max_weight_init = DFL_MAX_WEIGHT_INIT;
    Arguments::learning_rate = DFL_LEARNING_RATE;
    Arguments::reward_pill = DFL_REWARD_PILL;
    Arguments::reward_powerpill = DFL_REWARD_POWERPILL;
    Arguments::reward_kill_ghost = DFL_REWARD_KILL_GHOST;
    Arguments::reward_win = DFL_REWARD_WIN;
    Arguments::reward_lose = DFL_REWARD_LOSE;
    Arguments::reward_reverse = DFL_REWARD_REVERSE;
    Arguments::reward_step = DFL_REWARD_STEP;
    Arguments::discount_factor = DFL_DISCOUNT_FACTOR;

    for (int i = 1; i < argc; ++i) treat_arg(argv[i]);
}

void Arguments::assign_argument(const string& key, const string& value) {
    if (key == "layout") Arguments::layout_path = LAYOUT_FOLDER + value;
    else if (key == "ghost_speed") Arguments::ghost_speed = stof(value);
    else if (key == "pacman_speed") Arguments::pacman_speed = stof(value);
    else if (key == "ghost_afraid_speed_fraction") Arguments::ghost_afraid_speed_fraction = stof(value);
    else if (key == "n_rounds_powerpill") Arguments::n_rounds_powerpill = stoi(value);
    else if (key == "n_rounds_ghost_revive") Arguments::n_rounds_ghost_revive = stoi(value);
    else if (key == "scatter_cycle_factor") Arguments::scatter_cycle_factor = stof(value);
    else if (key == "chase_cycle_factor") Arguments::chase_cycle_factor = stof(value);
    else if (key == "initial_scatter_cycle_rounds") Arguments::initial_scatter_cycle_rounds = stoi(value);
    else if (key == "initial_chase_cycle_rounds") Arguments::initial_chase_cycle_rounds = stoi(value);
    else if (key == "cycle_rounds_stdev") Arguments::cycle_rounds_stdev = stof(value);
    else if (key == "n_rounds_between_ghosts_start") Arguments::n_rounds_between_ghosts_start = stoi(value);
    else if (key == "pacman_ai_agent") {
        if (value == "pathfinding") Arguments::pacman_ai_agent = PATHFINDING;
        else if (value == "input") Arguments::pacman_ai_agent = INPUT;
        else if (value == "random") Arguments::pacman_ai_agent = RANDOM;
        else if (value == "rl") Arguments::pacman_ai_agent = RL;
        else error("Invalid pacman AI agent name '" + value + "'");
    }
    else if (key == "plays") Arguments::plays = stoi(value);
    else if (key == "random_seed") {
        if (value == "time") Arguments::random_seed = time(0);
        else Arguments::random_seed = stoul(value);
    }
    else if (key == "n_hidden_layers") Arguments::n_hidden_layers = stoi(value);
    else if (key == "n_hidden_neurons") Arguments::n_hidden_neurons = stoi(value);
    else if (key == "min_weight_init") Arguments::min_weight_init = stod(value);
    else if (key == "max_weight_init") Arguments::max_weight_init = stod(value);
    else if (key == "learning_rate") Arguments::learning_rate = stod(value);
    else if (key == "reward_pill") reward_pill = stod(value);
    else if (key == "reward_powerpill") reward_powerpill = stod(value);
    else if (key == "reward_kill_ghost") reward_kill_ghost = stod(value);
    else if (key == "reward_win") reward_win = stod(value);
    else if (key == "reward_lose") reward_lose = stod(value);
    else if (key == "reward_reverse") reward_reverse = stod(value);
    else if (key == "reward_step") reward_step = stod(value);
    else if (key == "discount_factor") discount_factor = stod(value);
    else error("Invalid argument name '" + key + "'");
}

void Arguments::treat_arg(const string& arg) {
    string key, value;
    parse_key_value(arg, key, value);
    assign_argument(key, value);
}

void Arguments::parse_key_value(const string& arg, string& key, string& value) {
    uint i = 0;

    while (i < arg.length() and arg[i] != '=')
        key.push_back(arg[i++]);

    ++i;

    if (i >= arg.length())
        error("Invalid parameter \'" + arg + "\': Should be of the form key=value");

    while (i < arg.length())
        value.push_back(arg[i++]);
}

void Arguments::postprocess() {
    Arguments::ghost_speed *= Arguments::pacman_speed;

    Arguments::initial_chase_cycle_rounds /= Arguments::pacman_speed;
    Arguments::initial_scatter_cycle_rounds /= Arguments::pacman_speed;
    Arguments::cycle_rounds_stdev /= Arguments::pacman_speed;
    Arguments::n_rounds_ghost_revive /= Arguments::pacman_speed;
    Arguments::n_rounds_powerpill /= Arguments::pacman_speed;
    Arguments::n_rounds_between_ghosts_start /= Arguments::pacman_speed;

    Arguments::random_generator = mt19937_64(Arguments::random_seed);

    Arguments::reward_step *= Arguments::pacman_speed;
}

#endif
