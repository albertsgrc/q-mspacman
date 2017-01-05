#ifndef PATHFINDING_PACMAN_AGENT_HH
#define PATHFINDING_PACMAN_AGENT_HH

#include "../agent.hh"
#include "../../state/direction.hh"
#include "../../pathfinding/bfs.hh"
#include "../../pathfinding/wsp.hh"
#include <cmath>

class Pathfinding_Pacman_Agent: public Agent {

    // In this case ghost_id can be ignored
    inline Direction take_action(const State& s, uint ghost_id) {
        if (s.distance_closest_harmful_ghost(s.pacman.pos) < 10) {
            return wsp(s.pacman.pos,
                       [&s](const Position& pos) { return s.has_any_pill(pos); },
                       s,
                       [&s](const Position& pos) { return 10*pow(2, 10 - min(10,s.distance_closest_harmful_ghost(pos))); }
                   ).dir;
        }
        else return bfs(s.pacman.pos,
                 [&s](const Position& pos) { return s.has_any_pill(pos); },
                 s).dir;
    }
};

#endif
