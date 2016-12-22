#ifndef DIRECTION_HH
#define DIRECTION_HH

#include "utils.hh"

class Direction {
public:
    int i, j;

    static Direction STAY, LEFT, RIGHT, UP, DOWN;
    const static int NUMBER = 4;
    static Direction LIST[NUMBER];

    Direction() {}

    Direction(int i, int j) : i(i), j(j) {
        ensure(between(abs(i) + abs(j), 0, 1), "Incorrect direction %d %d", i, j);
    }

    inline void operator=(const Direction& d) {
        i = d.i; j = d.j;
    }

    inline Direction opposite() const {
        return Direction(-this->i, -this->j);
    }

    inline bool isOpposite(const Direction& o) const {
        return o.i == -i and o.j == -j;
    }

    static inline Direction random() {
        return LIST[randint(NUMBER)];
    }
};

inline bool operator!=(const Direction& a, const Direction& b) {
    return a.i != b.i or a.j != b.j;
}

inline bool operator==(const Direction& a, const Direction& b) {
    return not (a != b);
}

Direction Direction::STAY = Direction(0, 0);

Direction Direction::LEFT = Direction(0, -1);
Direction Direction::RIGHT = Direction(0, +1);
Direction Direction::UP = Direction(-1, 0);
Direction Direction::DOWN = Direction(+1, 0);

Direction Direction::LIST[4] =
    { Direction::LEFT, Direction::RIGHT, Direction::UP, Direction::DOWN };

#endif
