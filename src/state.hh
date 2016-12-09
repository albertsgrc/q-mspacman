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
    Matrix<char> maze;

    // Contains a list of all valid positions in the board
    vector<Position> valid_positions;

    // Number of power pill rounds left. 0 if no power pill in action
    int n_rounds_powerpill;

    int n_pills_left;

    int n_powerpills_left;

    int round;

    vector<Ghost_State> ghosts;
    Agent_State pacman;

    State() :
        round(0), n_rounds_powerpill(0), n_powerpills_left(0),
        n_pills_left(0) {}


    inline void operator=(const State& o) {
        maze = o.maze;
        n_rounds_powerpill = o.n_rounds_powerpill;
        n_pills_left = o.n_pills_left;
        n_powerpills_left = o.n_powerpills_left;
        ghosts = o.ghosts;
        pacman = o.pacman;
        valid_positions = o.valid_positions;
    }

    friend ostream& operator<<(ostream& o, const State& s) {
        Matrix<char> maze_aux = s.maze;

        maze_aux[s.pacman.pos.i][s.pacman.pos.j] = 'P';

        for (const Ghost_State& ghost : s.ghosts)
            if (ghost.n_rounds_revive == 0)
                maze_aux[ghost.pos.i][ghost.pos.j] = ghost.scared and s.n_rounds_powerpill > 0 ? 'S' : 'G';

        o << "Round " << s.round << ":" << endl
          << maze_aux << endl
          << "Pills left: " << s.n_pills_left << endl
          << "Powerpills left: " << s.n_powerpills_left << endl
          << "Rounds powerpill: " << s.n_rounds_powerpill;

        return o;
    }

    inline int ghost_in_position(const Position& pos) const {
        for (int i = 0; i < ghosts.size(); ++i) {
            if (ghosts[i].pos.i == pos.i and ghosts[i].pos.j == pos.j) return i;
        }

        return -1;
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

    inline bool is_intersection(const Position& pos) const {
        int count = 0;

        for (const Direction& d : Direction::LIST) {
            count += valid_to_move(pos.move_destination(d));
        }

        return count > 1;
    }

    inline Direction random_valid_dir(const Position& pos) const {
        vector<Direction> valid_dirs;

        for (const Direction& d : Direction::LIST) {
            Position dest = pos.move_destination(d);

            if (valid_to_move(dest))
                valid_dirs.push_back(d);

        }

        return valid_dirs[randint(valid_dirs.size())];
    }

    inline Position random_valid_pos() const {
        return valid_positions[randint(valid_positions.size())];
    }
};

#endif
