//
// Created by Albert Segarra Roca on 9/12/16.
//

#ifndef PATHFINDING_HH
#define PATHFINDING_HH

#include <vector>
#include <cstring>
#include <queue>
#include <stack>
#include <unordered_map>
#include <iomanip>

#include "direction.hh"
#include "position.hh"

using namespace std;

struct NoDistPathStep {
    Position pos;
    Direction initial;

    NoDistPathStep() {}
    NoDistPathStep(const Position& pos, const Direction& initial) : pos(pos), initial(initial) {}
};

struct PathStep {
    Position pos;
    Direction initial;
    int dist;

    PathStep() {}
    PathStep(const Position& pos, const Direction& initial, int dist) : pos(pos), initial(initial), dist(dist) {}
};

struct PathStepMagic {
    Position pos;
    char dir;
    int dist;
    int n_intersections;

    PathStepMagic() {}
    PathStepMagic(const Position& pos, char dir, int dist, int n_intersections)
            : pos(pos), dir(dir), dist(dist), n_intersections(n_intersections) {}
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
    int dist;

    PathResult() {}
    PathResult(bool found) : found(found) {}
    PathResult(const Direction& dir, int dist) : dir(dir), found(true), dist(dist) {}
};

struct IntersectionPathStep {
    Position pos;
    int previous_intersection;

    IntersectionPathStep() {}
    IntersectionPathStep(const Position& pos, int previous_intersection) :
            pos(pos), previous_intersection(previous_intersection) {}
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
    static vector<vector<char>> valid_dirs; // List of valid directions for each position
    static vector<vector<int>> enclosing_intersections; // List of enclosing intersections for each position
    static vector<vector<int>> n_intersections; // Number of intersections that must be crossed in order to get from i to j
    static vector<int> intersections; // Vector of intersections
    static vector<vector<int>> intersections_graph; // Intersections graph
    static vector<vector<int>> distances; // Distance from i to j
    static vector<vector<char>> direction_from_to; // Direction of the shortest path from i to j
    static vector<vector<int>> index_from_pos; // Index in the vector of valid positions for a given position

    static inline int id(const Position& p) { return index_from_pos[p.i][p.j]; }

    static inline Direction from_to(const Position& from, const Position& to) {
        int id_from = id(from);
        int id_to = id(to);

        if (id_from == id_to) return Direction::STAY;

        return Direction::LIST[(int)direction_from_to[id_from][id_to]];
    }

    static inline int dist(const Position&from, const Position& to) {
        return from.i == -1 or to.i == -1 ? MAXINT : distances[id(from)][id(to)];
    }

    static void compute_valid_dirs_intersections(const State &s) {
        valid_dirs = vector<vector<char>>(s.valid_positions.size());
        for (uint i = 0; i < s.valid_positions.size(); ++i) {
            for (int j = 0; j < Direction::NUMBER; ++j)
                if (s.valid_to_move(s.valid_positions[i].move_destination(Direction::LIST[j])))
                    valid_dirs[i].push_back(j);

            if (valid_dirs[i].size() > 2) intersections.push_back(i);
        }
    }

    template<typename T>
    static void add_to_set(vector<T>& s, T x) {
        for (const T& y : s)
            if (x == y) return;

        s.push_back(x);
    }

    static vector<char>& dirs(const Position& pos) {
        return valid_dirs[index_from_pos[pos.i][pos.j]];
    }

    static void compute_paths(const State& s) { // TODO: Implement other things
        s.max_dist = 1;
        int n = s.valid_positions.size();
        direction_from_to = vector<vector<char>>(n, vector<char>(n));
        distances = vector<vector<int>>(n, vector<int>(n));
        n_intersections = vector<vector<int>>(n, vector<int>(n));


        for (int i = 0; i < n; ++i) {
            distances[i][i] = 0;
            n_intersections[i][i] = 0;
            const Position& start = s.valid_positions[i];

            queue<PathStepMagic> Q;
            SeenMatrix::reset();
            vector<vector<bool>>& S = SeenMatrix::S;
            S[start.i][start.j] = true;

            for (char j : dirs(start)) {
                Position dest = start.move_destination(Direction::LIST[(int)j]);

                Q.push(PathStepMagic(dest, j, 1, is_intersection(dest)));
                S[dest.i][dest.j] = true;
            }

            while (not Q.empty()) {
                PathStepMagic current = Q.front(); Q.pop();
                int index_from = index_from_pos[current.pos.i][current.pos.j];
                distances[i][index_from] = current.dist;
                n_intersections[i][index_from] = current.n_intersections;

                s.max_dist = max(s.max_dist, current.dist);
                direction_from_to[i][index_from_pos[current.pos.i][current.pos.j]] = current.dir;

                for (char j : dirs(current.pos)) {
                    Position dest = current.pos.move_destination(Direction::LIST[(int) j]);

                    if (not S[dest.i][dest.j]) {
                        Q.push(PathStepMagic(dest, current.dir, current.dist + 1, current.n_intersections + is_intersection(dest)));
                        S[dest.i][dest.j] = true;
        }   }   }   }
    }

