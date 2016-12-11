#ifndef GHOST_AGENT_HH
#define GHOST_AGENT_HH

#include "direction.hh"
#include "agent.hh"
#include "utils.hh"
#include "best_first.hh"
#include "position.hh"

class Ghost_Agent: public Agent {

    bool was_intersection;

public:

    Ghost_Agent() : was_intersection(false) {}

    Direction take_action(const State& s, uint ghost_id) {
        const Ghost_State& ghost = s.ghosts[ghost_id];

        if (s.is_scared(ghost)) {
            return s.try_to_avoid(ghost.pos, best_first(ghost.pos, s.pacman.pos, s).dir);
        }
        else {
            PathResult pr;

            switch(ghost.behaviour) {
                case SCATTER:
                    pr = best_first(ghost.pos, ghost.scatter_pos, s); break;
                case CHASE:
                    pr = best_first(ghost.pos, s.pacman.pos, s); break;
                default:
                    ensure(false, "Invalid ghost behaviour enum");
            }

            ensure(pr.found, "Path not found!");
            if (pr.dir == Direction::STAY) __log("No movement for ghost #%d!", ghost_id);
            return pr.dir;
        }
    }
};

#endif
