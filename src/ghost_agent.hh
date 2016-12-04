#ifndef GHOST_AGENT_HH
#define GHOST_AGENT_HH

#include "direction.hh"
#include "agent.hh"
#include "utils.hh"

class Ghost_Agent: public Agent {

    //TODO: Compte, s'ha de diferenciar el cas en el qu eacaba d'arribar
    //a un ainterseccio i encara no s'ha decidit, del que ja s'ha decidit
    // i te un step en una direccio dun tunel
    // es considera en aquest ultim cas que ja no esta en nuna interseccio sino al tunel
    // a efectes de canviar la direccio aleatoriament
    Direction take_action(const State& s, uint ghost_id) {
        return Direction::random();
    }
};

#endif
