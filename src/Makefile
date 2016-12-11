all: pacman

pacman: main.cc agent.hh arguments.hh game.hh utils.hh agent_state.hh direction.hh ghost_agent.hh ghost_state.hh pathfinding_pacman_agent.hh position.hh state.hh best_first.hh wsp.hh pathfinding.hh input_pacman_agent.hh bfs.hh
	g++ -O3 -std=c++14 -g -D_GLIBCXX_DEBUG -march=native -Wall -Wno-uninitialized main.cc -o pacman

clean:
	rm -rf pacman
