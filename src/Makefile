all: pacman

pacman: main.cc agent.hh arguments.hh game.hh utils.hh agent_state.hh direction.hh ghost_agent.hh ghost_state.hh pacman_agent.hh position.hh state.hh a_star.hh
	g++ -std=c++14 -g -D_GLIBCXX_DEBUG -march=native -Wall -Wno-uninitialized main.cc -o pacman

clean:
	rm -rf pacman
