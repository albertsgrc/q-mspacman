#ifndef STATE_HH
#define STATE_HH

#include "utils.hh"
#include "position.hh"
#include "ghost_state.hh"
#include "game.hh"

struct State {
    static const uchar PILL = '.';
    static const uchar POWER_PILL = 'o';
    static const uchar PACMAN = 'P';
    static const uchar GHOST = 'G';
    static const uchar FREE = ' ';
    static const uchar WALL = '%';

    // Contains the current state of the maze
    // NOTE THAT PACMAN AND GHOSTS ARE **NOT** HERE
    // Instead, their positions are encoded as a Position member
    // in their state instances
    Matrix<char> maze;

    // Contains a list of all valid positions in the board
    static vector<Position> valid_positions;
    static int total_pills;
    static int max_dist;
    static double max_dist_inverse;
    static double total_pills_inverse;

    uniform_int_distribution<> distribution_valid_pos;

    int round;

    // Number of power pill rounds left. 0 if no power pill in action
    int n_rounds_powerpill;

    int n_pills_left;

    int n_powerpills_left;


    vector<Ghost_State> ghosts;
    Agent_State pacman;

    State() :
        round(0), n_rounds_powerpill(0), n_pills_left(0),
        n_powerpills_left(0) {}

    inline void operator=(const State& o) {
        maze = o.maze;

        n_pills_left = o.n_pills_left;
        n_powerpills_left = o.n_powerpills_left;
        ghosts = o.ghosts;
        pacman = o.pacman;
        round = o.round;
        n_rounds_powerpill = o.n_rounds_powerpill;
    }

    friend ostream& operator<<(ostream& o, const State& s) {
        Matrix<char> maze_aux = s.maze;

        maze_aux[s.pacman.pos.i][s.pacman.pos.j] = 'P';

        for (const Ghost_State& ghost : s.ghosts)
            if (ghost.n_rounds_revive == 0)
                maze_aux[ghost.pos.i][ghost.pos.j] = s.is_scared(ghost) ? 'S' : 'G';

        o << "Round " << s.round << ":" << endl
          << maze_aux << endl
          << "Pills left: " << s.n_pills_left << endl
          << "Powerpills left: " << s.n_powerpills_left << endl
          << "Rounds powerpill: " << s.n_rounds_powerpill;

        return o;
    }

    inline bool is_wall(const Position& pos) const {
        return maze[pos.i][pos.j] == WALL;
    }

    inline bool inside_boundaries(const Position& pos)const {
        return between(pos.i, 0, maze.rows()-1) and between(pos.j, 0, maze.cols()-1);
    }

    inline vector<int> ghosts_in_position(const Position& pos) const {
        vector<int> ghosts_pos;

        for (uint i = 0; i < ghosts.size(); ++i) {
            if (ghosts[i].pos.i == pos.i and ghosts[i].pos.j == pos.j)
                ghosts_pos.push_back(i);
        }

        return ghosts_pos;
    }

    inline bool dangerous_ghost_in_position(const Position& pos) const {
        for (uint i = 0; i < ghosts.size(); ++i) {
            if (ghosts[i].pos.i == pos.i and ghosts[i].pos.j == pos.j and
                    (n_rounds_powerpill == 0 or not ghosts[i].maybe_scared))
                return true;
        }

        return false;
    }

    inline bool scared_ghost_in_position(const Position& pos) const {
        if (n_rounds_powerpill == 0) return false;

        for (const Ghost_State& ghost : ghosts) {
            if (ghost.pos.i == pos.i and ghost.pos.j == pos.j
                and ghost.maybe_scared) return true;
        }

        return false;
    }

    inline int distance_closest_harmful_ghost(const Position& pos) const {
        int distance = MAXINT;

        for (const Ghost_State& ghost : ghosts) {
            if (ghost.is_alive() and not is_scared(ghost)) {
                distance = min(distance, ghost.pos.manhattan(pos));
            }
        }

        return distance;
    }

    inline Direction try_to_avoid(const Position& p, const Direction& bad_dir) const {
        Direction opposite = bad_dir.opposite();

        if (valid_to_move(p.move_destination((opposite))))
            return opposite;
        else {
            for (const Direction& d : Direction::LIST) {
                if (d != opposite and d != bad_dir and valid_to_move(p.move_destination(d))) {
                    return d;
                }
            }

            return bad_dir;
        }
    }

    inline bool valid_to_move(const Position& pos) const {
        return maze[pos.i][pos.j] != WALL;
    }

    inline bool has_pill(const Position& pos) const {
        return maze[pos.i][pos.j] == PILL;
    }

    inline bool has_powerpill(const Position& pos) const {
        return maze[pos.i][pos.j] == POWER_PILL;
    }

    inline bool has_any_pill(const Position& pos) const {
        return has_pill(pos) or has_powerpill(pos);
    }

    inline vector<Direction> valid_dirs(const Position& pos) const { // TODO: Precompute this in the beggining and store in matrix
        vector<Direction> valid_dirs;

        for (const Direction& d : Direction::LIST) {
            Position dest = pos.move_destination(d);

            if (valid_to_move(dest))
                valid_dirs.push_back(d);
        }

        return valid_dirs;
    }

    inline bool is_intersection(const Position& pos) const {
        int count = 0;

        for (const Direction& d : Direction::LIST) {
            count += valid_to_move(pos.move_destination(d));
        }

        return count > 2;
    }

    inline Direction random_valid_dir(const Position& pos) const {
        vector<Direction> valid = valid_dirs(pos);

        uniform_int_distribution<> distr(0, int(valid.size()) - 1);

        return valid[distr(Arguments::random_generator)];
    }

    inline Position random_valid_pos() {
        return valid_positions[distribution_valid_pos(Arguments::random_generator)];
    }

    inline bool is_scared(const Ghost_State& ghost) const {
        return n_rounds_powerpill > 0 and ghost.maybe_scared;
    }
};

int State::total_pills;
int State::max_dist;
double State::max_dist_inverse;
double State::total_pills_inverse;
vector<Position> State::valid_positions;

#endif
