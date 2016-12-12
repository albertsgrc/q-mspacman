#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "arguments.hh"
#include "game.hh"
#include "agent.hh"
#include "pathfinding_pacman_agent.hh"
#include "input_pacman_agent.hh"

int main(int argc, char* argv[]) {
    srand(time(NULL));

    Arguments::init(argc, argv);
    Arguments::postprocess();

    Agent* pacman_ai;

    switch (Arguments::pacman_ai_agent) {
        case PATHFINDING: pacman_ai = new Pathfinding_Pacman_Agent(); break;
        case INPUT: pacman_ai = new Input_Pacman_Agent(); break;
        default: ensure(false, "Invalid pacman AI Agent enum value");
    }

    Game game(pacman_ai);

    game.load_maze();

    uint won = 0;
    uint total_pills = game.state.n_pills_left + game.state.n_powerpills_left;
    uint pills_left = 0;

    for (int i = 0; i < Arguments::plays; ++i) {
        game.play();
        won += game.result.won;
        pills_left += game.state.n_pills_left + game.state.n_powerpills_left;
        cout << "Game " << i << '/' << Arguments::plays << ": " << (game.result.won ? "WINS" : "LOSES") << endl;
        game.reset();
    }

    cout << "Number of wins: " << won << endl
         << "Percentage of wins: " << 100*won/double(Arguments::plays) << '%' << endl
         << "Percentage of level completion: " << 100*(total_pills - pills_left/double(Arguments::plays))/total_pills << '%' << endl;
}