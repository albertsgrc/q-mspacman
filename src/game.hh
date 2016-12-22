#ifndef GAME_HH
#define GAME_HH

#include <fstream>
#include <sstream>
#include <unistd.h>
#include <random>
#include <vector>

#include "agent.hh"
#include "arguments.hh"
#include "utils.hh"
#include "position.hh"
#include "ghost_agent.hh"
#include "state.hh"
#include "pathfinding.hh"

struct GameResult {
    GameResult() {}

    bool won;
    State state;
};

class Game {
private:

    Agent* pacman;
    vector<Agent*> ghosts;

    float ghost_speed(int ghost_id) {
        return state.is_scared(state.ghosts[ghost_id]) ?
               Arguments::ghost_speed*Arguments::ghost_afraid_speed_fraction :
               Arguments::ghost_speed;
    }

    // TODO: Take movement step in account?
    bool game_over_on_collision(Ghost_State& ghost) {
        if (state.is_scared(ghost)) {
            pacman->notify_killed_ghost();
            ghost.maybe_scared = false;
            ghost.n_rounds_revive = Arguments::n_rounds_ghost_revive;
            ghost.pos = Position(-1,-1);
        }
        else game_over = true;

        return game_over;
    }

    void update_ghost_states() {
        for (Ghost_State& ghost : state.ghosts) {
            if (ghost.n_rounds_left_behaviour-- == 0) {
                normal_distribution<float> distribution;

                switch (ghost.behaviour) {
                    case SCATTER:
                        distribution = normal_distribution<float>(ghost.chase_cycle_rounds, Arguments::cycle_rounds_stdev);
                        ghost.behaviour = Ghost_Behaviour::CHASE;
                        ghost.chase_cycle_rounds = min(1000000, int(ghost.chase_cycle_rounds*Arguments::chase_cycle_factor));
                        break;
                    case CHASE:
                        if (ghost.scatter_cycle_rounds >= 1) {
                            distribution = normal_distribution<float>(ghost.scatter_cycle_rounds, Arguments::cycle_rounds_stdev);
                            ghost.behaviour = Ghost_Behaviour::SCATTER;
                            ghost.scatter_cycle_rounds /= Arguments::scatter_cycle_factor;
                            ghost.scatter_pos = state.random_valid_pos();
                        }
                        break;
                    default: ensure(false, "Invalid ghost behaviour");
                }

                ghost.n_rounds_left_behaviour = max(1, (int) distribution(Arguments::random_generator));
                __log("Ghost at position [%d,%d] changes behaviour to %s, %d rounds",
                      ghost.pos.i, ghost.pos.j, ghost.behaviour == Ghost_Behaviour::SCATTER ? "scatter" : "chase",
                      (int) (Arguments::pacman_speed*ghost.n_rounds_left_behaviour));
            }

            if (ghost.behaviour == SCATTER) {
                while (ghost.scatter_pos == ghost.pos) ghost.scatter_pos = state.random_valid_pos();
            }
        }
    }

public:
    GameResult result;

    State initialState;
    State state;

    bool loaded_maze;

    bool game_over;

    Game(Agent* pacman) : pacman(pacman), loaded_maze(false), game_over(false) {}

    void load_maze() {
        loaded_maze = true;

        ifstream layout_file;
        layout_file.open(Arguments::layout_path);

        if (not layout_file.is_open())
            error("Invalid layout file path '" + Arguments::layout_path + "'");

        int rows, cols;
        layout_file >> rows >> cols;
        state.maze = Matrix<char>(rows, cols);
        uint valid_index = 0;
        PathMagic::index_from_pos = vector<vector<int>>(rows, vector<int>(cols, -1));

        layout_file >> std::noskipws;
        for (int i = 0; i < rows; ++i) {
            char dummy; layout_file >> dummy;
            for (int j = 0; j < cols; ++j) {
                char cell; layout_file >> cell;
                state.maze[i][j] = cell != State::PACMAN and cell != State::GHOST ? cell : State::FREE;

                state.n_pills_left += cell == State::PILL;
                state.n_powerpills_left += cell == State::POWER_PILL;

                if (cell != State::WALL) {
                    state.valid_positions.push_back(Position(i, j));
                    PathMagic::index_from_pos[i][j] = valid_index++;
                }

                if (cell == State::PACMAN) state.pacman = Agent_State(Position(i, j), Direction::UP);
                else if (cell == State::GHOST) {
                    state.ghosts.push_back(Ghost_State(Position(i, j), Direction::UP));
                    ghosts.push_back((Agent*)new Ghost_Agent());
                }
            }
        }

        state.total_pills = state.n_pills_left + state.n_powerpills_left;
        state.distribution_valid_pos = uniform_int_distribution<>(0, state.valid_positions.size() - 1);

        SeenMatrix::init(rows, cols);
        PathMagic::compute(state);

        state.max_dist_inverse = 1.0/state.max_dist;
        state.total_pills_inverse = 1.0/state.total_pills;

        initialState = state;
    }

