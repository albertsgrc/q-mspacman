#ifndef GHOST_AGENT_HH
#define GHOST_AGENT_HH

#include "direction.hh"
#include "agent.hh"
#include "utils.hh"
#include "a_star.hh"

class Ghost_Agent: public Agent {

    bool was_intersection;

public:

    Ghost_Agent() : was_intersection(false) {}

    //TODO: Compte, s'ha de diferenciar el cas en el qu eacaba d'arribar
    //a un ainterseccio i encara no s'ha decidit, del que ja s'ha decidit
    // i te un step en una direccio dun tunel
    // es considera en aquest ultim cas que ja no esta en nuna interseccio sino al tunel
    // a efectes de canviar la direccio aleatoriament
    Direction take_action(const State& s, uint ghost_id) {
        /*if (s.is_intersection(s.ghosts[ghost_id].pos)) {
            Direction r = was_intersection ? s.ghosts[ghost_id].dir :
                                             s.random_valid_dir(s.ghosts[ghost_id].pos);

            this->was_intersection = true;
            return r;
        }
        else {
            this->was_intersection = false;
            return s.ghosts[ghost_id].dir;
        }*/

        return AStar(s.ghosts[ghost_id].pos, s.pacman.pos, s).dir;
    }
};

#endif
