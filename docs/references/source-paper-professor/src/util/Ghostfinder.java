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
 *  GHOSTFINDER
 *  This class implements the ghost finder as an input algorithm.
 * 
 */

package util;

import environment.Environment;
import environment.Vertex;
import global.Globals;
import pacman.Agent;

public class Ghostfinder {

    // Storage variables for the results of the search for ghost
    public double toLeft;
    public double toRight;
    public double toUp;
    public double toDown;
    public double dirLeft;
    public double dirRight;
    public double dirUp;
    public double dirDown;
    public double minDir;
    
    private final short maxVal = Globals.maxVal;
    private double[] distanceLeft;
    private double[] distanceRight;
    private double[] distanceUp;
    private double[] distanceDown;
    private double[] directionLeft;
    private double[] directionRight;
    private double[] directionUp;
    private double[] directionDown;
    

    public Ghostfinder() {
    }
    
    // Function that updates the global variables to reflect the ghosts given to the function.
    // findGhosts should be called first.
    public void calculateDirection(Agent[] ghost) {
        dirLeft = 0;
        dirRight = 0;
        dirUp = 0;
        dirDown = 0;
        
        int ghostCount = ghost.length;
        
        double distance;
        
        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < ghostCount; i++) {
                switch (j) {
                    case Globals.ACTION_DOWN:
                        distance = directionDown[ghost[i].getId()];
                        if (distance > dirDown) {
                            dirDown = distance;
                        }
                        break;
                    case Globals.ACTION_LEFT:
                        distance = directionLeft[ghost[i].getId()];
                        if (distance > dirLeft) {
                            dirLeft = distance;
                        }
                        break;
                    case Globals.ACTION_RIGHT:
                        distance = directionRight[ghost[i].getId()];
                        if (distance > dirRight) {
                            dirRight = distance;
                        }
                        break;
                    case Globals.ACTION_UP:
                        distance = directionUp[ghost[i].getId()];
                        if (distance > dirUp) {
                            dirUp = distance;
                        }
                        break;

                }
            }
        }
           
    }
    
    // Function that updates the global variables to reflect the ghosts given to the function.
    // findGhosts should be called first.
    public void calculateDistance(Agent[] ghost) {
        toLeft = maxVal;
        toRight = maxVal;
        toUp = maxVal;
        toDown = maxVal;
        
        int ghostCount = ghost.length;
        
        double maxDistance;

        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < ghostCount; i++) {
                switch (j) {
                    case Globals.ACTION_DOWN:
                        if (toDown == maxVal || distanceDown[ghost[i].getId()] < toDown) {
                            toDown = distanceDown[ghost[i].getId()];
                        }
                        break;

                    case Globals.ACTION_LEFT:
                        if (toLeft == maxVal || distanceLeft[ghost[i].getId()] < toLeft) {
                            toLeft = distanceLeft[ghost[i].getId()];
                        }
                        break;

                    case Globals.ACTION_RIGHT:
                        if (toRight == maxVal || distanceRight[ghost[i].getId()] < toRight) {
                            toRight = distanceRight[ghost[i].getId()];
                        }
                        break;

                    case Globals.ACTION_UP:
                        if (toUp == maxVal || distanceUp[ghost[i].getId()] < toUp) {
                            toUp = distanceUp[ghost[i].getId()];
                        }
                        break;
                }
            }
        }
        
        maxDistance = maxVal;
        if (Globals.safeEnabled == true) {
            maxDistance = Globals.safeDistanceGeneral;
            if (toLeft > Globals.safeDistanceGeneral) {
                toLeft = Globals.safeDistanceGeneral;
            }
            if (toRight > Globals.safeDistanceGeneral) {
                toRight = Globals.safeDistanceGeneral;
            }
            if (toUp > Globals.safeDistanceGeneral) {
                toUp = Globals.safeDistanceGeneral;
            }
            if (toDown > Globals.safeDistanceGeneral) {
                toDown = Globals.safeDistanceGeneral;
            }
        }
        
        // Normalize distance
        toLeft = (maxDistance - toLeft) / maxDistance;
        toRight = (maxDistance - toRight) / maxDistance;
        toUp = (maxDistance - toUp) / maxDistance;
        toDown = (maxDistance - toDown) / maxDistance;
        
        if (toLeft < 0) toLeft = 0;
        if (toRight < 0) toRight = 0;
        if (toUp < 0) toUp = 0;
        if (toDown < 0) toDown = 0;
    }

    // Function that resets all global variables and (re)calculates the distances to ghosts
    public void findGhosts(Environment Game) {
        short PacManX1 = (short) Game.getPacMan().getX1();
        short PacManY1 = (short) Game.getPacMan().getY1();
        short PacManX2 = (short) Game.getPacMan().getX2();
        short PacManY2 = (short) Game.getPacMan().getY2();        
        double PacManMovefloat = Game.getPacMan().getMovefloat();

        short ghostX1, ghostY1, ghostX2, ghostY2;

        int ghostCount = Game.getGhosts().length;
        distanceLeft = new double[ghostCount];
        distanceRight = new double[ghostCount];
        distanceUp = new double[ghostCount];
        distanceDown = new double[ghostCount];
        directionLeft = new double[ghostCount];
        directionRight = new double[ghostCount];
        directionUp = new double[ghostCount];
        directionDown = new double[ghostCount];
        
        for (int i=0; i<ghostCount; i++) {
            distanceLeft[i] = maxVal;
            distanceRight[i] = maxVal;
            distanceUp[i] = maxVal;
            distanceDown[i] = maxVal;
            directionLeft[i] = 0;
            directionRight[i] = 0;
            directionUp[i] = 0;
            directionDown[i] = 0;
        }
        
        toLeft = maxVal;
        toRight = maxVal;
        toUp = maxVal;
        toDown = maxVal;

        dirLeft = 0;
        dirRight = 0;
        dirUp = 0;
        dirDown = 0;

        minDir = maxVal;
        
        // Establish which vertices surround Ms. Pac-man
        // First find the enclosing vertices
        int[] pacmanVertices = Game.findEnclosingVertices(PacManX1, PacManY1);
        short[] pacmanDirectionality;
        
        // If Ms. Pac-man is on a vertex, there are no enclosing vertices;
        int baseVertex[];
        if (pacmanVertices.length <= 1) {
            // If that is the case, look for neighbouring vertices
            int initialVertex = pacmanVertices[0];
            
            short[][] newVertices = Game.findNeighborVertices(Game.vertices.get(pacmanVertices[0]));
            pacmanVertices = new int[newVertices.length];
            pacmanDirectionality = new short[newVertices.length];
            baseVertex = new int[newVertices.length];
            
            // Then loop through these vertices and find their ID and initial direction.
            for(int i=0; i<newVertices.length; i++) {
                baseVertex[i] = initialVertex;
                pacmanVertices[i] = newVertices[i][0];
                pacmanDirectionality[i] = newVertices[i][2];
            }
        } else {
            // If Ms. Pac-man wasn't on a vertex, we now have two vertices of which
            // we do not know the initial direction.
            pacmanDirectionality = new short[pacmanVertices.length];
            baseVertex = new int[pacmanVertices.length];
            
            // Loop through these vertices
            for(int i=0; i<pacmanVertices.length; i++) {
                baseVertex[i] = pacmanVertices[pacmanVertices.length - 1 - i];
                
                short[][] newVertices = Game.findNeighborVertices(Game.vertices.get(pacmanVertices[i]));
                for(int j=0; j<newVertices.length; j++) {
                    if (newVertices[j][0] == pacmanVertices[pacmanVertices.length - 1 - i]) {
                        pacmanDirectionality[i] = Game.oppositeDirection(newVertices[j][2]);
                    }
                }
            }
        }
        
        // Loop through the vertices found, and if they are part of a hallway, 
        // find the vertex at the end of the hallway
        for (int i=0; i<pacmanVertices.length; i++) {
            short[][] neighbourVertices;
            int previousVertex = pacmanVertices[i];
            
            do {
                neighbourVertices = Game.findNeighborVertices(Game.vertices.get(previousVertex));
                if (neighbourVertices.length <= 2) {
                    findNewVertex:
                    for (short[] neighbourVertex : neighbourVertices) {
                        if (neighbourVertex[0] != baseVertex[i]) {
                            baseVertex[i] = previousVertex;
                            previousVertex = neighbourVertex[0];
                            break findNewVertex;
                        }
                    }
                }
            } while (neighbourVertices.length <= 2);
            
            pacmanVertices[i] = previousVertex;
        }
        
               
        // worst case approach
        // determine shortest path size to each ghost
        for (Agent ghost : Game.getGhosts()) {
            ghostX1 = (short) ghost.getX1();
            ghostY1 = (short) ghost.getY1();
            ghostX2 = (short) ghost.getX2();
            ghostY2 = (short) ghost.getY2();
            
            //short[] result = Game.findAllPaths(PacManX1, PacManY1, ghostX1, ghostY1);
            double ghostMovefloat = ghost.getMovefloat();

            boolean ghost2pacman = false;
            boolean pacman2ghost = false;
            
            short[] lowestDistance = Game.findShortestPath(PacManX1, PacManY1, ghostX1, ghostY1, null);
            double correctedLowestDistance = lowestDistance[0];
            short lowestDirection = lowestDistance[1];
            boolean seperatedbyVertex = false;
            
            for (int i=0; i<pacmanVertices.length; i++) {
                Vertex currentGoal = Game.vertices.get(pacmanVertices[i]);
                short[] tempDistance = Game.findShortestPath(currentGoal.getX(), currentGoal.getY(), ghostX1, ghostY1, null);
                double correctedDistance = tempDistance[0] + Globals.shortDistanceFactor * Game.findShortestPath(currentGoal.getX(), currentGoal.getY(), PacManX1, PacManY1, null)[0];
                if (correctedDistance < lowestDistance[0]) {
                    correctedLowestDistance = correctedDistance;
                    lowestDirection = pacmanDirectionality[i];
                    seperatedbyVertex = true;
                    
                    // determine if ghost is moving towards Ms. Pac-Man.
                    if (Game.distance(ghostX1, ghostY1, currentGoal.getX(), currentGoal.getY()) < Game.distance(ghostX2, ghostY2, currentGoal.getX(), currentGoal.getY())) {
                        ghost2pacman = true;
                    }
                }
            }
            
            if (seperatedbyVertex == true ) {
                processGhostDistance(ghost.getId(), correctedLowestDistance, lowestDirection, ghost2pacman, ghostMovefloat);
            } else {
                processGhostDistance(ghost.getId(), 0, lowestDirection, true, 1);
            }

//
//            // determine if Ms. Pac-Man is moving towards ghost
//            if (Game.distance(PacManX1, PacManY1, ghostX1, ghostY1) < Game.distance(PacManX2, PacManY2, ghostX1, ghostY1)) {
//                pacman2ghost = true;
//            }
            
            short[] result = Game.findShortestPath(PacManX1, PacManY1, ghostX1, ghostY1, null);
            processGhostDirection(ghost.getId(), result[0], result[1], ghost2pacman, PacManMovefloat, ghostMovefloat);
        }
        
    }

    // Recursive function used to store paths that lead to ghosts
    private void processGhostDistance(int ghostid, double distance, short direction, boolean ghost2pacman, double ghostMovefloat) {
 
        // add movefloat 
//        if (pacman2ghost) {
//            distance += 1 - PacManMovefloat;
//        } else {
//            distance -= 1 - PacManMovefloat;
//        }
        
        if (ghost2pacman) {
            distance += 1 - ghostMovefloat;
        } else {
            distance -= 1 - ghostMovefloat;
        }
        
        if (distance < 0) {
            distance = 0;
        }
        
        switch (direction) {
            case Globals.ACTION_DOWN:
                distanceDown[ghostid] = distance;
                break;

            case Globals.ACTION_LEFT:
                distanceLeft[ghostid] = distance;
                break;

            case Globals.ACTION_RIGHT:
                distanceRight[ghostid] = distance;
                break;

            case Globals.ACTION_UP:
                distanceUp[ghostid] = distance;
                break;
        }
    }

    // Recursive function used to store paths that lead to ghosts
    private void processGhostDirection(int ghostid, double distance, short direction, boolean ghost2pacman, double PacManMovefloat, double ghostMovefloat) {
        double tmpDir = 0;
        int maxDistance;
        
//        distance -= 1 - PacManMovefloat;
//        
//        // ghost is moving towards x1
//        if (ghost2pacman) {
//            distance -= 1 - ghostMovefloat;
//        } else {
//            distance -= ghostMovefloat;
//        }

        maxDistance = maxVal;
        if (Globals.safeEnabled == true) {
            maxDistance = Globals.safeDistanceDirection;
            if (distance > Globals.safeDistanceDirection) {
                distance = Globals.safeDistanceDirection;
            }
        }
        if (distance < minDir) {
            minDir = distance;
        }

        tmpDir = Math.pow((maxDistance - distance) / maxDistance, ghost2pacman ? 2 : 3);
        
        switch (direction) {
            case Globals.ACTION_DOWN:
                directionDown[ghostid] = tmpDir;
                break;

            case Globals.ACTION_LEFT:
                directionLeft[ghostid] = tmpDir;
                break;

            case Globals.ACTION_RIGHT:
                directionRight[ghostid] = tmpDir;
                break;

            case Globals.ACTION_UP:
                directionUp[ghostid] = tmpDir;
                break;
        }
        
    }
}