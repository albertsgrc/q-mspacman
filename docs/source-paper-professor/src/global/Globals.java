/*
 *  Ms. Pac-Man Framework
 *
 *  Created by Luuk Bom & Ruud Henken under supervision by Marco Wiering,
 *  Department of Artificial Intelligence, University of Groningen.
 *
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 *  GLOBALS
 *  This file contains all global settings, which can be used to alter 
 *  or tweak all aspects of the simulation, the learning process and the 
 *  user interface.
 *
 */


package global;

import java.lang.reflect.Field;
import pacman.PersonalTrainer;

public class Globals {
    // INITIALIZATION (Do not remove or alter)
    public static PersonalTrainer trainingData = new PersonalTrainer();
    private static PersonalTrainer.trainingSession newSession;



//**** CONSTANTS (Only change if necessary)

   // Action states
    public static final short ACTION_LEFT    = 0;
    public static final short ACTION_RIGHT   = 1;
    public static final short ACTION_UP      = 2;
    public static final short ACTION_DOWN    = 3;

    // Collision states
    public static short COLLISION_NO          = 0;
    public static short COLLISION_FATAL       = 1;
    public static short COLLISION_ALLOWED     = 2;

    // Ghost states
    public static final short GHOST_RANDOM   = 0;
    public static final short GHOST_CHASE    = 1;
    public static final short GHOST_SCATTER  = 2;
    public static final short GHOST_AFRAID   = 3;

    // Object types
    public static final short MAZE_PATH = 0;
    public static final short MAZE_WALL = 1;
    public static final short MAZE_PILL = 2;
    public static final short MAZE_POWER_PILL = 3;
    public static final short MAZE_PACMAN = 4;
    public static final short MAZE_GHOST = 5;
    public static final short MAZE_FRIGHT_GHOST = 6;
    public static final short MAZE_GHOST_FIRST_STOP = 7;
    public static final short MAZE_GHOST_SPAWN_AREA = 8;
    public static final short MAZE_GHOST_SPAWN_POINT = 9;

    // Networks belonging to different behaviors of Ms. Pac-Man
    public static final int NETWORK_SINGLE = 0;    // Single network
    public static final int NETWORK_ACTION = 1;    // Network per action

    // Learning rules
    public static final short Q           = 0;     // Q-Learning
    public static final short QV          = 1;     // Q(v)-Learning
    public static final short SARSA       = 2;     // SARSA


//**** GAME SETTINGS

    // Should we load an existing neural network from a file?
    // If so, enter the filename below. If not, leave empty.
    // EXAMPLE: "NN_20120614_23.36.11.txt"
    public static String loadFile = "";

    // Maximum path length.
    public static short maxVal = 54;

    // Do you want to alter variables in this file throughout the training
    // process, as scripted in the function trainingRegimen() of this file?
    // This will override the Globals for a certain amount of games,
    // so that targeted training can take place.
    // Afterwards the Globals will be reverted to those in this file and 
    // training will continue.
    public static boolean enableTraining = true;

    // State the training regimen for Ms. Pac-Man below.
    // This will only work if the above variable is set to true.
    public static void trainingRegimen() {
        // SYNTAX:
        // newSession = trainingDate.newTraining(   length in epochs   )
        // newSession.setParameter("  some variable  ", "  some value  ");
        //
        // - Always state the global variable name as a string!
        //   (surrounded by quotes)
        //
        // EXAMPLE:
        //
        // newSession = trainingData.newTraining(100);
        // newSession.setParameter("pacmanSpeed", 1);
        // newSession.setParameter("activeNet", NETWORK_HUNT);
        // newSession.setParameter("rewardPowerPill", -5);



        newSession = trainingData.newTraining(500);
        newSession.setParameter("diminishLearning",1.0);
        newSession.setParameter("boldExploration",true);
        newSession.setParameter("explorationTime",1);
        
        newSession = trainingData.newTraining(2000);
        newSession.setParameter("diminishLearning",1.0);
        newSession.setParameter("boldExploration",true);
        newSession.setParameter("gradualExploration",true);
        newSession.setParameter("explorationTime",1);
        newSession.setParameter("explorationTippingpoint",500);
        newSession.setParameter("explorationLowest",2500);
        newSession.setParameter("explorationMinimum",0.3);

        newSession = trainingData.newTraining(2000);
        newSession.setParameter("diminishLearning",1.0);
        newSession.setParameter("boldExploration",true);
        newSession.setParameter("gradualExploration",true);
        newSession.setParameter("explorationTime",0.3);
        newSession.setParameter("explorationTippingpoint",2500);
        newSession.setParameter("explorationLowest",4500);
        newSession.setParameter("explorationMinimum",0);
        
    }

