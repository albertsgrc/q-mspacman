#ifndef ARGUMENTS_HH
#define ARGUMENTS_HH

#include <string>
#include <random>
using namespace std;

enum Pacman_AI_Agent {
    PATHFINDING, INPUT, RANDOM, RL, NN
};

static const string PACMAN_AI_AGENT_STRINGS[5] = { "pathfinding", "input", "random", "rl", "nn" };

enum Fitness_Attribute {
    COMPLETION, WINS
};

static const string FITNESS_ATTRIBUTES_STRINGS[2] = { "completion", "wins" };

enum Exploration_Strategy{
    ANNEALING, EPSILON
};

static const string EXPLORATION_STRATEGY_STRINGS[2] = { "annealing", "epsilon" };

const string MAZE_FOLDER = "./mazes/";

// ## DEFAULT VALUES ##

/** SIMULATION ARGUMENTS **/

// Note that here a round is considered to be the time it takes
// for pacman to move one cell, which is not the same as an actual game round

const string DFL_MAZE_PATH = MAZE_FOLDER + "originalClassic.lay";

const Pacman_AI_Agent DFL_PACMAN_AI_AGENT = RL;

const float DFL_PACMAN_SPEED = 0.5;
const float DFL_GHOST_SPEED = 0.8; // Relative to pacman
const float DFL_GHOST_AFRAID_SPEED_FRACTION = 0.55; // Relative to their current speed

// The duration in rounds of a scatter mode cycle will be divided
// by this factor at the end of every cycle
const float DFL_SCATTER_CYCLE_FACTOR = 1.03;
// The duration in rounds of a chase mode cycle will be multiplied
// by this factor at the end of every cycle
const float DFL_CHASE_CYCLE_FACTOR = 1.05;
// Initial duration of a scatter cycle in rounds.

const int DFL_INITIAL_SCATTER_CYCLE_ROUNDS = 55;
// Same for chase cycle
const int DFL_INITIAL_CHASE_CYCLE_ROUNDS = 20;

// Standard deviation of the normal distribution used to decide the number
// of ghost mode cycles when there is a change in the mode
const float DFL_CYCLE_ROUNDS_STDEV = 4;

const int DFL_N_ROUNDS_POWERPILL = 35;
const int DFL_N_ROUNDS_GHOST_REVIVE = 20;

// For a value of X, ghost #0 will start moving at round 1, ghost #1 will start
// at round X, ghost #2 will start at round 2*X, ghost #3 will start at round 3*X,
// and so on. Note that round here also means the time it takes for pacman to move one cell
const int DFL_N_ROUNDS_BETWEEN_GHOSTS_START = 4;

const int DFL_PLAYS = 10000;

const size_t DFL_RANDOM_SEED = time(0);

/** NEURAL NETWORK ARGUMENTS **/

const int DFL_N_HIDDEN_LAYERS = 1;
const int DFL_N_HIDDEN_NEURONS = 125;
// For the random initialization of the weights in the neural network

const double DFL_LEARNING_RATE = 0.00015;


/** Q-LEARNING ARGUMENTS **/

const double DFL_REWARD_PILL = 9;
const double DFL_REWARD_POWERPILL = 11;
const double DFL_REWARD_KILL_GHOST = 6;
const double DFL_REWARD_WIN = 60;
const double DFL_REWARD_LOSE = -350;
const double DFL_REWARD_REVERSE = -1;
const double DFL_REWARD_STEP = -4;

const double DFL_DISCOUNT_FACTOR = 0.95;

// When active doesn't allow pacman to reverse its direction
// when performing exploration, nor change its target direction
// until it has moved from its current cell
const bool DFL_SMART_EXPLORATION = true;

const Exploration_Strategy DFL_EXPLORATION_STRATEGY = ANNEALING;

const double DFL_EXPLORATION_ANNEALING_MIN_PROGRESSION = 0.15;
const double DFL_EXPLORATION_ANNEALING_MAX_PROGRESSION = 0.95;
const double DFL_EXPLORATION_EPSILON = 0.25;
const double DFL_EXPLORATION_EPSILON_STOP_PROGRESSION = 0.95;

/** Q-LEARNING INPUT ALGORITHMS ARGUMENTS **/

