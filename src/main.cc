#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

#include "arguments.hh"
#include "game.hh"
#include "agent.hh"

using namespace std;

void printResult(const GameResult& result) {

}

int main(int argc, char* argv[]) {
    Arguments::init(argc, argv);

    Agent agent;
    Game game(agent);

    GameResult result = game.play();

    printResult(result);
}
