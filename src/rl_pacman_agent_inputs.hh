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

class RL_Pacman_Agent_Inputs {
public:
    static double total_rounds_powerpill_inverse;
    static double total_powerpills_inverse;
    static double total_normal_pills_inverse;
    static double n_ghosts_inverse;
    static double max_distance_inverse;
    static double n_intersections_inverse;

    static int n_inputs;

    static int current_input;
    static double* input;

    static vector<vector<double>> safe_paths;
    static vector<double> total_safe_paths_inverse;

    static void precompute() {
        total_rounds_powerpill_inverse = 1.0/Arguments::n_rounds_powerpill;
        total_powerpills_inverse = State::total_powerpills > 0 ? 1.0/State::total_powerpills : 1;
        total_normal_pills_inverse = State::total_normal_pills > 0 ? 1.0/State::total_normal_pills : 1;
        max_distance_inverse = 1.0/State::max_dist;
        n_ghosts_inverse = 1.0/State::n_ghosts;
        n_intersections_inverse = PathMagic::intersections.size() > 0 ? 1.0/PathMagic::intersections.size() : 1;
        n_inputs = State::n_ghosts*6 + Arguments::max_intersection_distance + 13;
    }

    static inline void set_input(double* input_p) {
        current_input = 0;
        input = input_p;
    }

    static inline void add_input(double v) {
        input[current_input++] = v;
    }

    static inline bool between_path_to_intersection(const State &state, int pacman_pos, const Ghost_State &ghost_state, int intersection) {
        int ghost_pos_id = PathMagic::id(ghost_state.pos);
        return PathMagic::distances[ghost_pos_id][intersection] < PathMagic::distances[pacman_pos][intersection]
                and
               PathMagic::n_intersections[pacman_pos][ghost_pos_id] == 0
                and
               PathMagic::n_intersections[pacman_pos][intersection] == 1
                and
               PathMagic::n_intersections[ghost_pos_id][intersection] == 1;
    }

    static bool can_pacman_reach_safely(const State& s, int pacman_pos, int pos) {
        for (const Ghost_State& g : s.ghosts)
            if (g.is_alive() and not s.is_scared(g) and
                    ((PathMagic::corrected_dist(PathMagic::id(g.pos), pos, g, Arguments::ghost_speed) <
                                 PathMagic::corrected_dist(pacman_pos, pos, s.pacman, 1.0) + Arguments::safe_distance_margin)
                     or between_path_to_intersection(s, pacman_pos, g, pos)))
                return false;

        return true;
    }

    static inline void closest_pills(const State& s, const Position& from, Position& closest, Position& closest_powerpill,
                                     int& dist_pill, int& dist_powerpill) {
        int fromid = PathMagic::id(from);
        dist_pill = s.max_dist;

        for (int pill : s.pills) {
            if (PathMagic::distances[fromid][pill] < dist_pill) {
                dist_pill = PathMagic::distances[fromid][pill];
                closest = s.valid_positions[pill];
            }
        }

        dist_powerpill = s.max_dist;

        for (int pill : s.powerpills) {
            if (PathMagic::distances[fromid][pill] < dist_powerpill) {
                dist_powerpill = PathMagic::distances[fromid][pill];
                closest_powerpill = s.valid_positions[pill];
            }
        }
    }

    static bool is_approaching_pacman(const Ghost_State& g, const State& s) {
        return g.is_alive() and PathMagic::from_to(g.pos, s.pacman.pos) == g.dir;
    }