// Pacman would not die unless staying still this number of rounds after reaching an intersection considered safe
const int DFL_SAFE_DISTANCE_MARGIN = 3;
// For a value of X, we only consider positions <= X intersections away
// pacman's current position for the inputs
const int DFL_MAX_INTERSECTION_DISTANCE = 3;

/** INFORMATION LOGGING ARGUMENTS **/

// For a value of X, logging information will be averaged over
// last X observations
const int DFL_LOGGING_STATISTICS_PRECISION = 100;
// For a value of X, the logging information line will be updated every X game plays
const int DFL_LOGGING_UPDATE_RATE = 5;

const double DFL_VISUALIZATION_SPEED = 1;

const bool DFL_NON_INTERACTIVE = false;

/** AI TESTING **/

// Number of games to test the reinforcement learning AI on after
// training
const int DFL_N_GAMES_TEST = 5000;

const int DFL_TEST_STATISTICS_PRECISION = 75;

/** BEST AI SELECTION **/

const double DFL_NN_EVALUATION_START = 0.85;
const Fitness_Attribute DFL_NN_EVALUATION_ATTRIBUTE = WINS;

/** DEBUGGING **/

// Whether to show the current input values to the console
const bool DFL_SHOW_INPUTS = false;

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
    6. Add string and value to the json map
*/


class Arguments {
public:

    static string maze_path;
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
    static double learning_rate;
    static double reward_pill;
    static double reward_powerpill;
    static double reward_kill_ghost;
    static double reward_win;
    static double reward_lose;
    static double reward_reverse;
    static double reward_step;
    static double discount_factor;
    static bool smart_exploration;
    static int safe_distance_margin;
    static int max_intersection_distance;
    static int logging_statistics_precision;
    static int logging_update_rate;
    static int n_games_test;
    static string neural_network_path;
    static bool show_inputs;
    static double visualization_speed;
    static int test_statistics_precision;
    static bool non_interactive;
    static double nn_evaluation_start;
    static Fitness_Attribute nn_evaluation_attribute;
    static Exploration_Strategy exploration_strategy;
    static double exploration_annealing_min_progression;
    static double exploration_annealing_max_progression;
    static double exploration_epsilon;
    static double exploration_epsilon_stop_progression;

    static void init(int argc, char* argv[]);

    static void parse_key_value(const string& arg, string& key, string& value);

    static void assign_argument(const string& key, const string& value);

    static void treat_arg(const string& arg);

    static void postprocess();

    static vector<pair<string, string>> create_json();
};

string Arguments::maze_path;
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
double Arguments::learning_rate;
double Arguments::reward_pill;
double Arguments::reward_powerpill;
double Arguments::reward_kill_ghost;
double Arguments::reward_win;
double Arguments::reward_lose;
double Arguments::reward_reverse;
double Arguments::reward_step;
double Arguments::discount_factor;
bool Arguments::smart_exploration;
int Arguments::safe_distance_margin;
int Arguments::max_intersection_distance;
int Arguments::logging_statistics_precision;
int Arguments::logging_update_rate;
int Arguments::n_games_test;
string Arguments::neural_network_path;
bool Arguments::show_inputs;
double Arguments::visualization_speed;
int Arguments::test_statistics_precision;
bool Arguments::non_interactive;
double Arguments::nn_evaluation_start;
Fitness_Attribute Arguments::nn_evaluation_attribute;
Exploration_Strategy Arguments::exploration_strategy;
double Arguments::exploration_annealing_min_progression;
double Arguments::exploration_annealing_max_progression;
double Arguments::exploration_epsilon;
double Arguments::exploration_epsilon_stop_progression;

