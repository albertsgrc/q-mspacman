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

    game.play();
}