    static inline void compute_undirected(const State& s) {
        current_input = 0;
        int pacman_pos_id = PathMagic::id(s.pacman.pos);
        Position closest_pill, closest_powerpill;
        int dist_pill, dist_powerpill;
        closest_pills(s, s.pacman.pos, closest_pill, closest_powerpill, dist_pill, dist_powerpill);

        add_input((s.total_powerpills - (s.n_powerpills_left))*total_powerpills_inverse);
        add_input((s.total_normal_pills - (s.n_normal_pills_left))*total_normal_pills_inverse);
        add_input(s.n_rounds_powerpill*total_rounds_powerpill_inverse);
        add_input(dist_powerpill <= Arguments::close_powerpill_distance);

        int n_scared = 0;
        if (s.n_rounds_powerpill > 0)
            for (const Ghost_State& g : s.ghosts) n_scared += g.maybe_scared;

        add_input(n_scared*n_ghosts_inverse);

        queue<IntersectionPathStep> Q;
        for (int intersection : PathMagic::enclosing_intersections[pacman_pos_id])
            if (can_pacman_reach_safely(s, pacman_pos_id, intersection))
                Q.push(IntersectionPathStep(intersection, 0, PathMagic::direction_from_to[pacman_pos_id][intersection], -1));

        safe_paths = vector<vector<double>>(Direction::NUMBER, vector<double>(Arguments::max_intersection_distance, 0));
        total_safe_paths_inverse = vector<double>(Arguments::max_intersection_distance, 0);

        while (not Q.empty()) {
            IntersectionPathStep step = Q.front();
            Q.pop();

            ++safe_paths[(int) step.direction][step.distance];
            ++total_safe_paths_inverse[step.distance];

            if (step.distance < Arguments::max_intersection_distance - 1)
                for (int intersection : PathMagic::enclosing_intersections[step.intersection])
                    if (intersection != step.camefrom and can_pacman_reach_safely(s, pacman_pos_id, intersection))
                        Q.push(IntersectionPathStep(intersection, step.distance + 1, step.direction, step.intersection));
        }

        for (double& x : total_safe_paths_inverse)
            if (x > 0.0) x = 1.0/x;
    }

    static inline void compute_directed(const Direction& d, const State& s) {
        uint previous_current_input = current_input;

        const Position& pos = s.pacman.pos.move_destination(d);
        int pos_id = PathMagic::id(pos);

        vector<int> dist_ghosts(s.ghosts.size());
        for (uint i = 0; i < s.ghosts.size(); ++i)
            dist_ghosts[i] = s.ghosts[i].is_alive() ? PathMagic::distances[PathMagic::id(s.ghosts[i].pos)][pos_id] : s.max_dist;


        Position closest_pill, closest_powerpill;
        int dist_pill, dist_powerpill;
        closest_pills(s, pos, closest_pill, closest_powerpill, dist_pill, dist_powerpill);

        add_input(s.pacman.dir == d);
        add_input(dist_pill*max_distance_inverse);
        add_input(dist_powerpill*max_distance_inverse);

        vector<int> scared_ghosts;
        vector<int> non_scared_ghosts;
        vector<int> ghosts;

        for (int i = 0; i < s.n_ghosts; ++i) {
            ghosts.push_back(i);
            if (s.ghosts[i].is_alive()) {
                if (s.is_scared(s.ghosts[i])) scared_ghosts.push_back(i);
                else non_scared_ghosts.push_back(i);
            }
        }

        sort(scared_ghosts.begin(), scared_ghosts.end(), [&dist_ghosts](int a, int b) { return dist_ghosts[a] < dist_ghosts[b]; });
        sort(non_scared_ghosts.begin(), non_scared_ghosts.end(), [&dist_ghosts](int a, int b) { return dist_ghosts[a] < dist_ghosts[b]; });
        sort(ghosts.begin(), ghosts.end(), [&dist_ghosts](int a, int b) { return dist_ghosts[a] < dist_ghosts[b]; });

        for (uint i = 0; i < uint(State::n_ghosts); ++i) {
            add_input(i < scared_ghosts.size() ? dist_ghosts[scared_ghosts[i]]*max_distance_inverse : 1);
            add_input(i < non_scared_ghosts.size() ? dist_ghosts[non_scared_ghosts[i]]*max_distance_inverse : 1);
            add_input(i < non_scared_ghosts.size() ? is_approaching_pacman(s.ghosts[non_scared_ghosts[i]], s) : 0);

            add_input(s.ghosts[ghosts[i]].pos.i/double(s.maze.rows()));
            add_input(s.ghosts[ghosts[i]].pos.j/double(s.maze.cols()));

            double in;
            if (i < non_scared_ghosts.size()) {
                const Ghost_State& g = s.ghosts[non_scared_ghosts[i]];
                in = (PathMagic::n_intersections[pos_id][PathMagic::id(g.pos)] - PathMagic::is_intersection(g.pos))*n_intersections_inverse;
            }
            else in = 1;

            add_input(in);
        }

        int min_d = s.max_dist;
        int closest_intersection = -1;
        if (PathMagic::is_intersection(pos)) {
            closest_intersection = pos_id;
            min_d = 0;
        }
        else {
            for (int intersection : PathMagic::enclosing_intersections[pos_id]) {
                if (PathMagic::distances[intersection][pos_id] < min_d) {
                    min_d = PathMagic::distances[intersection][pos_id];
                    closest_intersection = intersection;
                }
            }
        }

        add_input(min_d*max_distance_inverse);
        //add_input(closest_powerpill.i != -1 ? can_pacman_reach_safely(s, pos, closest_powerpill) : 0);

        int dir_index = Direction::index(d);
        for (int i = 0; i < Arguments::max_intersection_distance; ++i)
            add_input(safe_paths[dir_index][i]*total_safe_paths_inverse[i]);

        add_input(safe_paths[dir_index][0] == 0);

        double ghost_dist_intersection;
        if (non_scared_ghosts.size() > 0) {
            const Ghost_State& ghost = s.ghosts[non_scared_ghosts[0]];
            ghost_dist_intersection = PathMagic::corrected_dist_exact(PathMagic::id(ghost.pos), closest_intersection, ghost);
        }
        else ghost_dist_intersection = s.max_dist;

        add_input((s.max_dist + PathMagic::corrected_dist_exact(pos_id, closest_intersection, s.pacman)*Arguments::ghost_speed
                   - ghost_dist_intersection)*max_distance_inverse);

        add_input(pos.i/double(s.maze.rows()));
        add_input(pos.j/double(s.maze.cols()));

        ensure(current_input == n_inputs, "The number of inputs is incorrect");
        current_input = previous_current_input;
    }

