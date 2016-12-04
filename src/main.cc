#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "arguments.hh"
#include "game.hh"
#include "agent.hh"
#include "pacman_agent.hh"


void printResult(const GameResult& result) {

}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    Arguments::init(argc, argv);

    Agent* pacmanAI = new Pacman_Agent();

    Game game(pacmanAI);

    game.load_maze();

    GameResult result = game.play();

    printResult(result);
}
