# q-mspacman

Implementation of a Ms. Pacman game simulation and Q-learning AI agent in C++

It was programmed entirely from scratch, following guidelines from the following
papers:

- L. Bom *et al*, [Reinforcement Learning to Train Ms. Pac-Man Using Higher-order Action-relative Inputs](https://repositories.lib.utexas.edu/bitstream/handle/2152/25046/SCHRUM-DISSERTATION-2014.pdf?sequence=1&isAllowed=y), University of Groningen, Groningen, The Netherlands
- D. Ballard *et al*, [Evolving Multimodal Behavior Through Modular Multiobjective Neuroevolution](https://repositories.lib.utexas.edu/bitstream/handle/2152/25046/SCHRUM-DISSERTATION-2014.pdf?sequence=1&isAllowed=y), University of Texas, Austin, USA
- D. B. Ortega, [Machine Learning Applied to Pac-Man](http://upcommons.upc.edu/bitstream/handle/2099.1/26448/108745.pdf?sequence=1), Barcelona School of Informatics, UPC, Barcelona, Spain
- M. H van der Ree and M. A Wiering, [Reinforcement Learning in the Game of Othello: Learning Against a Fixed Opponent and Learning from Self-Play](https://www.researchgate.net/publication/236645828_Reinforcement_Learning_in_the_Game_of_Othello_Learning_Against_a_Fixed_Opponent_and_Learning_from_Self-Play), University of Groningen, Groningen, The Netherlands

And with the help and supervision of professor Marco A. Wiering from the
University of Groningen.

Disclaimer: Only tested on **Unix** platforms

## Download and installation instructions

1. git clone https://github.com/albertsgrc/q-mspacman.git
2. cd q-mspacman/src
3. make

In case you want to execute the scripts provided inside the `src/scripts` folder,
you should also install [coffeescript](http://coffeescript.org/) via [npm](https://www.npmjs.com/).

Node, together with npm, can be downloaded and installed from the following link:

https://nodejs.org/en/download/

Once node is installed, run `npm install -g coffee-script` (may need privileges)
in order to install the coffee-script interpreter. A script named `script.coffee`
can be run with the command `coffee script.coffee`.

## Run instructions

The generated executable is called `pacman`. There are several program arguments,
which are specified as in the following example command:

`./pacman argname1=value1 argname2=value2` (etc.)

Every argument has a default value. For the full list of arguments, its meanings
and default values refer to the source file `arguments.hh`.

In order to train a neural network with the default maze `originalClassic.lay`,
one can simply run `./pacman`. The resulting neural network is saved in the file
`data/neural-networks/nn<id>.txt`. The id is generated automatically.

Now, in order to visualize the reinforcement learning agent game play, one can
run `./pacman pacman_ai_agent=nn<id> plays=1`. Here `<id>` is the id of the
neural network that was generated, which can be obtained from the corresponding
file name. To ease the process, one can use `last-nn.coffee` script,
which outputs the name of the last generated neural network. The command would look like:
`./pacman pacman_ai_agent=$(coffee scripts/last-nn.coffee) plays=1`
