#ifndef AGENT_HH
#define AGENT_HH

#include "arguments.hh"
#include "direction.hh"
#include "state.hh"

class Agent {
public:
    virtual Direction take_action(const State& s, uint ghost_id) = 0;
};

#endif
