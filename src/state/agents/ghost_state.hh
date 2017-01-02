#ifndef GHOST_STATE_HH
#define GHOST_STATE_HH

#include "../position.hh"
#include "agent_state.hh"

enum Ghost_Behaviour {
    SCATTER, CHASE
};

class Ghost_State: public Agent_State {
public:
    Position initial_pos; // To know where to respawn when dead

    bool maybe_scared; // Could be true and still not scared if state.n_rounds_power_pill = 0

    // Number of rounds left until ghost revives. 0 if alive
    int n_rounds_revive;

    int scatter_cycle_rounds;
    int chase_cycle_rounds;

    Ghost_Behaviour behaviour;
    Position scatter_pos;
    int n_rounds_left_behaviour; // rounds left of current behaviour
                                 // will change in next round if 0
                                 // decreases anyway if ghost is scared or dead

    Ghost_State(const Position& pos, const Direction& dir) :
        Agent_State(pos, dir), initial_pos(pos), maybe_scared(false), n_rounds_revive(0),
        scatter_cycle_rounds(Arguments::initial_scatter_cycle_rounds),
        chase_cycle_rounds(Arguments::initial_chase_cycle_rounds),
        behaviour(CHASE), /* They will start with the opposite (SCATTER) */
        n_rounds_left_behaviour(0) {}

    void operator=(const Ghost_State& o) {
        Agent_State::operator=(o);
        initial_pos = o.initial_pos;
        maybe_scared = o.maybe_scared;
        n_rounds_revive = o.n_rounds_revive;
        behaviour = o.behaviour;
        scatter_pos = o.scatter_pos;
        n_rounds_left_behaviour = o.n_rounds_left_behaviour;
        scatter_cycle_rounds = o.scatter_cycle_rounds;
        chase_cycle_rounds = o.chase_cycle_rounds;
    }

    inline bool is_alive() const {
        return n_rounds_revive == 0;
    }
};

#endif