    // Should the simulation stop after a specific number of games?
    public static boolean limitGames = true;
    public static int totalGames = 20;
    
    // The state ghosts should spawn in.
    public static short startingGhostState = GHOST_SCATTER;

    // Moving speed for agent (in distance traveled per epoch)
    // Speed for ghosts are defined as relative to the speed below.
    public static double pacmanSpeed = 0.5;

    // Time resolution for offering input to neural network,
    // in epochs per second.
    // Only relevant when visualisation is turned on.
    public static int timeResolution = 12;

    // How long should a power pill last, in distance traveled by Ms. Pac-man?
    public static int durationPowerpill = 55;

    // With what factor should we multiply ghost speed when they are afraid?
    public static double effectPowerpill = 0.55;

    // With what factor should we multiply ghost speed when they are eaten,
    // and traveling back to spawn point?
    public static double effectEaten = 1.3;

    // With what factor should we multiply Ms. Pac-Man's speed
    // when she eats a pill?
    public static double effectPill = 0.85;

    // Should ghosts always be in a certain state?
    public static boolean alwaysEnabled = false;
    public static short alwaysState = GHOST_AFRAID;

    // Which neural networks should be used?
    public static int activeNet = NETWORK_SINGLE;

    // How many hidden neurons should be used?
    public static int hiddenNeurons = 50;
    
    // Should training be done after the entire level has been played?
    // This will only train on games that were lost, and the learning
    // rate will decline a little at every game won.
    public static boolean trainingAfterTheFact = false;

//**** ALGORITHMS

    public static boolean enableActionInput =             true;
    public static boolean enablePillInput =               true;
    public static boolean enableGhostDistanceInput =      true;
    public static boolean enableGhostDirectionInput =     false;
    public static boolean enableGhostAfraidInput =        true;
    public static boolean enableEntrapmentInput =         true;
    public static boolean enablePowerPillInput =          false;
    public static boolean enablePillsEatenInput =         true;
    public static boolean enablePowerPillDurationInput =  true;

    public static int numAlghorithms = 9;

    
    // When looking for pills, what is the range around Ms. Pac-man that should be
    // explored using breadth-first search, before switching to the A*-algorithm?
    // If set to -1, breadth-first search will always be used.
    // If set to 0, the A*-algorithm will always be used.
    public static int pillRange = 10;

    // When looking for escape routes, how many vertices should Ms. Pac-man
    // be able to travel for that route to be considered safe?
    public static int maxVertex = 3;

    // When no such routes are available for all directions, we'll look at
    // routes with one less vertex, and so on. What is the least amount
    // of vertices a safe route should have in this case?
    public static int minVertex = 1;

    // Should we invert the input given by the entrapmentfinder, so that
    // a highly dangerous direction will recieve a high value?
    public static boolean invertEntrapment = true;

    // Concerning the GhostDistanceInput:
    // How strongly should the distance between Ms. Pac-Man and the closest vertex be counted?
    public static double shortDistanceFactor = -0.8;

    // Should Ms. Pac-man be considered safe from a ghost, if the distance
    // between them is greater than a certain value?
    public static boolean safeEnabled = true;

    // After what distance should Ms. Pac-man be considered absolutely safe
    // in calculating GhostDistanceInput?
    // (Requires above value to be set to true)
    public static int safeDistanceGeneral = 20;

    // After what distance should Ms. Pac-man be considered absolutely safe?
    // in calculating GhostDirectionInput?
    // (Requires above value to be set to true)
    public static int safeDistanceDirection = 35;

