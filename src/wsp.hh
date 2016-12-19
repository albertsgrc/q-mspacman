//
// Created by Albert Segarra Roca on 10/12/16.
//

#ifndef WSP_HH
#define WSP_HH

#include <functional>
#include <queue>
#include <vector>

#include "direction.hh"
#include "pathfinding.hh"
#include "state.hh"
#include "position.hh"

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

    for (const Direction& d : Direction::LIST) {
        Position s = start.move_destination(d);
        if (state.valid_to_move(s)) {
            C[s.i][s.j] = cell_cost(s);
            Q.push(CostPathStep(s, d, C[s.i][s.j]));
        }
    }

    while (not Q.empty() and not reached_end(Q.top().pos)) {
        CostPathStep a = Q.top(); Q.pop();
        if (not S[a.pos.i][a.pos.j]) {
            S[a.pos.i][a.pos.j] = true;
            for (const Direction&d : Direction::LIST) {
                Position s = a.pos.move_destination(d);
                int c = cell_cost(s) + 1;
                if (state.valid_to_move(s) and C[a.pos.i][a.pos.j] + c < C[s.i][s.j]) {
                    C[s.i][s.j] = C[a.pos.i][a.pos.j] + c;
                    Q.push(CostPathStep(s,
                                a.initial, C[s.i][s.j]));
                }
            }
        }
    }

    return Q.empty() ? PathResult(false) : PathResult(Q.top().initial, 0);
}

#endif //SRC_WSP_HH
