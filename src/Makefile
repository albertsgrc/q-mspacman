all: pacman

pacman: main.cc agent.hh arguments.hh game.hh utils.hh agent_state.hh direction.hh ghost_agent.hh ghost_state.hh pathfinding_pacman_agent.hh position.hh state.hh best_first.hh wsp.hh pathfinding.hh input_pacman_agent.hh bfs.hh random_pacman_agent.hh neural_network.hh rl_pacman_agent.hh
	g++ -g -Ofast -std=c++14 -march=native -Wall -Wno-uninitialized main.cc -o pacman

clean:
	rm -rf pacman