    // If Ms. Pac-man and a ghost are both heading for the same vertex,
    // we will consider travelling to that vertex safe if Ms. Pac-man
    // can get there whilst leaving a specifc amount of distance between
    // her and the ghost, as set here:
    public static int safeHeadstart = 1;

    // Should the direction that offers the shortest path to a pill recieve
    // a small bump in PillInput-value, when its result input value falls below the bumpLimit?
    public static boolean bumpBestPill = false;

    // Below what inputvalue should we start bumping a little?
    public static double bumpLimit = 0.40;

    // With what factor between 0 and 1 should we bump?
    // Where 0 is no bumping, and 1 is always bump to the bumpLimit.
    public static double bumpSize = 0.6;

    // Should the directions that do not offer the shortest path to a pill recieve
    // a small decline in PillInput-value, when its result input value fall above the declineLimit?
    public static boolean declineWorstPills = false;

    // Above what inputvalue should we start declining a little?
    public static double declineLimit = 0.85;

    // What what factor between 0 and 1 should we bump?
    // Where 0 is no decline, and 1 is always decline to the declineLimit.
    public static double declineSize = 0.3;

    // Should the PillInput-values be normalized by calculating the square root?
    public static boolean normalizePill = true;



//**** VISUALISATION & STATISTICS

    // Calculate the running average over the following number of games.
    public static int runningAverage = 125;

    // Choose whether to show the visualisation.
    // If set to true, the game will run at normal playing speed.
    // If set to false, the game will run at maximum speed.
    public static boolean showVisualisation = false;

    // Should empty spaces be marked in the visualisation, so that they are easily countable?
    public static boolean markSpaces = false;

    // Width and height in pixels of a single maze position when displayed
    public static int itemWidth = 16;
    public static int itemHeight = 16;

    // Choose whether to show a chart of input values.
    // Visualisation must be turned on as well, for the charter to show!
    public static boolean showCharter = true;

    // Choose whether to show a graph concerning the learning rate.
    public static boolean showRateGraph = true;

    // Choose whether the rateGraph should plot the absolute rewards
    public static boolean plotAbsoluteRewards = false;

    // Choose whether the rateGraph should plot the lifetime
    public static boolean plotLifeTime = false;



    // Choose whether to show a graph concerning the Q-values.
    // You can leave this on with visualisation turned off,
    // but then:
    // - Plotting the Q-values will form a bottleneck on performance.
    // - Many errors will be thrown for trying to add values to the graph,
    //   while it is still working on drawing the previous data points.
    public static boolean showQGraph = false;

    // Choose whether to show a graph concerning the computational load.
    // Warning: plotting the computational load, will obviously increase computional load!
    //          So don't use it, if you don't need it.
    public static boolean showLoadGraph = false;

    // Choose whether to show a graph concerning the network active at time of death.
    public static boolean showRIPGraph = false;

    // This value decides after how many epochs a new data point will be added to the graph.
    // Raise this value to decrease load on CPU.
    public static int graphResolution = 50;



//**** TRAINING PARAMETERS

    // Set to Q for Q-Learning, QV for QV-Learning or SARSA.
    public static short learningRule = QV;

    // Set the rewards for Ms. Pac-man's actions
    public static short rewardPill = 12;
    public static short rewardPowerPill = 0;
    public static short rewardFruit = 0;
    public static short rewardStep = (short) (-10 * pacmanSpeed);
    public static short rewardLose = -375; //-350
    public static short rewardWin = 50;
    public static short rewardGhost = 40;  //-20
    public static short rewardReverse = -6;

    // Variables concerning initialization and training of neural network
    public static double randMin = -0.3;
    public static double randMax = 0.3;
    public static double learningRate = 0.0005;
    
    // Diminish learning rate on win? Set to 1.0 if not.
    public static double diminishLearning = 0.9995;

    // Gradually lower the learningRate?
    public static boolean gradualLearning = false;

    // If so, how long till we start lowering learningRate?
    public static int learningTippingpoint = 9000;

    // If so, how long till the learningRate has dropped to its lowest point?
    public static int learningLowest = 4000;

