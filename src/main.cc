#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "arguments.hh"
#include "game.hh"
#include "agent.hh"
#include "pathfinding_pacman_agent.hh"
#include "input_pacman_agent.hh"
#include "random_pacman_agent.hh"
#include "rl_pacman_agent.hh"

int main(int argc, char* argv[]) {
    cout.setf(ios::fixed);
    cout.precision(2);

    Arguments::init(argc, argv);
    Arguments::postprocess();

    srand(Arguments::random_seed);

    Agent* pacman_ai;

    switch (Arguments::pacman_ai_agent) {
        case PATHFINDING: pacman_ai = new Pathfinding_Pacman_Agent(); break;
        case INPUT: pacman_ai = new Input_Pacman_Agent(); break;
        case RANDOM: pacman_ai = new Random_Pacman_Agent(); break;
        case RL: pacman_ai = new RL_Pacman_Agent(); break;
        default: ensure(false, "Invalid pacman AI Agent enum value");
    }

    Game game(pacman_ai);

    game.load_maze();

    Inputs::init(game.state);

    uint total_won = 0;
    uint pills_left = 0;
    double completion_mean = 0;
    double completion_m2 = 0;

    for (int i = 0; i < Arguments::plays; ++i) {
        int aux = -1;
        if (i == Arguments::plays - 1) {aux = Arguments::plays; Arguments::plays = 1;}
        game.play();
        if (aux > -1) Arguments::plays = aux;

        total_won += game.result.won;
        double completion = 1 - (game.state.n_normal_pills_left + game.state.n_powerpills_left)/double(game.state.total_pills);
        pills_left += game.state.n_normal_pills_left + game.state.n_powerpills_left;

        double completion_delta = completion - completion_mean;
        completion_mean += completion_delta/(i+1);
        completion_m2 += completion_delta*(completion - completion_mean);

        if (i%10 == 9) {
            cout << "\rWins: " << 100*total_won/double(i+1) << '%'
                 << " (" << total_won << "/" << i + 1 << ") ::: "
                 << 100*completion_mean << "% (" << sqrt(completion_m2/max(1, i)) << "sd) completion";
        }

        cout.flush();

        game.reset();
    }

    //if (Arguments::pacman_ai_agent == RL) ((RL_Pacman_Agent*)(pacman_ai))->nn.write_file("../data/neural.txt");

    cout << endl;
}