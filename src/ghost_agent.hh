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
            Direction pacman_dir = AStar(ghost.pos, s.pacman.pos, s).dir;
            Direction opposite = pacman_dir.opposite();
            if (s.valid_to_move(ghost.pos.move_destination((opposite))))
                return opposite;
            else {
                for (const Direction& d : Direction::LIST) {
                    if (d != opposite and d != pacman_dir and s.valid_to_move(ghost.pos.move_destination(d))) {
                        return d;
                    }
                }

                return pacman_dir;
            }
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
