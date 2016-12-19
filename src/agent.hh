#ifndef AGENT_HH
#define AGENT_HH

#include "arguments.hh"
#include "direction.hh"
#include "state.hh"

class Agent {
public:
    virtual Direction take_action(const State& s, uint ghost_id) = 0;
    virtual inline void notify_game_result(bool won) {};
    virtual inline void notify_eaten_pill() {};
    virtual inline void notify_eaten_powerpill() {}
    virtual inline void notify_killed_ghost() {}
    virtual inline void notify_reverse_direction() {}
};

#endif
