#ifndef DIRECTION_HH
#define DIRECTION_HH

#include "utils.hh"

class Direction {
public:
    int i, j;

    static Direction LEFT, RIGHT, UP, DOWN;
    const static int NUMBER = 4;
    static Direction LIST[NUMBER];

    Direction() {}

    Direction(int i, int j) : i(i), j(j) {
        assert(between(abs(i) + abs(j), 0, 1));
    }

    inline void operator=(const Direction& d) {
        i = d.i; j = d.j;
    }

    inline Direction opposite() {
        return Direction(-this->i, -this->j);
    }


    inline bool operator==(const Direction& d) {
        return i == d.i and j == d.j;
    }

    static inline Direction random() {
        return LIST[randint(NUMBER)];
    }
};

Direction Direction::LEFT = Direction(0, -1);
Direction Direction::RIGHT = Direction(0, +1);
Direction Direction::UP = Direction(-1, 0);
Direction Direction::DOWN = Direction(+1, 0);

Direction Direction::LIST[4] =
    { Direction::LEFT, Direction::RIGHT, Direction::UP, Direction::DOWN };

#endif
