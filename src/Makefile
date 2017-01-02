all: pacman

pacman: main.cc agents/agent.hh arguments.hh game.hh utils.hh state/agents/agent_state.hh state/direction.hh agents/ghosts/ghost_agent.hh state/agents/ghost_state.hh agents/pacman/pathfinding_pacman_agent.hh state/position.hh state/state.hh pathfinding/wsp.hh pathfinding/pathfinding.hh agents/pacman/input_pacman_agent.hh pathfinding/bfs.hh agents/pacman/random_pacman_agent.hh agents/pacman/rl/neural_network.hh agents/pacman/rl/rl_pacman_agent.hh agents/pacman/rl/rl_pacman_agent_inputs.hh agents/pacman/rl/nn_pacman_agent.hh
	g++ -g -Ofast -std=c++11 -march=native -Wall -Wno-uninitialized main.cc -o pacman

clean:
	rm -rf pacman
