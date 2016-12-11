//
// Created by Albert Segarra Roca on 5/12/16.
//

#ifndef PATHFINDING_A_STAR_HH
#define PATHFINDING_A_STAR_HH

#include <functional>
#include <vector>
#include <queue>

#include "direction.hh"
#include "position.hh"
#include "state.hh"
#include "pathfinding.hh"
using namespace std;

PathResult a_star(Position start, Position end, const State &state) {
    if (start == end) return PathResult(Direction::STAY);

    auto comp = [end](const PathStep& a, const PathStep& b) {
        return a.pos.manhattan(end) > b.pos.manhattan(end);
    };

    priority_queue<PathStep, vector<PathStep>, decltype(comp)> Q(comp);
    vector<vector<bool>> S(state.maze.rows(), vector<bool>(state.maze.cols(), false));

    for (const Direction& dir : Direction::LIST) {
        Position dest = start.move_destination(dir);

        if (state.valid_to_move(dest)) {
            Q.push(PathStep(dest, dir));
            S[dest.i][dest.j] = true;
    }   }

    while (not Q.empty() and Q.top().pos != end) {
        PathStep current = Q.top(); Q.pop();

        for (const Direction& dir : Direction::LIST) {
            Position dest = current.pos.move_destination(dir);

            if (not S[dest.i][dest.j] and state.valid_to_move(dest)) {
                Q.push(PathStep(dest, current.initial));
                S[dest.i][dest.j] = true;
    }   }   }

    return Q.empty() ? PathResult(false) : PathResult(Q.top().initial);
}

#endif //PATHFINDING_A_STAR_HH