    static void compute_intersections_graph(const State &s) {
        intersections_graph = vector<vector<int>>(s.valid_positions.size());

        for (uint i = 0; i < intersections.size(); ++i) {
            for (uint j = i + 1; j < intersections.size(); ++j) {
                int interi = intersections[i];
                int interj = intersections[j];
                if (n_intersections[interi][interj] == 1) {
                    intersections_graph[interi].push_back(interj);
                    intersections_graph[interj].push_back(interi);
                }
            }
        }
    }

    // Recorda comprovar si l'origen es interseccio alhora de calcular les neighboring intersections (cal comptar l'origen com a interseccio tambe
    // Recorda comprovar si el desti es interseccio alhora de calcular el nombre d'interseccions a creuar per arribar a un ghost (s'ha de restar)

    static void compute_enclosing_intersections(const State& s) {
        enclosing_intersections = vector<vector<int>>(s.valid_positions.size());

        for (uint i = 0; i < s.valid_positions.size(); ++i) {
            if (is_intersection(s.valid_positions[i])) enclosing_intersections[i].push_back(i);
            for (int intersection : intersections)
                if (n_intersections[i][intersection] == 1) enclosing_intersections[i].push_back(intersection);
        }
    }

    static void compute(State& s) {
        compute_valid_dirs_intersections(s);
        compute_paths(s);
        compute_intersections_graph(s);
        compute_enclosing_intersections(s);
        //debug(s);
    }

    static void debug(const State& s) {
        int n = s.valid_positions.size();

        for (uint i = 0; i < index_from_pos.size(); ++i) {
            for (uint j = 0; j < index_from_pos.size(); ++j) {
                Position p(i, j);
                if (s.valid_to_move(p))
                    cout << setw(3) << index_from_pos[i][j] << ' ';
                else
                    cout << "XXX ";

            }
            cout << endl;
        }

        cout << endl << "Enclosing intersections:" << endl;

        for (int i = 0; i < n; ++i) {
            cout << i << ": ";
            for (int x : enclosing_intersections[i]) cout << x << ' ';
            cout << endl;
        }

        cout << endl << "Number of intersections to go from i to j" << endl;

        for (int i = 0; i < n; ++i) {
            cout << "From " << i << ": ";
            for (int j = 0; j < n; ++j) {
                cout << n_intersections[i][j] << ' ';
            }
            cout << endl;
        }

        cout << endl << "Intersections: ";
        for (int x : intersections) cout << x << ' ';

        cout << endl << "Intersections graph:" << endl;
        for (int i = 0; i < n; ++i) {
            if (intersections_graph[i].size() > 0) {
                cout << "Intersection " << i << ": ";
                for (uint j = 0; j < intersections_graph[i].size(); ++j)
                    cout << intersections_graph[i][j] << ' ';
                cout << endl;
            }
        }

        cout << "Distances:" << endl;
        for (int i = 0; i < n; ++i) {
            cout << "From " << i << ": ";
            for (int j = 0; j < n; ++j)
                cout << distances[i][j] << ' ';
            cout << endl;
        }
    }

    static bool is_intersection(const Position& pos) {
        return valid_dirs[index_from_pos[pos.i][pos.j]].size() > 2;
    }
};

vector<vector<char>> PathMagic::direction_from_to;
vector<vector<int>> PathMagic::index_from_pos;
vector<vector<char>> PathMagic::valid_dirs;
vector<vector<int>> PathMagic::enclosing_intersections;
vector<vector<int>> PathMagic::n_intersections;
vector<vector<int>> PathMagic::distances;
vector<int> PathMagic::intersections;
vector<vector<int>> PathMagic::intersections_graph;

#endif //SRC_PATHFINDING_HH
