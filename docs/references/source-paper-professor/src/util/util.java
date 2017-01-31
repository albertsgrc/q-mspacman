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
 *  ENVIRONMENT
 *  This class holds several mathematical functions used by several classes.
 * 
 */

package util;

import environment.Environment;
import global.Globals;

public class util {

    public static double movefloatCorrection(boolean a2b, boolean b2a, double movefloat_a, double movefloat_b) {
        // returns a distance correcting for movefloat
        // In case this function is used to find a pill, use movefloat_b = 1

        double distance = 0;

        if (a2b) {
            distance += 1 - movefloat_a;
        } else {
            distance -= 1 - movefloat_a;
        }

        if (b2a) {
            distance += 1 - movefloat_b;
        } else {
            distance -= 1 - movefloat_b;
        }

        if (distance < 0) {
            distance = 0;
        }
        return distance;
    }

    public static boolean towardsObject(Environment Game, short x, short y) {
        short PacManX1 = (short) Game.getPacMan().getX1();
        short PacManY1 = (short) Game.getPacMan().getY1();
        short PacManX2 = (short) Game.getPacMan().getX2();
        short PacManY2 = (short) Game.getPacMan().getY2();

        // determine if Ms. Pac-Man is moving towards object
        if (Game.distance(PacManX1, PacManY1, x, y) < Game.distance(PacManX2, PacManY2, x, y)) {
            return true;
        }
        return false;
    }

    // Function to calculate the number of active input algorithms
    public static int calculateActiveAlgorithms() {
        int algorithm = 0;
        if (Globals.enableActionInput == true) {
            algorithm++;
        }
        if (Globals.enablePillInput == true) {
            algorithm++;
        }
        if (Globals.enableGhostDistanceInput == true) {
            algorithm++;
        }
        if (Globals.enableGhostDirectionInput == true) {
            algorithm++;
        }
        if (Globals.enableGhostAfraidInput == true) {
            algorithm++;
        }
        if (Globals.enableEntrapmentInput == true) {
            algorithm++;
        }
        if (Globals.enablePowerPillInput == true) {
            algorithm++;
        }
        if (Globals.enablePillsEatenInput == true) {
            algorithm++;
        }
        if (Globals.enablePowerPillDurationInput == true) {
            algorithm++;
        }
        return algorithm;
    }

    // Function that returns the reverse of an action
    public static short reverseActionFor(short action) {
        short reverseAction = action;
        switch (action) {
            case Globals.ACTION_LEFT:
                reverseAction = Globals.ACTION_RIGHT;
                break;
            case Globals.ACTION_RIGHT:
                reverseAction = Globals.ACTION_LEFT;
                break;
            case Globals.ACTION_UP:
                reverseAction = Globals.ACTION_DOWN;
                break;
            case Globals.ACTION_DOWN:
                reverseAction = Globals.ACTION_UP;
                break;
        }
        return reverseAction;
    }
}
