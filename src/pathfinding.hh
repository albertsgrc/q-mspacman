//
// Created by Albert Segarra Roca on 9/12/16.
//

#ifndef PATHFINDING_HH
#define PATHFINDING_HH

#include "direction.hh"
#include "position.hh"
#include "state.hh"

using namespace std;

struct PathStep {
    Position pos;
    Direction initial;

    PathStep() {}
    PathStep(const Position& pos, const Direction& initial) : pos(pos), initial(initial) {}
};

struct CostPathStep {
    Position pos;
    Direction initial;
    int cost;

    CostPathStep() {}
    CostPathStep(const Position& pos, const Direction& initial, int cost) :
            pos(pos), initial(initial), cost(cost) {}
};

struct PathResult {
    Direction dir;
    bool found;

    PathResult() {}
    PathResult(bool found) : found(found) {}
    PathResult(const Direction& dir) : dir(dir), found(true) {}
};

#endif //SRC_PATHFINDING_HH
