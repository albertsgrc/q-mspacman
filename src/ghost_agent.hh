#ifndef GHOST_AGENT_HH
#define GHOST_AGENT_HH

#include "direction.hh"
#include "agent.hh"
#include "utils.hh"
#include "a_star.hh"
#include "position.hh"

class Ghost_Agent: public Agent {

    bool was_intersection;

public:

    Ghost_Agent() : was_intersection(false) {}

    Direction take_action(const State& s, uint ghost_id) {
        const Ghost_State& ghost = s.ghosts[ghost_id];

        if (ghost.scared and s.n_rounds_powerpill > 0) {
            return AStar(ghost.pos, s.pacman.pos, s).dir.opposite();
        }
        else {
            switch(ghost.behaviour) {
                case SCATTER:
                    return AStar(ghost.pos, ghost.scatter_pos, s).dir;
                case CHASE:
                    return AStar(ghost.pos, s.pacman.pos, s).dir;
                default:
                    ensure(false, "Invalid ghost behaviour enum");
            }
        }
    }
};

#endif
