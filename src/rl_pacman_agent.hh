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
    static const int N_INPUTS = 6;

    double reward;

    double previous_input[N_INPUTS];

    uint n_games;
    double total_plays_inverse;

    double n_rounds_powerpill_inverse;

    Neural_Network nn;

    RL_Pacman_Agent() : reward(0), n_games(1), total_plays_inverse(1.0/Arguments::plays),
                        n_rounds_powerpill_inverse(1.0/Arguments::n_rounds_powerpill),
                        nn(N_INPUTS, Arguments::n_hidden_layers, Arguments::n_hidden_neurons, 1, Arguments::learning_rate)
    {
        for (int i = 0; i < N_INPUTS; ++i) previous_input[i] = 0.0;
    }

    inline Direction take_action(const State& s, uint ghost_id) {
        const Position& pos = s.pacman.pos;

        double input[N_INPUTS];

        input[0] = (s.total_pills - (s.n_pills_left + s.n_powerpills_left))*s.total_pills_inverse;
        input[1] = s.n_rounds_powerpill*n_rounds_powerpill_inverse;

        // Input number of rounds ?

        Direction best_dir;
        double max_q = numeric_limits<double>::lowest();
        double max_input[N_INPUTS];

        vector<Direction> valid_dirs, valid_dirs_no_opposite;
        s.valid_dirs_extended(pos, s.pacman.dir, valid_dirs, valid_dirs_no_opposite);

        for (uint j = 0; j < valid_dirs.size(); ++j) {
            const Direction& d = valid_dirs[j];

            const Position& rpos = pos.move_destination(d);
            input[2] = bfs(rpos, [s](const Position& p){ return s.has_any_pill(p); }, s).dist*s.max_dist_inverse;
            input[3] = bfs(rpos, [s](const Position& p){ return s.dangerous_ghost_in_position(p); }, s).dist*s.max_dist_inverse;
            input[4] = bfs(rpos, [s](const Position& p){ return s.is_intersection(p); }, s).dist*s.max_dist_inverse;
            input[5] = s.pacman.dir == d;

            double q = nn.recall(input)[0];

            if (q > max_q) {
                memcpy(max_input, input, N_INPUTS*sizeof(double));
                max_q = q;
                best_dir = d;
            }
        }

        double adv = double(n_games)/Arguments::plays;
        Direction take;
        if (randdouble() >= clamp(adv, 0.2, 0.95)) {
            if (Arguments::smart_exploration) {
                if (s.pacman.pos != s.pacman.prev)  {
                    take = valid_dirs_no_opposite.size() > 0 ?
                           valid_dirs_no_opposite[randint(valid_dirs_no_opposite.size())] :
                           s.pacman.dir.opposite();
                }
                else take = s.pacman.dir;
            }
            else take = valid_dirs[randint(valid_dirs.size())];
        }
        else take = best_dir;

        // If the game ended previously there's no sense of future reward for the last move of the game
        // Note that the first train of all games is a bit sketchy (all inputs 0, reward 0) but doesn't matter
        // in the long term
        double expected[1] = { reward + (s.round > 1 ? Arguments::discount_factor*max_q : 0) };
        nn.train(previous_input, expected);

        memcpy(previous_input, max_input, N_INPUTS*sizeof(double));

        reward = Arguments::reward_step;

        return take;
    };

    inline void notify_game_result(bool won) override {
        reward += won ? Arguments::reward_win : Arguments::reward_lose;
        ++n_games;
    };
    inline void notify_eaten_pill() override { reward += Arguments::reward_pill; };
    inline void notify_eaten_powerpill() override { reward += Arguments::reward_powerpill; }
    inline void notify_killed_ghost() override { reward += Arguments::reward_kill_ghost; }
    inline void notify_reverse_direction() override { reward += Arguments::reward_reverse; }
};

#endif //RL_PACMAN_AGENT_HH
