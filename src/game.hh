#ifndef GAME_HH
#define GAME_HH

#include <fstream>
#include <sstream>

#include "agent.hh"
#include "arguments.hh"
#include "utils.hh"

const uchar PILL = '.';
const uchar POWER_PILL = 'o';
const uchar PACMAN = 'P';
const uchar GHOST = 'G';
const uchar FREE = ' ';
const uchar WALL = '%';

enum Direction {
    LEFT, RIGHT, UP, DOWN
};

struct GameResult {
    GameResult() {}

    bool won;
};

struct Position {
    int i, j;

    Position() : i(-1), j(-1) {}
    Position(int _i, int _j) : i(_i), j(_j) {}

    void move(Direction d) {
        switch(d) {
            case Direction::LEFT: --j; break;
            case Direction::RIGHT: ++j; break;
            case Direction::UP: --i; break;
            case Direction::DOWN: ++i; break;
        }
    }

    friend ostream& operator<<(ostream& s, const Position& p) {
        s << p.i << ' ' << p.j;
        return s;
	}
};

struct Ghost {
    bool scared;

    Position pos;
    Position initial_pos;

    // Number of rounds left until ghost revives. 0 if alive
    int n_rounds_revive;

    Ghost() {}
    Ghost(Position _pos, bool _scared) :
        pos(_pos), initial_pos(_pos), scared(_scared), n_rounds_revive(0) {}
};

class Game {
private:
    GameResult result;

    // Contains the current state of the maze
    Matrix<char> maze;

    // Number of power pill rounds left. 0 if no power pill in action
    int n_rounds_power_pill;

    int n_pills_left;

    vector<Ghost> ghosts;
    Position pacman;

    void load_maze() {
        ifstream layout_file;
        layout_file.open(Arguments::layout_path);

        if (not layout_file.is_open())
            error("Invalid layout file path '" + Arguments::layout_path + "'");

        n_pills_left = n_rounds_power_pill = 0;

        int rows, cols;
        layout_file >> rows >> cols;
        maze = Matrix<char>(rows, cols);

        layout_file >> std::noskipws;
        for (int i = 0; i < rows; ++i) {
            char dummy; layout_file >> dummy;
            for (int j = 0; j < cols; ++j) {
                char cell; layout_file >> cell;
                maze[i][j] = cell;
                n_pills_left += cell == PILL;

                if (cell == PACMAN) pacman = Position(i, j);
                else if (cell == GHOST) ghosts.push_back(Ghost(Position(i, j), false));
            }
        }

        cout << maze;
        cout << "Pills left: " << n_pills_left << endl;
        cout << "Pacman: " << pacman << endl;
        for (Ghost ghost : ghosts)
            cout << "Ghost: " << ghost.pos << endl;
    }

public:
    Agent& agent;

    Game(Agent& _agent) : agent(_agent) {
        load_maze();
    }

    GameResult& play() {
        this->result = GameResult();

        return this->result;
    }
};

#endif
