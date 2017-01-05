//
// Created by Albert Segarra Roca on 31/12/16.
//

#ifndef SRC_NN_PACMAN_AGENT_HH
#define SRC_NN_PACMAN_AGENT_HH

#include "../../../state/direction.hh"
#include "../../../state/state.hh"
#include "neural_network.hh"
#include "rl_pacman_agent_inputs.hh"

// This pacman agent follows the best direction indicated
// by the Q-value returned by its neural network, which is
// loaded from a file. Only used for testing the RL AI after
// training

class NN_Pacman_Agent : public Agent {

public:

    Neural_Network nn;
    vector<double> input;

    NN_Pacman_Agent(string neural_network_path) : nn(neural_network_path, 0.0) {
        ensure(nn.n_inputs == uint(RL_Pacman_Agent_Inputs::n_inputs), "This neural network was trained with different inputs");
        input = vector<double>(RL_Pacman_Agent_Inputs::n_inputs);
    }

    NN_Pacman_Agent(const Neural_Network& o) : nn(o) {
        ensure(nn.n_inputs == uint(RL_Pacman_Agent_Inputs::n_inputs), "This neural network was trained with different inputs");
        input = vector<double>(RL_Pacman_Agent_Inputs::n_inputs);
    }

    inline Direction take_action(const State& s, uint ghost_id) {
        const Position& pos = s.pacman.pos;

        RL_Pacman_Agent_Inputs::set_input();
        RL_Pacman_Agent_Inputs::compute_undirected(s, input);

        double max_q = numeric_limits<double>::lowest();
        Direction best_dir;

        for (char i : PathMagic::dirs(pos)) {
            const Direction& d = Direction::LIST[(int)i];

            RL_Pacman_Agent_Inputs::compute_directed(d, s, input);

            if (Arguments::show_inputs) {
                cout << "Direction " << Direction::name(i) << ":" << endl;
                RL_Pacman_Agent_Inputs::debug(input);
                cout << endl;
            }

            double q = nn.recall(&input[0])[0];

            if (q > max_q) {
                best_dir = d;
                max_q = q;
            }
        }

        return best_dir;
    }
};
#endif //SRC_NN_PACMAN_AGENT_HH
