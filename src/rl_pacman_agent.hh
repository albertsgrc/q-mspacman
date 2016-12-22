//
// Created by Albert Segarra Roca on 18/12/16.
//

#ifndef RL_PACMAN_AGENT_HH
#define RL_PACMAN_AGENT_HH

#include <limits>
#include <cassert>

#include "agent.hh"
#include "neural_network.hh"
#include "arguments.hh"
#include "bfs.hh"

class RL_Pacman_Agent: public Agent {
public:
    static const int RESOLUTION = 7;
    static const int N_INPUTS = 3 + RESOLUTION*RESOLUTION*5;

    double reward;

    double previous_input[N_INPUTS];

    uint n_games;
    double total_plays_inverse;
    int prevmaxdir;

    double prop_select_best;
    int n_best_selected;
    int total_possible;

    double n_rounds_powerpill_inverse;

    Neural_Network nn[4];

    RL_Pacman_Agent() : reward(0), n_games(1), total_plays_inverse(1.0/Arguments::plays), prevmaxdir(0),
                        prop_select_best(0), n_best_selected(0), total_possible(0),
                        n_rounds_powerpill_inverse(1.0/Arguments::n_rounds_powerpill)
                        {
        for (int i = 0; i < N_INPUTS; ++i) previous_input[i] = 0.0;

        for (int i = 0; i < 4; ++i)
            nn[i] = Neural_Network(N_INPUTS, Arguments::n_hidden_layers, Arguments::n_hidden_neurons, 1, Arguments::learning_rate);
    }

    inline Direction take_action(const State& s, uint ghost_id) {
        const Position& pos = s.pacman.pos;

        double input[N_INPUTS];

        input[0] = (s.total_pills - (s.n_pills_left + s.n_powerpills_left))*s.total_pills_inverse;
        input[1] = s.n_rounds_powerpill*n_rounds_powerpill_inverse;

        int k = 2;
        for (int i = pos.i - RESOLUTION/2; i <= pos.i + RESOLUTION/2; ++i) {
            for (int j = pos.j - RESOLUTION/2; j <= pos.j + RESOLUTION/2; ++j) {
                Position cpos(i, j);

                if (s.inside_boundaries(cpos)) {
                    input[k] = s.dangerous_ghost_in_position(cpos);
                    input[k+1] = s.has_any_pill(cpos);
                    input[k+2] = s.has_powerpill(cpos);
                    input[k+3] = s.scared_ghost_in_position(cpos);
                    input[k+4] = s.is_wall(cpos);
                }
                else {
                    input[k] = input[k+1] = input[k+2] = input[k+3] = 0;
                    input[k+4] = 1;
                }

                k += 5;
            }
        }

        // Input number of rounds ?

        Direction best_dir;
        double max_q = numeric_limits<double>::lowest();
        double max_input[N_INPUTS];
        int maxdir = -1;

        vector<Direction> valid_dirs = s.valid_dirs(pos);

        for (uint j = 0; j < valid_dirs.size(); ++j) {
            const Direction& d = valid_dirs[j];

            input[k] = s.pacman.dir == d;

            int dirindex = Direction::index(d);

            double q = nn[dirindex].recall(input)[0];

            if (q > max_q) {
                memcpy(max_input, input, N_INPUTS*sizeof(double));
                max_q = q;
                best_dir = d;
                maxdir = dirindex;
            }
        }

        double adv = double(n_games)/Arguments::plays;
        Direction take = randdouble() > clamp(adv, 0.2, 0.95) ? valid_dirs[randint(valid_dirs.size())] : best_dir;

        if (take == best_dir) ++n_best_selected;
        ++total_possible;

        // If the game ended previously there's no sense of future reward for the last move of the game
        // Note that the first train of all games is a bit sketchy (all inputs 0, reward 0) but doesn't matter
        // in the long term
        double expected[1] = { reward + (s.round > 1 ? Arguments::discount_factor*max_q : 0) };
        nn[prevmaxdir].train(previous_input, expected);

        memcpy(previous_input, max_input, N_INPUTS*sizeof(double));
        prevmaxdir = maxdir;

        reward = Arguments::reward_step;

        return take;
    };

    inline void notify_game_result(bool won) override {
        reward += won ? Arguments::reward_win : Arguments::reward_lose;
        ++n_games;
        prop_select_best = double(n_best_selected)/total_possible;
        n_best_selected = total_possible = 0;
    };
    inline void notify_eaten_pill() override { reward += Arguments::reward_pill; };
    inline void notify_eaten_powerpill() override { reward += Arguments::reward_powerpill; }
    inline void notify_killed_ghost() override { reward += Arguments::reward_kill_ghost; }
    inline void notify_reverse_direction() override { reward += Arguments::reward_reverse; }
};

#endif //RL_PACMAN_AGENT_HH
