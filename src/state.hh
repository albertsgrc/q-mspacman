#ifndef STATE_HH
#define STATE_HH

#include "utils.hh"
#include "position.hh"
#include "ghost_state.hh"

struct State {
    // Contains the current state of the maze
    Matrix<char> maze;

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
};

#endif
