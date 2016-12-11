#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "arguments.hh"
#include "game.hh"
#include "agent.hh"
#include "pacman_agent.hh"
#include "input_agent.hh"

int main(int argc, char* argv[]) {
    srand(time(NULL));

    Arguments::init(argc, argv);
    Arguments::postprocess();

    Agent* pacman_ai = new Input_Agent();

    Game game(pacman_ai);

    game.load_maze();

    game.play();
}