//
// Created by Albert Segarra Roca on 5/12/16.
//

#ifndef PATHFINDING_BEST_FIRST_HH
#define PATHFINDING_BEST_FIRST_HH

#include <functional>
#include <vector>
#include <queue>

#include "direction.hh"
#include "position.hh"
#include "state.hh"
#include "pathfinding.hh"
using namespace std;

PathResult best_first(Position start, Position end, const State &state) {
    if (start == end) return PathResult(Direction::STAY, 0);

    auto comp = [end](const PathStep& a, const PathStep& b) {
        return a.pos.manhattan(end) > b.pos.manhattan(end);
    };

    priority_queue<PathStep, vector<PathStep>, decltype(comp)> Q(comp);
    SeenMatrix::reset();
    vector<vector<bool>>& S = SeenMatrix::S;

    for (const Direction& dir : Direction::LIST) {
        Position dest = start.move_destination(dir);

        if (state.valid_to_move(dest)) {
            Q.push(PathStep(dest, dir, 1));
            S[dest.i][dest.j] = true;
    }   }

    while (not Q.empty() and Q.top().pos != end) {
        PathStep current = Q.top(); Q.pop();

        for (const Direction& dir : Direction::LIST) {
            Position dest = current.pos.move_destination(dir);

            if (not S[dest.i][dest.j] and state.valid_to_move(dest)) {
                Q.push(PathStep(dest, current.initial, current.dist + 1));
                S[dest.i][dest.j] = true;
    }   }   }

    return Q.empty() ? PathResult(false) : PathResult(Q.top().initial, Q.top().dist);
}

#endif //PATHFINDING_BEST_FIRST_HH