void Arguments::init(int argc, char* argv[]) {
    Arguments::maze_path = DFL_MAZE_PATH;
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
    Arguments::learning_rate = DFL_LEARNING_RATE;
    Arguments::reward_pill = DFL_REWARD_PILL;
    Arguments::reward_powerpill = DFL_REWARD_POWERPILL;
    Arguments::reward_kill_ghost = DFL_REWARD_KILL_GHOST;
    Arguments::reward_win = DFL_REWARD_WIN;
    Arguments::reward_lose = DFL_REWARD_LOSE;
    Arguments::reward_reverse = DFL_REWARD_REVERSE;
    Arguments::reward_step = DFL_REWARD_STEP;
    Arguments::discount_factor = DFL_DISCOUNT_FACTOR;
    Arguments::smart_exploration = DFL_SMART_EXPLORATION;
    Arguments::safe_distance_margin = DFL_SAFE_DISTANCE_MARGIN;
    Arguments::max_intersection_distance = DFL_MAX_INTERSECTION_DISTANCE;
    Arguments::logging_statistics_precision = DFL_LOGGING_STATISTICS_PRECISION;
    Arguments::logging_update_rate = DFL_LOGGING_UPDATE_RATE;
    Arguments::n_games_test = DFL_N_GAMES_TEST;
    Arguments::show_inputs = DFL_SHOW_INPUTS;
    Arguments::visualization_speed = DFL_VISUALIZATION_SPEED;
    Arguments::test_statistics_precision = DFL_TEST_STATISTICS_PRECISION;
    Arguments::non_interactive = DFL_NON_INTERACTIVE;
    Arguments::nn_evaluation_start = DFL_NN_EVALUATION_START;
    Arguments::nn_evaluation_attribute = DFL_NN_EVALUATION_ATTRIBUTE;
    Arguments::exploration_strategy = DFL_EXPLORATION_STRATEGY;
    Arguments::exploration_annealing_min_progression = DFL_EXPLORATION_ANNEALING_MIN_PROGRESSION;
    Arguments::exploration_annealing_max_progression = DFL_EXPLORATION_ANNEALING_MAX_PROGRESSION;
    Arguments::exploration_epsilon = DFL_EXPLORATION_EPSILON;
    Arguments::exploration_epsilon_stop_progression = DFL_EXPLORATION_EPSILON_STOP_PROGRESSION;

    for (int i = 1; i < argc; ++i) treat_arg(argv[i]);
}

bool stob(const string& v) {
    return v != "0" and v != "false";
}

void Arguments::assign_argument(const string& key, const string& value) {
    if (key == "maze") Arguments::maze_path = MAZE_FOLDER + value;
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
        else if (value[0] == value[1] and value[1] == 'n') {
            Arguments::pacman_ai_agent = NN;
            string extension = ".txt";
            Arguments::neural_network_path = "../data/neural-networks/" + value + extension;
        }
        else error("Invalid pacman AI agent name '" + value + "'");
    }
    else if (key == "plays") Arguments::plays = stoi(value);
    else if (key == "random_seed") {
        if (value == "time") Arguments::random_seed = time(0);
        else Arguments::random_seed = stoul(value);
    }
    else if (key == "n_hidden_layers") Arguments::n_hidden_layers = stoi(value);
    else if (key == "n_hidden_neurons") Arguments::n_hidden_neurons = stoi(value);
    else if (key == "learning_rate") Arguments::learning_rate = stod(value);
    else if (key == "reward_pill") reward_pill = stod(value);
    else if (key == "reward_powerpill") reward_powerpill = stod(value);
    else if (key == "reward_kill_ghost") reward_kill_ghost = stod(value);
    else if (key == "reward_win") reward_win = stod(value);
    else if (key == "reward_lose") reward_lose = stod(value);
    else if (key == "reward_reverse") reward_reverse = stod(value);
    else if (key == "reward_step") reward_step = stod(value);
    else if (key == "discount_factor") discount_factor = stod(value);
    else if (key == "smart_exploration") smart_exploration = stob(value);
    else if (key == "safe_distance_margin") safe_distance_margin = stoi(value);
    else if (key == "max_intersection_distance") max_intersection_distance = stoi(value);
    else if (key == "logging_statistics_precision") logging_statistics_precision = stoi(value);
    else if (key == "logging_update_rate") logging_update_rate = stoi(value);
    else if (key == "n_games_test") n_games_test = stoi(value);
    else if (key == "show_inputs") show_inputs = stob(value);
    else if (key == "visualization_speed") visualization_speed = stod(value);
    else if (key == "test_statistics_precision") test_statistics_precision = stoi(value);
    else if (key == "non_interactive") non_interactive = stob(value);
    else if (key == "nn_evaluation_start") nn_evaluation_start = stod(value);
    else if (key == "nn_evaluation_attribute") {
        if (value == "wins") nn_evaluation_attribute = WINS;
        else if (value == "completion") nn_evaluation_attribute = COMPLETION;
        else error("Invalid neural network evaluation attribute name '" + value + "'");
    }
    else if (key == "exploration_strategy") {
        if (value.substr(0, 9) == "annealing") exploration_strategy = ANNEALING;
        else if (value.substr(0, 7) == "epsilon") exploration_strategy = EPSILON;
        else error("Invalid exploration strategy attribute name '" + value + "'");
    }
    else if (key == "exploration_annealing_min_progression") exploration_annealing_min_progression = stod(value);
    else if (key == "exploration_annealing_max_progression") exploration_annealing_max_progression = stod(value);
    else if (key == "exploration_epsilon") exploration_epsilon = stod(value);
    else if (key == "exploration_epsilon_stop_progression") exploration_epsilon_stop_progression = stod(value);
    else error("Invalid argument name '" + key + "'");
}

