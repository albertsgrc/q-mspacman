#ifndef GHOST_AGENT_HH
#define GHOST_AGENT_HH

#include "direction.hh"
#include "agent.hh"
#include "utils.hh"
#include "best_first.hh"
#include "position.hh"
#include "pathfinding.hh"

class Ghost_Agent: public Agent {

public:

    virtual ~Ghost_Agent() {}

    Direction take_action(const State& s, uint ghost_id) {
        const Ghost_State& ghost = s.ghosts[ghost_id];

        if (ghost.pos != ghost.prev) {
            if (s.is_scared(ghost)) {
                return s.try_to_avoid(ghost.pos, PathMagic::from_to(ghost.pos, s.pacman.pos));
            }
            else {
                Direction d;

                switch(ghost.behaviour) {
                    case SCATTER:
                        d = PathMagic::from_to(ghost.pos, ghost.scatter_pos); break;
                    case CHASE:
                        d = PathMagic::from_to(ghost.pos, s.pacman.pos); break;
                    default:
                    ensure(false, "Invalid ghost behaviour enum");
                }

                //ensure(pr.found, "Path not found!");
                if (d == Direction::STAY) __log("No movement for ghost #%d!", ghost_id);
                return d;
            }
        }
        else return ghost.dir;
    }
};

#endif
