//
// Created by Albert Segarra Roca on 16/12/16.
//

#ifndef SRC_SILLY_AGENT_HH
#define SRC_SILLY_AGENT_HH

#include "agent.hh"
#include "direction.hh"
#include "bfs.hh"
#include "wsp.hh"
#include <cmath>

class Random_Pacman_Agent: public Agent {
    Position previous;

    // In this case ghost_id can be ignored
    Direction take_action(const State& s, uint ghost_id) {
        if (s.pacman.pos != previous) {
            previous = s.pacman.pos;
            return s.random_valid_dir(s.pacman.pos);
        }
        else return s.pacman.dir;
    }
};

#endif //SRC_SILLY_AGENT_HH
