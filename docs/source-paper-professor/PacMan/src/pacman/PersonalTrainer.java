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
 *  PERSONALTRAINER
 *  This class handles all training regemins, if any were set-up.
 *
 */

package pacman;

import global.Globals;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

public class PersonalTrainer {

    public void PersonalTrainer() {
    }
    private HashMap originalParameters = new HashMap();
    private ArrayList<trainingSession> trainingScheme = new ArrayList();
    private int totalTraining = 0;
    private int curDone = 0;
    private int trainingSwitch = 0;
    private boolean theEnd = false;

    public class trainingSession {

        private int gameLength = 0;
        private HashMap trainParameters = new HashMap();

        public void setParameter(String var, Object objectval) {
            if (curDone == 0) {
                originalParameters.put(var, Globals.getGlobal(var));
                trainParameters.put(var, String.valueOf(objectval));
            } else {
                System.err.println("Trying to set a trainingParameter called '" + var + "' after training has already started!");
                return;
            }
        }
    }

    public trainingSession newTraining(int gameLength) {
        trainingSession newSession = new trainingSession();
        newSession.gameLength = gameLength;

        trainingScheme.add(newSession);
        totalTraining += newSession.gameLength;

        return newSession;
    }

    public boolean getTraining(int gamesDone) {
        curDone = gamesDone + 1;

        if (theEnd == true || trainingScheme.size() == 0) {
            return false;
        }

        if (curDone == totalTraining || curDone == trainingSwitch) {
            if (originalParameters.size() > 0) {

                if (curDone == totalTraining) {
                    System.out.println("\n   End of training\n   Falling back to original Globals.");
                }

                Iterator it = originalParameters.entrySet().iterator();
                while (it.hasNext()) {
                    Map.Entry pairs = (Map.Entry) it.next();

                    if (curDone == totalTraining) {
                        System.out.println("   - " + pairs.getKey() + "  =  " + pairs.getValue());
                    }

                    Globals.setGlobal((String) pairs.getKey(), pairs.getValue());
                }

                if (curDone == totalTraining) {
                    System.out.println("");
                    theEnd = true;
                    return false;
                }
            }
        }
        if (curDone == trainingSwitch) {
            trainingSession curSession = trainingScheme.get(0);

            System.out.println("\n   Started training regmin with new Globals");
            Iterator it = curSession.trainParameters.entrySet().iterator();
            while (it.hasNext()) {
                Map.Entry pairs = (Map.Entry) it.next();
                Globals.setGlobal((String) pairs.getKey(), (String) pairs.getValue());
                System.out.println("   - " + pairs.getKey() + "  =  " + pairs.getValue());
                it.remove(); // avoids a ConcurrentModificationException
            }

            trainingSwitch += curSession.gameLength;
            trainingScheme.remove(0);
            System.out.println("");

            return false;

        }

        return false;
    }
}
