//
// Created by Albert Segarra Roca on 31/12/16.
//

#ifndef SRC_NN_PACMAN_AGENT_HH
#define SRC_NN_PACMAN_AGENT_HH

#include "direction.hh"
#include "state.hh"
#include "neural_network.hh"
#include "rl_pacman_agent_inputs.hh"

class NN_Pacman_Agent : public Agent {

public:

    Neural_Network nn;

    NN_Pacman_Agent(string neural_network_path) : nn(neural_network_path, 0.0) {}

    inline Direction take_action(const State& s, uint ghost_id) {
        const Position& pos = s.pacman.pos;

        double input[RL_Pacman_Agent_Inputs::N_INPUTS];
        RL_Pacman_Agent_Inputs::set_input(input);
        RL_Pacman_Agent_Inputs::compute_undirected(s);

        double max_q = numeric_limits<double>::lowest();
        Direction best_dir;

        for (char i : PathMagic::dirs(pos)) {
            const Direction& d = Direction::LIST[(int)i];

            RL_Pacman_Agent_Inputs::compute_directed(d, s);

            double q = nn.recall(input)[0];

            if (q > max_q) {
                best_dir = d;
                max_q = q;
            }
        }

        return best_dir;
    }
};
#endif //SRC_NN_PACMAN_AGENT_HH