    void reset() {
        ensure(loaded_maze, "Try to reset without a loaded maze");
        state = initialState;
        game_over = false;

        for (uint i = 0; i < ghosts.size(); ++i) {
            delete (Ghost_Agent*) ghosts[i];
            ghosts[i] = (Agent*) new Ghost_Agent();
        }
    }

    GameResult& play() {
        ensure(loaded_maze, "Try to play without a loaded maze");

        if (Arguments::plays == 1) cout << state << endl;

        while (state.n_powerpills_left + state.n_pills_left > 0) { // break if game_over
            if (Arguments::plays == 1) {
                cout.flush();
                usleep(200000*Arguments::ghost_speed);
            }

            update_ghost_states();

            ++state.round;
            state.n_rounds_powerpill = max(0, state.n_rounds_powerpill - 1);

            Direction pacman_direction = pacman->take_action(state, 0);

            Position pacman_previous_pos = state.pacman.pos;
            vector<Position> ghost_previous_pos(4);
            for (const Ghost_State& g : state.ghosts) ghost_previous_pos.push_back(g.pos);

            // TODO: Maybe process movements stochastically to guarantee equality?

            if (state.pacman.dir == pacman_direction) {
                state.pacman.step += Arguments::pacman_speed;

                if (state.pacman.step >= 1) {
                    --state.pacman.step;

                    Position next_pos = state.pacman.pos.move_destination(pacman_direction);
                    char cell_content = state.maze[next_pos.i][next_pos.j];


                    if (cell_content == State::WALL) {
                        __log("Pacman crashes with wall");
                    }
                    else {
                        state.pacman.pos.move(pacman_direction);

                        vector<int> ghosts_pos = state.ghosts_in_position(next_pos);

                        for (int ghost_id : ghosts_pos)
                            if (game_over_on_collision(state.ghosts[ghost_id]))
                                goto end;

                        switch(cell_content) {
                            case State::PILL:
                                state.maze[next_pos.i][next_pos.j] = State::FREE;
                                --state.n_pills_left;
                                pacman->notify_eaten_pill();
                                break;
                            case State::POWER_PILL:
                                state.maze[next_pos.i][next_pos.j] = State::FREE;
                                --state.n_powerpills_left;
                                pacman->notify_eaten_powerpill();

                                for (Ghost_State& ghost : state.ghosts) ghost.maybe_scared = true;

                                state.n_rounds_powerpill = Arguments::n_rounds_powerpill;
                                break;
                            case State::FREE: break;
                            default: ensure(false, "Invalid cell_content character '%c'", cell_content);
                        }
                    }
                }
            }
            else {
                pacman->notify_reverse_direction();
                state.pacman.step = (1 - state.pacman.step)*Arguments::pacman_speed;
                state.pacman.dir = pacman_direction;
            }

            for (int i = 0; i < min(1 + state.round/Arguments::n_rounds_between_ghosts_start, (int) ghosts.size()); ++i) {
                Ghost_State& ghost = state.ghosts[i];

                if (ghost.is_alive()) {
                    Direction ghost_direction = ghosts[i]->take_action(state, i);

                    if (ghost_direction == ghost.dir) {
                        ghost.step += ghost_speed(i);

                        if (ghost.step >= 1) {
                            --ghost.step;

                            Position next_pos = ghost.pos.move_destination(ghost_direction);
                            char cell_content = state.maze[next_pos.i][next_pos.j];

                            if (cell_content == State::WALL) {
                                __log("Ghost #%d at [%d,%d] crashes with wall", i, ghost.pos.i, ghost.pos.j);
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

            }

            state.pacman.prev = pacman_previous_pos;
            for (uint i = 0; i < state.ghosts.size(); ++i) state.ghosts[i].prev = ghost_previous_pos[i];

            if (Arguments::plays == 1) cout << state << endl;
        }

        end:

        if (Arguments::plays == 1) {
            cout << state << endl;
            cout << (game_over ? "LOST" : "WON") << endl;
        }

        this->result.won = not game_over;
        pacman->notify_game_result(this->result.won);
        return this->result;
    }
};

#endif
