#ifndef GHOST_STATE_HH
#define GHOST_STATE_HH

#include "position.hh"
#include "agent_state.hh"

class Ghost_State: public Agent_State {
public:
    Position initial_pos; // To know where to respawn when dead

    bool scared; // Could be true and still not scared if state.n_rounds_power_pill = 0

    // Number of rounds left until ghost revives. 0 if alive
    int n_rounds_revive;

    Ghost_State(const Position& pos, const Direction& dir) :
        Agent_State(pos, dir), initial_pos(pos), scared(false), n_rounds_revive(0) {}

    void operator=(const Ghost_State& o) {
        Agent_State::operator=(o);
        initial_pos = o.initial_pos;
        scared = o.scared;
        n_rounds_revive = o.n_rounds_revive;
    }
};

#endif
