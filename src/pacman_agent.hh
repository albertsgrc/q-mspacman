#ifndef PACMAN_AGENT_HH
#define PACMAN_AGENT_HH

#include "agent.hh"
#include "direction.hh"

class Pacman_Agent: public Agent {
    // In this case ghost_id can be ignored
    Direction take_action(const State& s, uint ghost_id) { return Direction::random(); }
};

#endif
