//
// Created by Albert Segarra Roca on 18/12/16.
//

#ifndef RL_PACMAN_AGENT_HH
#define RL_PACMAN_AGENT_HH

#include <limits>
#include <cassert>

#include "../../agent.hh"
#include "neural_network.hh"
#include "../../../arguments.hh"
#include "../../../pathfinding/bfs.hh"
#include "rl_pacman_agent_inputs.hh"

class RL_Pacman_Agent: public Agent {
public:

    double mse_sum;
    double mse_sum_last;
    double reward;

    vector<double> previous_input;
    vector<double> input;
    vector<double> max_input;

    uint n_games;

    Neural_Network nn;

    RL_Pacman_Agent() : reward(0), n_games(1),
                        nn(RL_Pacman_Agent_Inputs::n_inputs, Arguments::n_hidden_layers, Arguments::n_hidden_neurons, 1, Arguments::learning_rate, Arguments::activation_function)
    {
        previous_input = vector<double>(RL_Pacman_Agent_Inputs::n_inputs, 0.0);
        max_input = vector<double>(RL_Pacman_Agent_Inputs::n_inputs);
        input = vector<double>(RL_Pacman_Agent_Inputs::n_inputs);
        mse_sum_last = mse_sum = 0.0;
    }

    inline Direction take_action(const State& s, uint ghost_id) override {
        const Position& pos = s.pacman.pos;

        RL_Pacman_Agent_Inputs::set_input();

        RL_Pacman_Agent_Inputs::compute_undirected(s, input);

        Direction best_dir;
        double max_q = numeric_limits<double>::lowest();

        vector<char>& valid_dirs = PathMagic::dirs(pos);
        vector<Direction> valid_dirs_no_opposite;

        for (char i : valid_dirs) {
            const Direction& d = Direction::LIST[(int)i];
            if (not d.isOpposite(s.pacman.dir)) valid_dirs_no_opposite.push_back(d);

            RL_Pacman_Agent_Inputs::compute_directed(d, s, input);

            if (Arguments::show_inputs) {
                cout << "Direction " << Direction::name(i) << ":" << endl;
                RL_Pacman_Agent_Inputs::debug(input);
                cout << endl;
            }

            double q = nn.recall(&input[0])[0];

            if (q > max_q) {
                max_input = input;
                max_q = q;
                best_dir = d;
            }
        }

        double progression = double(n_games)/Arguments::plays;

        bool should_explore =
                Arguments::exploration_strategy == ANNEALING ?
                    randdouble() >= clamp(progression,
                                          Arguments::exploration_annealing_min_progression,
                                          Arguments::exploration_annealing_max_progression)
                            :
                    (randdouble() < Arguments::exploration_epsilon and progression < Arguments::exploration_epsilon_stop_progression);

        Direction take;
        if (should_explore) { // epsilon-learning (prob. 0.1) is also really good!
            if (Arguments::smart_exploration) {
                if (s.pacman.pos != s.pacman.prev)  {
                    take = valid_dirs_no_opposite.size() > 0 ?
                           valid_dirs_no_opposite[randint(valid_dirs_no_opposite.size())] :
                           s.pacman.dir.opposite();
                }
                else take = s.pacman.dir;
            }
            else take = Direction::LIST[(int)valid_dirs[randint(valid_dirs.size())]];
        }
        else take = best_dir;

        // If the game ended previously there's no sense of future reward for the last move of the game
        // Note that the first train of all games is a bit sketchy (all inputs 0, reward 0) but doesn't matter
        // in the long term

        double expected[1] = { reward + (s.round > 1 ? Arguments::discount_factor*max_q : 0) };
        double mse = nn.train(&previous_input[0], expected);
        mse_sum += mse;

        previous_input.swap(max_input);

        reward = Arguments::reward_step;

        return take;
    };

    inline void notify_game_result(bool won) override {
        reward += won ? Arguments::reward_win : Arguments::reward_lose;
        ++n_games;
        mse_sum_last = mse_sum;
        mse_sum = 0.0;
    };
    inline void notify_eaten_pill() override { reward += Arguments::reward_pill; };
    inline void notify_eaten_powerpill() override { reward += Arguments::reward_powerpill; }
    inline void notify_killed_ghost() override { reward += Arguments::reward_kill_ghost; }
    inline void notify_reverse_direction() override { reward += Arguments::reward_reverse; }
};

#endif //RL_PACMAN_AGENT_HH
