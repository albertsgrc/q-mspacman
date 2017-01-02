//
// Created by Albert Segarra Roca on 10/12/16.
//

#ifndef WSP_HH
#define WSP_HH

#include <functional>
#include <queue>
#include <vector>

#include "../state/direction.hh"
#include "pathfinding.hh"
#include "../state/state.hh"
#include "../state/position.hh"

PathResult wsp(const Position& start,
                          function<bool(Position)> reached_end,
                          const State& state,
                          function<int(Position)> cell_cost) {

    if (reached_end(start)) return PathResult(Direction::STAY, 0);

    auto comp = [](const CostPathStep& a, const CostPathStep& b) {
        return a.cost > b.cost;
    };

    priority_queue<CostPathStep, vector<CostPathStep>, decltype(comp)> Q(comp);
    SeenMatrix::reset();
    vector<vector<bool>>& S = SeenMatrix::S;
    vector<vector<int>> C(state.maze.rows(), vector<int>(state.maze.cols(), MAXINT));

    S[start.i][start.j] = true;

    for (char d : PathMagic::dirs(start)) {
        const Direction& dir = Direction::LIST[(int)d];
        Position s = start.move_destination(dir);

        C[s.i][s.j] = cell_cost(s);
        Q.push(CostPathStep(s, dir, C[s.i][s.j]));
    }

    while (not Q.empty() and not reached_end(Q.top().pos)) {
        CostPathStep a = Q.top(); Q.pop();
        if (not S[a.pos.i][a.pos.j]) {
            S[a.pos.i][a.pos.j] = true;
            for (char d : PathMagic::dirs(a.pos)) {
                const Direction& dir = Direction::LIST[(int)d];
                Position s = a.pos.move_destination(dir);
                int c = cell_cost(s) + 1;
                if (C[a.pos.i][a.pos.j] + c < C[s.i][s.j]) {
                    C[s.i][s.j] = C[a.pos.i][a.pos.j] + c;
                    Q.push(CostPathStep(s, a.initial, C[s.i][s.j]));
    }   }   }   }

    return Q.empty() ? PathResult(false) : PathResult(Q.top().initial, 0);
}

#endif //SRC_WSP_HH