    // Value of learningRate after lowering
    public static double learningMinimum = 0;


    // Value for discountFactor
    public static double discountFactor = 0.95;



//**** EXPLORATION

    // Enable this to disallow reversing on the current path when exploration is triggered
    public static boolean boldExploration = false;
    
    // Percentage of time spend on exploration
    public static double explorationTime = 0.00;

    // Gradually lower time spend on exploration?
    public static boolean gradualExploration = false;

    // If so, how long till we start lowering exploration?
    public static int explorationTippingpoint = 10;

    // If so, how long till there is no exploration whatsoever?
    public static int explorationLowest = 10;

    // Percentage of time spend on exploration
    public static double explorationMinimum = 0.05;



    //////////////////////////////////////////////////////////////////////////////////////////

    public static String setGlobal(String var, Object objectval) {

        String val = objectval.toString();

        // Add any variables that should be changed during training below.
        // Be very carefull which ones you add, as some may be cached in other classes
        // or used in initialization of the environment.
        // This may make the simulation of the game unreliable.

        String oldValue = getGlobal(var);

        if ("durationPowerpill".equals(var)) {
            durationPowerpill = Integer.parseInt(val);
            return oldValue;
        }
        if ("effectPowerpill".equals(var)) {
            effectPowerpill = Integer.parseInt(val);
            return oldValue;
        }
        if ("effectEaten".equals(var)) {
            effectEaten = Integer.parseInt(val);
            return oldValue;
        }
        if ("alwaysEnabled".equals(var)) {
            alwaysEnabled = Boolean.parseBoolean(val);
            return oldValue;
        }
        if ("alwaysState".equals(var)) {
            alwaysState = Short.parseShort(val);
            return oldValue;
        }
        if ("activeNet".equals(var)) {
            activeNet = Integer.parseInt(val);
            return oldValue;
        }
        if ("maxVertex".equals(var)) {
            maxVertex = Integer.parseInt(val);
            return oldValue;
        }
        if ("rewardPill".equals(var)) {
            rewardPill = Short.parseShort(val);
            return oldValue;
        }
        if ("rewardPowerPill".equals(var)) {
            rewardPowerPill = Short.parseShort(val);
            return oldValue;
        }
        if ("rewardFruit".equals(var)) {
            rewardFruit = Short.parseShort(val);
            return oldValue;
        }
        if ("rewardStep".equals(var)) {
            rewardStep = Short.parseShort(val);
            return oldValue;
        }
        if ("rewardLose".equals(var)) {
            rewardLose = Short.parseShort(val);
            return oldValue;
        }
        if ("rewardWin".equals(var)) {
            rewardWin = Short.parseShort(val);
            return oldValue;
        }
        if ("rewardGhost".equals(var)) {
            rewardGhost = Short.parseShort(val);
            return oldValue;
        }
        if ("rewardReverse".equals(var)) {
            rewardReverse = Short.parseShort(val);
            return oldValue;
        }
        if ("discountFactor".equals(var)) {
            discountFactor = Integer.parseInt(val);
            return oldValue;
        }
        if ("explorationTime".equals(var)) {
            explorationTime = Double.parseDouble(val);
            return oldValue;
        }
        if ("boldExploration".equals(var)) {
            boldExploration = Boolean.parseBoolean(val);
            return oldValue;
        }
        if ("gradualExploration".equals(var)) {
            gradualExploration = Boolean.parseBoolean(val);
            return oldValue;
        }
        if ("explorationTippingpoint".equals(var)) {
            explorationTippingpoint = Integer.parseInt(val);
            return oldValue;
        }
        if ("explorationLowest".equals(var)) {
            explorationLowest = Integer.parseInt(val);
            return oldValue;
        }
        if ("explorationMinimum".equals(var)) {
            explorationMinimum = Double.parseDouble(val);
            return oldValue;
        }
        if ("diminishLearning".equals(var)) {
            diminishLearning = Double.parseDouble(val);
            return oldValue;
        }
        if ("gradualLearning".equals(var)) {
            gradualLearning = Boolean.parseBoolean(val);
            return oldValue;
        }
        if ("learningTippingpoint".equals(var)) {
            learningTippingpoint = Integer.parseInt(val);
            return oldValue;
        }
        if ("learningLowest".equals(var)) {
            learningLowest = Integer.parseInt(val);
            return oldValue;
        }
        if ("learningMinimum".equals(var)) {
            learningMinimum = Double.parseDouble(val);
            return oldValue;
        }
        if ("learningRate".equals(var)) {
            learningRate = Double.parseDouble(val);
            return oldValue;
        }
        if ("trainingAfterTheFact".equals(var)) {
            trainingAfterTheFact = Boolean.parseBoolean(val);
            return oldValue;
        }

        System.err.println("- Trying to write a non-existing or non-changable global variable for training");
        return oldValue;

    }

