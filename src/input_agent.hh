//
// Created by Albert Segarra Roca on 11/12/16.
//

#ifndef INPUT_AGENT_HH
#define INPUT_AGENT_HH

#include "agent.hh"
#include "direction.hh"
#include "bfs.hh"
#include "wsp.hh"
#include <cmath>

class Input_Agent: public Agent {
    // In this case ghost_id can be ignored
    Direction take_action(const State& s, uint ghost_id) {
        r:
        char c;
        cin >> c;

        switch(c) {
            case 'a': return Direction::LEFT;
            case 'w': return Direction::UP;
            case 's': return Direction::DOWN;
            case 'd': return Direction::RIGHT;
            case 'x': return Direction::STAY;
            default: goto r;
        }
    }
};

#endif //SRC_INPUT_AGENT_HH_HH
