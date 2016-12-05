#ifndef GAME_HH
#define GAME_HH

#include <fstream>
#include <sstream>
#include <unistd.h>

#include "agent.hh"
#include "arguments.hh"
#include "utils.hh"
#include "position.hh"
#include "ghost_agent.hh"

struct GameResult {
    GameResult() {}

    bool won;
    State state;
};

class Game {
private:
    GameResult result;
    State initialState;
    State state;
    bool loaded_maze;

    bool game_over;

    Agent* pacman;
    vector<Agent*> ghosts;

    float ghost_speed(int ghost_id) {
        float speed_normal = Arguments::ghost_speed*Arguments::pacman_speed;

        return state.ghosts[ghost_id].scared ?
               speed_normal*Arguments::ghost_afraid_speed_fraction :
               speed_normal;
    }

    bool game_over_on_collision(Ghost_State& ghost) {
        if (state.n_rounds_powerpill > 0 and ghost.scared) {
            ghost.scared = false;
            ghost.n_rounds_revive = Arguments::n_rounds_ghost_revive;
        }
        else if (ghost.n_rounds_revive == 0) {
            game_over = true;
            return true;
        }

        return false;
    }

public:

    Game(Agent* pacman) : loaded_maze(false), pacman(pacman), game_over(false) {}

    void load_maze() {
        loaded_maze = true;

        ifstream layout_file;
        layout_file.open(Arguments::layout_path);

        if (not layout_file.is_open())
            error("Invalid layout file path '" + Arguments::layout_path + "'");

        int rows, cols;
        layout_file >> rows >> cols;
        state.maze = Matrix<char>(rows, cols);

        layout_file >> std::noskipws;
        for (int i = 0; i < rows; ++i) {
            char dummy; layout_file >> dummy;
            for (int j = 0; j < cols; ++j) {
                char cell; layout_file >> cell;
                state.maze[i][j] = cell != State::PACMAN and cell != State::GHOST ? cell : State::FREE;

                state.n_pills_left += cell == State::PILL;
                state.n_powerpills_left += cell == State::POWER_PILL;

                if (cell == State::PACMAN) state.pacman = Agent_State(Position(i, j), Direction::UP);
                else if (cell == State::GHOST) {
                    state.ghosts.push_back(Ghost_State(Position(i, j), Direction::UP));
                    ghosts.push_back((Agent*)new Ghost_Agent());
                }
            }
        }

        //cout << state << endl;

        initialState = state;
    }

    void reset() {
        assert(loaded_maze);
        state = initialState;
    }

    GameResult& play() {
        assert(loaded_maze);

        while (state.n_powerpills_left + state.n_pills_left > 0) {
            cout.flush();
            usleep(1000000);

            ++state.round;
            state.n_rounds_powerpill = max(0, state.n_rounds_powerpill - 1);

            Direction pacman_direction = pacman->take_action(state, 0);

            // TODO: Maybe process movements stochastically to guarantee equality?

            if (state.pacman.dir == pacman_direction) {
                state.pacman.step += Arguments::pacman_speed;

                if (state.pacman.step >= 1) {
                    --state.pacman.step;

                    Position next_pos = state.pacman.pos.move_destination(pacman_direction);
                    char cell_content = state.maze[next_pos.i][next_pos.j];


                    if (cell_content == State::WALL) {
                        // TODO: Warn
                    }
                    else {
                        state.pacman.pos.move(pacman_direction);

                        int ghost_id = state.ghost_in_position(next_pos);

                        if (ghost_id != -1 and game_over_on_collision(state.ghosts[ghost_id]))
                            goto end;

                        switch(cell_content) {
                            case State::PILL:
                                state.maze[next_pos.i][next_pos.j] = State::FREE;
                                --state.n_pills_left;
                                break;
                            case State::POWER_PILL:
                                state.maze[next_pos.i][next_pos.j] = State::FREE;
                                --state.n_powerpills_left;

                                for (Ghost_State& ghost : state.ghosts) ghost.scared = true;

                                state.n_rounds_powerpill = Arguments::n_rounds_powerpill;
                                break;
                            case State::FREE: break;
                            default: assert(false);
                        }
                    }
                }
            }
            else {
                state.pacman.step = (1 - state.pacman.step)*Arguments::pacman_speed;
                state.pacman.dir = pacman_direction;
            }

            // Comprovar si hi ha canvi de direccio
            // Augmentar / 1-step
            // Si arriba a 1:
            //    Step a 0
            //    Comprovar q no hi ha paret
            //      Si hi ha no es mou
            //      Si no hi ha
            //        Moure a posicio
            //        Comprovar si hi ha power pill
            //          Si hi ha, decrementar contador, posar ghosts a scared i posar n_rounds_powerpill
            //        Sino, si hi ha pill
            //          Decrementar contador
            //        Sino, si hi ha ghost
            //          Si esta scared es mata
            //          S'acaba el joc, game_over = true i break

            for (uint i = 0; i < min(1 + state.round/4, (int) ghosts.size()); ++i) {
                Ghost_State& ghost = state.ghosts[i];

                if (ghost.n_rounds_revive == 0) {
                    Direction ghost_direction = ghosts[i]->take_action(state, i);

                    if (ghost_direction == ghost.dir) {
                        ghost.step += ghost_speed(i);

                        if (ghost.step >= 1) {
                            --ghost.step;

                            Position next_pos = ghost.pos.move_destination(ghost_direction);
                            char cell_content = state.maze[next_pos.i][next_pos.j];

                            if (cell_content == State::WALL) {
                                // TODO: WARN
                            }
                            else {
                                ghost.pos.move(ghost_direction);

                                if (state.pacman.pos == next_pos and game_over_on_collision(ghost))
                                    goto end;
                            }
                        }
                    }
                    else {
                        ghost.step = (1 - ghost.step)*ghost_speed(i);
                        ghost.dir = ghost_direction;
                    }
                }
                else if (ghost.n_rounds_revive == 1) {
                    ghost.pos = ghost.initial_pos;
                    ghost.n_rounds_revive = 0;
                }
                else {
                    --ghost.n_rounds_revive;
                }

                // Comprovar si hi ha canvi de direccio
                // Augmentar segons si esta scared (scared && n_rounds_powerpill > 0) / 1-step
                // Si arriba a 1:
                //    Step a 0
                //    Comprovar q no hi ha paret
                //      Si hi ha no es mou
                //      Si no hi ha
                //        Moure a posicio
                //        Comprovar si hi ha pacman
                //          Si hi ha, s'acaba el joc, game_over = true i break
            }

            cout << state << endl;
        }

        end:

        cout << state << endl;
        cout << game_over << endl;

        this->result.state = this->state;
        this->result.won = not game_over;
        return this->result;
    }
};

#endif
