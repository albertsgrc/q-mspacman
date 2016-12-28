//
// Created by Albert Segarra Roca on 9/12/16.
//

#ifndef BFS_HH
#define BFS_HH

#include <functional>
#include <vector>
#include <queue>

#include "direction.hh"
#include "position.hh"
#include "state.hh"
#include "pathfinding.hh"
using namespace std;

PathResult bfs(const Position& start, function<bool(const Position&)> reached_end, const State& state) {
    if (reached_end(start)) return PathResult(Direction::STAY, 0);

    queue<NoDistPathStep> Q;
    SeenMatrix::reset();
    vector<vector<bool>>& S = SeenMatrix::S;
    S[start.i][start.j] = true;

    for (const Direction& dir : Direction::LIST) {
        Position dest = start.move_destination(dir);

        if (state.valid_to_move(dest)) {
            Q.push(NoDistPathStep(dest, dir));
            S[dest.i][dest.j] = true;
    }   }

    while (not Q.empty() and not reached_end(Q.front().pos)) {
        NoDistPathStep current = Q.front(); Q.pop();

        for (const Direction& dir : Direction::LIST) {
            Position dest = current.pos.move_destination(dir);

            if (not S[dest.i][dest.j] and state.valid_to_move(dest)) {
                Q.push(NoDistPathStep(dest, current.initial));
                S[dest.i][dest.j] = true;
    }   }   }

    return Q.empty() ? PathResult(false) : PathResult(Q.front().initial, -1);
}


#endif //SRC_BFS_HH