    public static String getGlobal(String var) {

        // Add any variables that should be changed during training below.

        if ("durationPowerpill".equals(var)) {
            return String.valueOf(durationPowerpill);
        }
        if ("effectPowerpill".equals(var)) {
            return String.valueOf(effectPowerpill);
        }
        if ("effectEaten".equals(var)) {
            return String.valueOf(effectEaten);
        }
        if ("alwaysEnabled".equals(var)) {
            return String.valueOf(alwaysEnabled);
        }
        if ("alwaysState".equals(var)) {
            return String.valueOf(alwaysState);
        }
        if ("activeNet".equals(var)) {
            return String.valueOf(activeNet);
        }
        if ("maxVertex".equals(var)) {
            return String.valueOf(maxVertex);
        }
        if ("rewardPill".equals(var)) {
            return String.valueOf(rewardPill);
        }
        if ("rewardPowerPill".equals(var)) {
            return String.valueOf(rewardPowerPill);
        }
        if ("rewardFruit".equals(var)) {
            return String.valueOf(rewardFruit);
        }
        if ("rewardStep".equals(var)) {
            return String.valueOf(rewardStep);
        }
        if ("rewardLose".equals(var)) {
            return String.valueOf(rewardLose);
        }
        if ("rewardReverse".equals(var)) {
            return String.valueOf(rewardReverse);
        }
        if ("rewardGhost".equals(var)) {
            return String.valueOf(rewardGhost);
        }
        if ("rewardWin".equals(var)) {
            return String.valueOf(rewardWin);
        }
        if ("discountFactor".equals(var)) {
            return String.valueOf(rewardGhost);
        }
        if ("explorationTime".equals(var)) {
            return String.valueOf(explorationTime);
        }
        if ("gradualExploration".equals(var)) {
            return String.valueOf(gradualExploration);
        }
        if ("boldExploration".equals(var)) {
            return String.valueOf(boldExploration);
        }
        if ("explorationTippingpoint".equals(var)) {
            return String.valueOf(explorationTippingpoint);
        }
        if ("explorationLowest".equals(var)) {
            return String.valueOf(explorationLowest);
        }
        if ("explorationMinimum".equals(var)) {
            return String.valueOf(explorationMinimum);
        }
        if ("gradualLearning".equals(var)) {
            return String.valueOf(gradualLearning);
        }
        if ("diminishLearning".equals(var)) {
            return String.valueOf(diminishLearning);
        }
        if ("learningTippingpoint".equals(var)) {
            return String.valueOf(learningTippingpoint);
        }
        if ("learningLowest".equals(var)) {
            return String.valueOf(learningLowest);
        }
        if ("learningMinimum".equals(var)) {
            return String.valueOf(learningMinimum);
        }
        if ("learningRate".equals(var)) {
            return String.valueOf(learningMinimum);
        }
        if ("trainingAfterTheFact".equals(var)) {
            return String.valueOf(trainingAfterTheFact);
        }

        System.err.println("- Trying to read a non-existing or non-changable global variable for training");
        return null;

    }

    public static void checkValidity() {
        if (pacmanSpeed > 1) {
            System.err.println("A pacmanSpeed larger than 1 will cause Ms. Pac-Man to make illegal moves.");
        }
    }

    public static void parseCommandLine(String[] args) {

    }
}
