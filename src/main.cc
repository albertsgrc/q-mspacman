#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <time.h>
#include <sstream>

#define DEV 1 // set dev mode
#define DEBUG 1 // set debug mode

#include "arguments.hh"
#include "game.hh"
#include "agents/pacman/pathfinding_pacman_agent.hh"
#include "agents/pacman/input_pacman_agent.hh"
#include "agents/pacman/random_pacman_agent.hh"
#include "agents/pacman/rl/rl_pacman_agent.hh"
#include "agents/pacman/rl/nn_pacman_agent.hh"

struct Statistics {
    bool won;
    double completion;
    double mse;

    Statistics(uint won, double completion, double mse) : won(won), completion(completion), mse(mse) {}
};

string id() {
    stringstream ss;
    ss << time(0) << rand()%100;
    return ss.str();
}

int main(int argc, char* argv[]) {
    cout.setf(ios::fixed);
    cout.precision(2);

    Arguments::init(argc, argv);
    Arguments::postprocess();

    srand(Arguments::random_seed);

    Game game;

    game.load_maze();

    Agent* pacman_ai;

    switch (Arguments::pacman_ai_agent) {
        case PATHFINDING: pacman_ai = new Pathfinding_Pacman_Agent(); break;
        case INPUT: pacman_ai = new Input_Pacman_Agent(); break;
        case RANDOM: pacman_ai = new Random_Pacman_Agent(); break;
        case RL: pacman_ai = new RL_Pacman_Agent(); break;
        case NN: pacman_ai = new NN_Pacman_Agent(Arguments::neural_network_path); break;
        default: ensure(false, "Invalid pacman AI Agent enum value");
    }

    game.set_ai(pacman_ai);

    uint total_won_always = 0;
    uint total_won = 0;
    double total_completion = 0;
    double total_mse = 0.0;
    queue<Statistics> Q;

    for (int i = 0; i < Arguments::plays; ++i) {
        game.play();

        double completion = game.completion();
        double mse = Arguments::pacman_ai_agent == RL ? ((RL_Pacman_Agent*)(pacman_ai))->mse_sum_last/game.state.round : 0;
        Q.push(Statistics(game.result.won, completion, mse));

        total_won_always += game.result.won;
        total_won += game.result.won;
        total_completion += completion;
        total_mse += mse;

        if (i >= Arguments::logging_statistics_precision) {
            Statistics r = Q.front();
            Q.pop();
            total_won -= r.won;
            total_completion -= r.completion;
            total_mse -= r.mse;
        }

        if (i + 1 >= Arguments::logging_statistics_precision and i%Arguments::logging_update_rate == Arguments::logging_update_rate - 1) {
            /*if (Arguments::pacman_ai_agent == RL and 100*total_won/double(Arguments::logging_statistics_precision) >= 70.0) {
                string nn_path = "../data/neural-networks-70/nn" + to_string(int(100*total_won/double(Arguments::logging_statistics_precision))) + "-" + id() + ".txt";
                ((RL_Pacman_Agent *) (pacman_ai))->nn.write_file(nn_path);
            }*/

            cout << "\rWins (last " << Arguments::logging_statistics_precision << "): " << 100*total_won/double(Arguments::logging_statistics_precision) << '%'
                 << " (" << total_won_always << "/" << i + 1 << ") ::: "
                 << 100*total_completion/double(Arguments::logging_statistics_precision)
                 << "% completion (last " << Arguments::logging_statistics_precision << ")";

            if (Arguments::pacman_ai_agent == RL)
                cout << " :: MSE " << total_mse/Arguments::logging_statistics_precision;

            cout.flush();
        }

        game.reset();
    }

    if (Arguments::pacman_ai_agent == RL) {
        cout << endl;

        RL_Pacman_Agent* agent = ((RL_Pacman_Agent*)(pacman_ai));
        system("mkdir -p ../data/neural-networks");
        string nn_path = "../data/neural-networks/nn" + id() + ".txt";
        agent->nn.write_file(nn_path);

        Game game_test;

        total_completion = 0.0;
        total_won = 0;

        game_test.load_maze();

        pacman_ai = new NN_Pacman_Agent(nn_path);
        game_test.set_ai(pacman_ai);

        for (int i = 0; i < Arguments::n_games_test; ++i) {
            game_test.play();

            total_completion += game_test.completion();
            total_won += game_test.result.won;
            if (i%Arguments::logging_update_rate == Arguments::logging_update_rate - 1) {
                cout << "\rWins " << 100 * total_won / double(i + 1) << '%'
                     << " (" << total_won << "/" << i + 1 << ") ::: "
                     << 100 * total_completion / double(i + 1)
                     << "% completion";

                cout.flush();
            }

            game_test.reset();
        }
    }

    cout << endl;
}
