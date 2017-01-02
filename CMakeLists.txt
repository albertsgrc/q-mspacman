cmake_minimum_required(VERSION 3.6)
project(Projecte)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/agents/ghosts/ghost_agent.hh
        src/agents/pacman/rl/neural_network.hh
        src/agents/pacman/rl/nn_pacman_agent.hh
        src/agents/pacman/rl/rl_pacman_agent.hh
        src/agents/pacman/rl/rl_pacman_agent_inputs.hh
        src/agents/pacman/input_pacman_agent.hh
        src/agents/pacman/pathfinding_pacman_agent.hh
        src/agents/pacman/random_pacman_agent.hh
        src/agents/agent.hh
        src/pathfinding/bfs.hh
        src/pathfinding/pathfinding.hh
        src/pathfinding/wsp.hh
        src/state/agents/agent_state.hh
        src/state/agents/ghost_state.hh
        src/state/direction.hh
        src/state/position.hh
        src/state/state.hh
        src/arguments.hh
        src/game.hh
        src/main.cc
        src/utils.hh)

add_executable(Projecte ${SOURCE_FILES})