    static void debug_helper(string s, double* input, int& index) {
        cout << s << ' ' << input[index++] << endl;
    }
    static void debug(double* input) {
        int i = 0;

        debug_helper("U: Powerpill progress", input, i);
        debug_helper("U: Normal pill progress", input, i);
        debug_helper("U: Powerpill rounds left", input, i);
        debug_helper("U: Powerpill at distance <= 10", input, i);
        debug_helper("U: Proportion of scared ghosts", input, i);
        debug_helper("Going in that direction", input, i);
        debug_helper("Distance closest pill (any)", input, i);
        debug_helper("Distance closest powerpill", input, i);

        for (int j = 0; j < State::n_ghosts; ++j) {
            debug_helper("Distance " + to_string(j) + " closest scared ghost", input, i);
            debug_helper("Distance " + to_string(j) + " closest ghost", input, i);
            debug_helper(to_string(j) + " closest ghost approaching pacman", input, i);
            debug_helper("Position i of " + to_string(j) + " closest ghost", input, i);
            debug_helper("Position j of " + to_string(j) + " closest ghost", input, i);
            debug_helper("Intersections to " + to_string(j) + " closest ghost", input, i);
        }

        debug_helper("Distance closest intersection", input, i);

        for (int j = 0; j < Arguments::max_intersection_distance; ++j)
            debug_helper("Proportion of safe paths " + to_string(j) + " int away", input, i);

        debug_helper("Will be trapped", input, i);
        debug_helper("Ghost danger", input, i);
        debug_helper("Pacman i position", input, i);
        debug_helper("Pacman j position", input, i);

        ensure(i == n_inputs, "The number of inputs is incorrect");
    }
};

double RL_Pacman_Agent_Inputs::total_rounds_powerpill_inverse;
double RL_Pacman_Agent_Inputs::total_powerpills_inverse;
double RL_Pacman_Agent_Inputs::total_normal_pills_inverse;
double RL_Pacman_Agent_Inputs::n_ghosts_inverse;
double RL_Pacman_Agent_Inputs::max_distance_inverse;
double RL_Pacman_Agent_Inputs::n_intersections_inverse;

int RL_Pacman_Agent_Inputs::current_input;
double* RL_Pacman_Agent_Inputs::input;

vector<vector<double>> RL_Pacman_Agent_Inputs::safe_paths;
vector<double> RL_Pacman_Agent_Inputs::total_safe_paths_inverse;

int RL_Pacman_Agent_Inputs::n_inputs;

#endif //SRC_INPUTS_HH
