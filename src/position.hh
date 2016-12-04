#ifndef POSITIONAL_HH
#define POSITIONAL_HH

#include <iostream>

#include "direction.hh"

using namespace std;

struct Position {
    int i, j;

    Position() : i(-1), j(-1) {}
    Position(int i, int j) : i(i), j(j) {}

    inline void move(const Direction& direction) {
        i += direction.i; j += direction.j;
    }

    inline Position move_destination(const Direction& direction) {
        return Position(i + direction.i, j + direction.j);
    }

    inline void operator=(const Position& position) {
        i = position.i; j = position.j;
    }

    inline bool operator==(const Position& pos) {
        return i == pos.i and j == pos.j;
    }

    friend ostream& operator<<(ostream& s, const Position& p) {
        s << p.i << ' ' << p.j;
        return s;
	}
};


#endif
