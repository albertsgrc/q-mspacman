//
// Created by Albert Segarra Roca on 16/12/16.
//

#ifndef SRC_SILLY_AGENT_HH
#define SRC_SILLY_AGENT_HH

#include "../agent.hh"
#include "../../state/direction.hh"
#include "../../pathfinding/bfs.hh"
#include "../../pathfinding/wsp.hh"
#include <cmath>

class Random_Pacman_Agent: public Agent {

    // In this case ghost_id can be ignored
    Direction take_action(const State& s, uint ghost_id) {
        if (s.pacman.pos != s.pacman.prev) return PathMagic::random_valid_dir(s.pacman.pos);
        else return s.pacman.dir;
    }
};

#endif //SRC_SILLY_AGENT_HH
