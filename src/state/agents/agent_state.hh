#ifndef AGENT_STATE_HH
#define AGENT_STATE_HH

#include "../position.hh"
#include "../direction.hh"

class Agent_State {
public:
    Position pos;
    Position prev;
    Direction dir;
    float step;

    Agent_State() {}
    Agent_State(const Position& pos, const Direction& dir) : pos(pos), dir(dir), step(0) {}

    void operator=(const Agent_State& o) {
        pos = o.pos;
        prev = o.prev;
        dir = o.dir;
        step = o.step;
    }
};

#endif
