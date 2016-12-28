//
// Created by Albert Segarra Roca on 26/12/16.
//

#ifndef SRC_INPUTS_HH
#define SRC_INPUTS_HH

#include <algorithm>

#include "direction.hh"
#include "state.hh"
#include "arguments.hh"
#include "pathfinding.hh"
#include "bfs.hh"

class Inputs {
public:
    static double total_rounds_powerpill_inverse;
    static double total_powerpills_inverse;
    static double total_normal_pills_inverse;
    static double n_ghosts_inverse;
    static double max_distance_inverse;

    static void init(const State& s) {
        total_rounds_powerpill_inverse = 1.0/Arguments::n_rounds_powerpill;
        total_powerpills_inverse = 1.0/State::total_powerpills;
        total_normal_pills_inverse = 1.0/State::total_normal_pills;
        max_distance_inverse = 1.0/State::max_dist;
        n_ghosts_inverse = 1.0/s.ghosts.size();
    }

    static bool can_pacman_reach_safely(const Position& pos) {}

    static inline void closest_pills(const State& s, const Position& from, Position& closest, Position& closest_normal, Position& closest_powerpill) {
        auto f = [s, from](int a, int b) { return PathMagic::dist(from, s.valid_positions[a]) <  PathMagic::dist(from, s.valid_positions[b]) ; };

        auto minnp = min_element(s.normal_pills.begin(), s.normal_pills.end(), f);
        auto minpp = min_element(s.powerpills.begin(), s.powerpills.end(), f);

        if (minpp == s.normal_pills.end()) {
            closest = s.valid_positions[*minnp];
            closest_normal = closest;
        }
        else if (minnp == s.powerpills.end()) {
            closest_powerpill = s.valid_positions[*minpp];
            closest = closest_powerpill;
        }
        else {
            closest = s.valid_positions[min(*minpp, *minnp, f)];
            closest_normal = s.valid_positions[*minnp];
            closest_powerpill = s.valid_positions[*minnp];
        }
    }

    static bool is_approaching_pacman(const Ghost_State& g, const State& s) {
        return g.n_rounds_revive == 0 and PathMagic::from_to(g.pos, s.pacman.pos) == g.dir;
    }


    static void compute_undirected(double* input, const State& s) {
        input[0] = (s.total_powerpills - (s.n_powerpills_left))*total_powerpills_inverse;
        input[1] = (s.total_normal_pills - (s.n_normal_pills_left))*total_normal_pills_inverse;
        input[2] = s.n_rounds_powerpill*total_rounds_powerpill_inverse;
        input[3] = s.n_rounds_powerpill > 0 ?
                   count_if(s.ghosts.begin(), s.ghosts.end(), [](const Ghost_State& g){ return g.maybe_scared; })*n_ghosts_inverse
                                            : 0;
        input[4] = any_of(s.powerpills.begin(), s.powerpills.end(),
                          [s](int p) { return PathMagic::dist(s.pacman.pos, s.valid_positions[p]) <= 10; });
    }

    static void compute_directed(double* input, const Direction& d, const State& s) {
        const Position& pos = s.pacman.pos.move_destination(d);

        auto f = [s](const Position& p) { return [s, p](const Ghost_State& g1, const Ghost_State& g2) { return PathMagic::dist(p, g1.pos) < PathMagic::dist(p, g2.pos); }; };
        Ghost_State closest_ghost_pacman = *min_element(s.ghosts.begin(), s.ghosts.end(), f(s.pacman.pos)); // TODO: Change to undirected
        Ghost_State closest_ghost_pos = *min_element(s.ghosts.begin(), s.ghosts.end(), f(pos));

        vector<Ghost_State> scared_ghosts;
        if (s.n_rounds_powerpill > 0)
            for (Ghost_State g : s.ghosts) if (g.maybe_scared) scared_ghosts.push_back(g);

        auto closest_scared_pacman = min_element(scared_ghosts.begin(), scared_ghosts.end(), f(s.pacman.pos)); // TODO: Change to undirected
        auto closest_scared_pos = min_element(scared_ghosts.begin(), scared_ghosts.end(), f(pos));
        Position closest_normal_pill, closest_pill, closest_powerpill;
        closest_pills(s, s.pacman.pos, closest_pill, closest_normal_pill, closest_powerpill); // TODO: Change to undirected

        input[5] = count_if(s.ghosts.begin(), s.ghosts.end(),
                            [s, pos](const Ghost_State& g){ return PathMagic::dist(pos, g.pos) <= 15 and is_approaching_pacman(g, s); })*n_ghosts_inverse;
        input[6] = accumulate(s.ghosts.begin(), s.ghosts.end(), 0, [s, pos](uint a, const Ghost_State& g) { return a + is_approaching_pacman(g, s)*(PathMagic::dist(g.pos, pos)); })*max_distance_inverse*(1.0/4.0);
        input[7] = s.pacman.dir == d;
        input[8] = accumulate(s.ghosts.begin(), s.ghosts.end(), 0, [s, pos](uint a, const Ghost_State& g) { return a + PathMagic::dist(pos, g.pos); })*max_distance_inverse*(1.0/4.0);
        input[9] = PathMagic::from_to(closest_pill, s.pacman.pos) == d; // TODO: Change to undirected
        input[10] = PathMagic::from_to(s.pacman.pos, closest_ghost_pacman.pos).isOpposite(d); // TODO: Change to undirected
        input[11] = closest_powerpill.i != -1 ? PathMagic::from_to(closest_powerpill, s.pacman.pos) == d : 0; // TODO: Change to undirected
        input[12] = closest_scared_pacman != scared_ghosts.end() ? PathMagic::from_to(s.pacman.pos, closest_ghost_pacman.pos) == d : 0.0; // TODO: Change to undirected
        input[13] = PathMagic::dist(closest_pill, pos)*max_distance_inverse;
        input[14] = closest_powerpill.i != -1 ? PathMagic::dist(closest_powerpill, pos)*max_distance_inverse : s.max_dist;
    }
};

double Inputs::total_rounds_powerpill_inverse;
double Inputs::total_powerpills_inverse;
double Inputs::total_normal_pills_inverse;
double Inputs::n_ghosts_inverse;
double Inputs::max_distance_inverse;
#endif //SRC_INPUTS_HH