inline vector<pair<string, string>> Arguments::create_json() {
    return {
            { "maze", '"' + maze_path + '"' },
            { "ghost_speed", to_string(ghost_speed) },
            { "pacman_speed", to_string(pacman_speed) },
            { "ghost_afraid_speed_fraction", to_string(ghost_afraid_speed_fraction) },
            { "n_rounds_powerpill", to_string(n_rounds_powerpill) },
            { "n_rounds_ghost_revive", to_string(n_rounds_ghost_revive) },
            { "scatter_cycle_factor", to_string(scatter_cycle_factor) },
            { "chase_cycle_factor", to_string(chase_cycle_factor) },
            { "initial_scatter_cycle_rounds", to_string(initial_scatter_cycle_rounds) },
            { "initial_chase_cycle_rounds", to_string(initial_chase_cycle_rounds) },
            { "cycle_rounds_stdev", to_string(cycle_rounds_stdev) },
            { "n_rounds_between_ghosts_start", to_string(n_rounds_between_ghosts_start) },
            { "pacman_ai_agent", '"' + (pacman_ai_agent == NN ? neural_network_path : PACMAN_AI_AGENT_STRINGS[pacman_ai_agent]) + '"' },
            { "plays", to_string(plays) },
            { "random_seed", to_string(Arguments::random_seed ) },
            { "n_hidden_layers", to_string(n_hidden_layers) },
            { "n_hidden_neurons", to_string(n_hidden_neurons) },
            { "learning_rate", to_string(learning_rate) },
            { "reward_pill", to_string(reward_pill) },
            { "reward_powerpill", to_string(reward_powerpill) },
            { "reward_kill_ghost", to_string(reward_kill_ghost) },
            { "reward_win", to_string(reward_win) },
            { "reward_lose", to_string(reward_lose) },
            { "reward_reverse", to_string(reward_reverse) },
            { "reward_step", to_string(reward_step) },
            { "discount_factor", to_string(discount_factor) },
            { "smart_exploration", to_string(smart_exploration) },
            { "safe_distance_margin", to_string(safe_distance_margin) },
            { "max_intersection_distance", to_string(max_intersection_distance) },
            { "logging_statistics_precision", to_string(logging_statistics_precision) },
            { "logging_update_rate", to_string(logging_update_rate) },
            { "n_games_test", to_string(n_games_test) },
            { "show_inputs", to_string(show_inputs) },
            { "visualization_speed", to_string(visualization_speed) },
            { "test_statistics_precision", to_string(test_statistics_precision) },
            { "non_interactive", to_string(non_interactive) },
            { "nn_evaluation_start", to_string(nn_evaluation_start) },
            { "nn_evaluation_attribute", '"' + FITNESS_ATTRIBUTES_STRINGS[nn_evaluation_attribute] + '"'},
            { "exploration_strategy", '"' + EXPLORATION_STRATEGY_STRINGS[exploration_strategy] + '"' },
            { "exploration_annealing_min_progression", to_string(exploration_annealing_min_progression) },
            { "exploration_annealing_max_progression", to_string(exploration_annealing_max_progression) },
            { "exploration_epsilon", to_string(exploration_epsilon) },
            { "exploration_epsilon_stop_progression", to_string(exploration_epsilon_stop_progression) }
    };
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

    Arguments::visualization_speed = 180000*Arguments::ghost_speed/Arguments::visualization_speed;

    if (Arguments::pacman_ai_agent == INPUT) Arguments::plays = 1;
}

#endif
