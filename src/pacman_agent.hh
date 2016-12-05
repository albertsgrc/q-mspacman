#ifndef PACMAN_AGENT_HH
#define PACMAN_AGENT_HH

#include "agent.hh"
#include "direction.hh"

class Pacman_Agent: public Agent {
    // In this case ghost_id can be ignored
    Direction take_action(const State& s, uint ghost_id) { 
        vector<Direction> valid_dirs;

        for (const Direction& d : Direction::LIST) {
            Position dest = s.pacman.pos.move_destination(d);

            if (s.ghost_in_position(dest) == -1 and s.valid_to_move(dest)) {
                if (s.has_any_pill(dest)) return d;
                else valid_dirs.push_back(d);
            }

        }

        return valid_dirs.size() > 0 ? valid_dirs[randint(valid_dirs.size())] : Direction::random();
    }
};

#endif
