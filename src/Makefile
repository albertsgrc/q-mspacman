all: pacman

pacman: main.cc agent.hh arguments.hh game.hh utils.hh
	g++ -std=c++14 -O3 -march=native -Wall -Wno-uninitialized -Wshadow main.cc -o pacman

clean:
	rm -rf pacman
