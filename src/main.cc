#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <time.h>
#include <sstream>

#include "arguments.hh"
#include "game.hh"
#include "agent.hh"
#include "pathfinding_pacman_agent.hh"
#include "input_pacman_agent.hh"
#include "random_pacman_agent.hh"
#include "rl_pacman_agent.hh"
#include "nn_pacman_agent.hh"

struct Statistics {
    bool won;
    double completion;

    Statistics(uint won, double completion) : won(won), completion(completion) {}
};

string id() {
    stringstream ss;
    ss << time(0) << rand()%100;
    return ss.str();
}

int main(int argc, char* argv[]) {
    Direction::LIST = vector<Direction>(4);
    Direction::LIST[0] = Direction::LEFT;
    Direction::LIST[1] = Direction::RIGHT;
    Direction::LIST[2] = Direction::UP;
    Direction::LIST[3] = Direction::DOWN;

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
    queue<Statistics> Q;

    for (int i = 0; i < Arguments::plays; ++i) {
        game.play();

        double completion = game.completion();
        Q.push(Statistics(game.result.won, completion));

        total_won_always += game.result.won;
        total_won += game.result.won;
        total_completion += completion;

        if (i >= Arguments::logging_statistics_precision) {
            Statistics r = Q.front();
            Q.pop();
            total_won -= r.won;
            total_completion -= r.completion;
        }

        if (i + 1 >= Arguments::logging_statistics_precision and i%Arguments::logging_update_rate == Arguments::logging_update_rate - 1) {
            cout << "\rWins (last " << Arguments::logging_statistics_precision << "): " << 100*total_won/double(Arguments::logging_statistics_precision) << '%'
                 << " (" << total_won_always << "/" << i + 1 << ") ::: "
                 << 100*total_completion/double(Arguments::logging_statistics_precision)
                 << "% completion (last " << Arguments::logging_statistics_precision << ")";
            cout.flush();
        }

        game.reset();
    }

    if (Arguments::pacman_ai_agent == RL) {
        cout << endl;

        RL_Pacman_Agent* agent = ((RL_Pacman_Agent*)(pacman_ai));
        string nn_path = "../data/nn" + id() + ".txt";
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