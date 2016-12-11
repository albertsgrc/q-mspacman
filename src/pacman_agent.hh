#ifndef PACMAN_AGENT_HH
#define PACMAN_AGENT_HH

#include "agent.hh"
#include "direction.hh"
#include "bfs.hh"
#include "wsp.hh"
#include <cmath>

class Pacman_Agent: public Agent {
    // In this case ghost_id can be ignored
    Direction take_action(const State& s, uint ghost_id) { 
        if (s.n_rounds_powerpill > 0) {
            for (const Ghost_State& ghost : s.ghosts) {
                if (ghost.maybe_scared)
                    return bfs(s.pacman.pos,
                               [s](const Position& pos) { return s.scared_ghost_in_position(pos); },
                               s).dir;
            }

            return bfs(s.pacman.pos,
                        [s](const Position& pos) { return s.has_any_pill(pos); },
                       s).dir;
        }
        else {
            if (s.distance_closest_harmful_ghost(s.pacman.pos) < 10) {
                return wsp(s.pacman.pos,
                           [s](const Position& pos) { return s.has_any_pill(pos); },
                           s,
                           [s](const Position& pos) { return 100*s.ghosts_in_position(pos).size(); }
                       ).dir;
            }
            else return bfs(s.pacman.pos,
                     [s](const Position& pos) { return s.has_any_pill(pos); },
                     s).dir;
        }
    }
};

#endif
