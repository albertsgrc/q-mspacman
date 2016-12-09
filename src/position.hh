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

    inline Position move_destination(const Direction& direction) const {
        return Position(i + direction.i, j + direction.j);
    }

    inline void operator=(const Position& position) {
        i = position.i; j = position.j;
    }

    inline bool operator==(const Position& pos) {
        return i == pos.i and j == pos.j;
    }

    inline bool operator!=(const Position& pos) {
        return not operator==(pos);
    }

    inline int manhattan(const Position& p) const {
        return abs(p.i - i) + abs(p.j - j);
    }

    friend ostream& operator<<(ostream& s, const Position& p) {
        s << p.i << ' ' << p.j;
        return s;
	}
};

inline bool operator!=(const Position& a, const Position& b) {
    return a.i != b.i or a.j != b.j;
}

inline bool operator==(const Position& a, const Position& b) {
    return not (a != b);
}


#endif
