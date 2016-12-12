//
// Created by Albert Segarra Roca on 9/12/16.
//

#ifndef PATHFINDING_HH
#define PATHFINDING_HH

#include <vector>
#include <cstring>
#include <queue>

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

struct PathStepMagic {
    Position pos;
    char dir;

    PathStepMagic() {}
    PathStepMagic(const Position& pos, char dir) : pos(pos), dir(dir) {}
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

// Used by the pathfinding algorithms so as not to allocate and deallocate
// the matrix in the stack on every pathfinding algorithm run (local variable)
// Instead, it is stored as a static struct member, which is allocated once
// in the heap
struct SeenMatrix {
    static vector<vector<bool>> S;

    static void init(int r, int c) {
        S = vector<vector<bool>>(r, vector<bool>(c, false));
    }

    static void reset() {
        for (auto &i : S)
            std::fill(i.begin(), i.end(), 0);
    }
};

vector<vector<bool>> SeenMatrix::S;

// Used to store and compute all-pairs shortest paths for memoization,
// which is more efficient than computing the path at every
// game round, specially if the game is played a lot of times like in
// reinforcement learning
struct PathMagic {
    static vector<vector<char>> direction_from_to;
    static vector<vector<int>> index_from_pos;

    static Direction from_to(const Position& from, const Position& to) {
        int id_from = index_from_pos[from.i][from.j];
        int id_to = index_from_pos[to.i][to.j];

        if (id_from == id_to) return Direction::STAY;

        return Direction::LIST[(int)direction_from_to[id_from][id_to]];
    }

    static void compute(const State& s) {
        int n = s.valid_positions.size();
        direction_from_to = vector<vector<char>>(n, vector<char>(n));

        for (int i = 0; i < n; ++i) {
            const Position& start = s.valid_positions[i];

            queue<PathStepMagic> Q;
            SeenMatrix::reset();
            vector<vector<bool>>& S = SeenMatrix::S;
            S[start.i][start.j] = true;

            for (int j = 0; j < Direction::NUMBER; ++j) {
                Position dest = start.move_destination(Direction::LIST[j]);

                if (s.valid_to_move(dest)) {
                    Q.push(PathStepMagic(dest, j));
                    S[dest.i][dest.j] = true;
            }   }

            while (not Q.empty()) {
                PathStepMagic current = Q.front(); Q.pop();
                direction_from_to[i][index_from_pos[current.pos.i][current.pos.j]] = current.dir;

                for (int j = 0; j < Direction::NUMBER; ++j) {
                    Position dest = current.pos.move_destination(Direction::LIST[j]);

                    if (not S[dest.i][dest.j] and s.valid_to_move(dest)) {
                        Q.push(PathStepMagic(dest, current.dir));
                        S[dest.i][dest.j] = true;
            }   }   }
        }
    }
};

vector<vector<char>> PathMagic::direction_from_to;
vector<vector<int>> PathMagic::index_from_pos;

#endif //SRC_PATHFINDING_HH
