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
 *  PILLFINDER
 *  This class implements the pill finder as an input algorithm.
 * 
 */

package util;

import global.Globals;
import util.util;
import environment.Environment;
import java.util.LinkedList;
import java.util.Queue;

public class Pillfinder {

    // Storage variables for the results of the search for pills
    public double toLeft = -1;
    public double toRight = -1;
    public double toUp = -1;
    public double toDown = -1;
    public double toLeftPower = -1;
    public double toRightPower = -1;
    public double toUpPower = -1;
    public double toDownPower = -1;
    
    public double totalInput = 0;
    // Contains the amount of maze locations that were looked up during breadth-first search
    public int looksNeeded = 0;
    // Contains whether A* was used
    public boolean astarUsed = false;
    private int foundDistance;
    
    private final short maxVal = Globals.maxVal;

    public Pillfinder() {
    }

    // Breath-first search
    // Will alter the public result variables,
    // and set them to -1 if no pills were in range.
    public void findPills(int xPos, int yPos, Environment e) {
        toLeft = -1;
        toUp = -1;
        toRight = -1;
        toDown = -1;
        astarUsed = false;
//
        // If we can go left, find the nearest pill on the left
        if (xPos > 0 && e.maze[xPos - 1][yPos] != Globals.MAZE_WALL) {
            Queue listFind = new LinkedList();
            instanceFind newInstance = new instanceFind(xPos - 1, yPos, Globals.ACTION_LEFT);
            listFind.offer(newInstance);
            while (!listFind.isEmpty()) {
                instanceFind o = (instanceFind) listFind.poll();
                listFind = rec_BFS(o.xPos, o.yPos, o.direction, o.offset, e, listFind);
                looksNeeded++;
            }
            toLeft = foundDistance;

            // If we are on the far left, try and see if we can teleport to the right,
            // and then find the nearest pill there.
        } else if (xPos == 0 && e.maze[e.mazeWidth - 1][yPos] != Globals.MAZE_WALL) {
            Queue listFind = new LinkedList();
            instanceFind newInstance = new instanceFind(e.mazeWidth - 1, yPos, Globals.ACTION_LEFT);
            listFind.offer(newInstance);
            while (!listFind.isEmpty()) {
                instanceFind o = (instanceFind) listFind.poll();
                listFind = rec_BFS(o.xPos, o.yPos, o.direction, o.offset, e, listFind);
                looksNeeded++;
            }
            toLeft = foundDistance;
        }
        if (yPos > 0 && e.maze[xPos][yPos - 1] != Globals.MAZE_WALL) {
            Queue listFind = new LinkedList();
            instanceFind newInstance = new instanceFind(xPos, yPos - 1, Globals.ACTION_UP);
            listFind.offer(newInstance);
            while (!listFind.isEmpty()) {
                instanceFind o = (instanceFind) listFind.poll();
                listFind = rec_BFS(o.xPos, o.yPos, o.direction, o.offset, e, listFind);
                looksNeeded++;
            }
            toUp = foundDistance;
        }
        if (xPos < e.mazeWidth - 1 && e.maze[xPos + 1][yPos] != Globals.MAZE_WALL) {
            Queue listFind = new LinkedList();
            instanceFind newInstance = new instanceFind(xPos + 1, yPos, Globals.ACTION_RIGHT);
            listFind.offer(newInstance);
            while (!listFind.isEmpty()) {
                instanceFind o = (instanceFind) listFind.poll();
                listFind = rec_BFS(o.xPos, o.yPos, o.direction, o.offset, e, listFind);
                looksNeeded++;
            }
            toRight = foundDistance;
        } else if (xPos == e.mazeWidth - 1 && e.maze[0][yPos] != Globals.MAZE_WALL) {
            Queue listFind = new LinkedList();
            instanceFind newInstance = new instanceFind(0, yPos, Globals.ACTION_RIGHT);
            listFind.offer(newInstance);
            while (!listFind.isEmpty()) {
                instanceFind o = (instanceFind) listFind.poll();
                listFind = rec_BFS(o.xPos, o.yPos, o.direction, o.offset, e, listFind);
                looksNeeded++;
            }
            toRight = foundDistance;
        }
        if (yPos < e.mazeHeight - 1 && e.maze[xPos][yPos + 1] != Globals.MAZE_WALL) {
            Queue listFind = new LinkedList();
            instanceFind newInstance = new instanceFind(xPos, yPos + 1, Globals.ACTION_DOWN);
            listFind.offer(newInstance);
            while (!listFind.isEmpty()) {
                instanceFind o = (instanceFind) listFind.poll();
                listFind = rec_BFS(o.xPos, o.yPos, o.direction, o.offset, e, listFind);
                looksNeeded++;
            }
            toDown = foundDistance;
        }

        short astar[];

        // If needed and allowed by settings, try using A*.
        if (toLeft == -1 && toRight == -1 && toUp == -1 && toDown == -1) {
            astarUsed = true;
        }

        if (astarUsed == true) {
            // Then for each pill currently in the maze...
            for (int y = 0; y < e.mazeHeight; y++) {
                for (int x = 0; x < e.mazeWidth; x++) {
                    if (e.maze[x][y] == Globals.MAZE_PILL || e.maze[x][y] == Globals.MAZE_POWER_PILL) {
                        // Perform A* to find the shortest path
                        astar = e.findAllPaths((short) xPos, (short) yPos, (short) x, (short) y);
                        for (int i = 0; i < 4; i++) {
                            if (astar[i] != -1) {
                                double correctedDistance = astar[i] + util.movefloatCorrection(util.towardsObject(e, (short) x, (short) y), false, e.getPacMan().getMovefloat(), 1);
                                // If this path is the shortest yet encountered,
                                // store it according to the starting move.
                                switch (i) {
                                    case Globals.ACTION_LEFT:
                                        if (toLeft == -1 || correctedDistance < toLeft) {
                                            toLeft = correctedDistance;
                                        }
                                        break;
                                    case Globals.ACTION_UP:
                                        if (toUp == -1 || correctedDistance < toUp) {
                                            toUp = correctedDistance;
                                        }
                                        break;
                                    case Globals.ACTION_DOWN:
                                        if (toDown == -1 || correctedDistance < toDown) {
                                            toDown = correctedDistance;
                                        }
                                        break;
                                    case Globals.ACTION_RIGHT:
                                        if (toRight == -1 || correctedDistance < toRight) {
                                            toRight = correctedDistance;
                                        }
                                        break;
                                }
                            }
                        }
                        if (e.maze[x][y] == Globals.MAZE_POWER_PILL) {
                            // Perform A* to find the shortest path
                            if (astar[Globals.ACTION_LEFT] != -1) {
                                double correctedDistance = astar[Globals.ACTION_LEFT] + util.movefloatCorrection(util.towardsObject(e, (short) x, (short) y), false, e.getPacMan().getMovefloat(), 1);
                                if (toLeftPower == -1 || correctedDistance < toLeftPower) {
                                    toLeftPower = correctedDistance;
                                }
                            }
                            if (astar[Globals.ACTION_UP] != -1) {
                                double correctedDistance = astar[Globals.ACTION_UP] + util.movefloatCorrection(util.towardsObject(e, (short) x, (short) y), false, e.getPacMan().getMovefloat(), 1);
                                if (toUpPower == -1 || correctedDistance < toUpPower) {
                                    toUpPower = correctedDistance;
                                }
                            }
                            if (astar[Globals.ACTION_DOWN] != -1) {
                                double correctedDistance = astar[Globals.ACTION_DOWN] + util.movefloatCorrection(util.towardsObject(e, (short) x, (short) y), false, e.getPacMan().getMovefloat(), 1);
                                if (toDownPower == -1 || correctedDistance < toDownPower) {
                                    toDownPower = correctedDistance;
                                }
                            }
                            if (astar[Globals.ACTION_RIGHT] != -1) {
                                double correctedDistance = astar[Globals.ACTION_RIGHT] + util.movefloatCorrection(util.towardsObject(e, (short) x, (short) y), false, e.getPacMan().getMovefloat(), 1);
                                if (toRightPower == -1 || correctedDistance < toRightPower) {
                                    toRightPower = correctedDistance;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (toLeft == -1) toLeft = maxVal;
        if (toRight == -1) toRight = maxVal;
        if (toUp == -1) toUp = maxVal;
        if (toDown == -1) toDown = maxVal;
        
        // Normalize distance
        toLeft = (maxVal - toLeft) / maxVal;
        toRight = (maxVal - toRight) / maxVal;
        toUp = (maxVal - toUp) / maxVal;
        toDown = (maxVal - toDown) / maxVal;
        
        if (toLeft < 0) toLeft = 0;
        if (toRight < 0) toRight = 0;
        if (toUp < 0) toUp = 0;
        if (toDown < 0) toDown = 0;
        
        if (toLeft > 1) toLeft = 1;
        if (toRight > 1) toRight = 1;
        if (toUp > 1) toUp = 1;
        if (toDown > 1) toDown = 1;
        
        if (Globals.bumpBestPill == true || Globals.declineWorstPills == true && Globals.normalizePill == true) {
            double highestValue = 0;
            short highestDirection = -1;
            if (toLeft > highestValue) {
                highestValue = toLeft;
                highestDirection = Globals.ACTION_LEFT;
            }
            if (toRight > highestValue) {
                highestValue = toRight;
                highestDirection = Globals.ACTION_RIGHT;
            }
            if (toUp > highestValue) {
                highestValue = toUp;
                highestDirection = Globals.ACTION_UP;
            }
            if (toDown > highestValue) {
                highestValue = toDown;
                highestDirection = Globals.ACTION_DOWN;
            }
            
            if (Globals.bumpBestPill == true && highestValue < Globals.bumpLimit) {
                switch(highestDirection) {
                    case Globals.ACTION_LEFT:
                        toLeft = toLeft + (Globals.bumpLimit - toLeft) * Globals.bumpSize;
                        break;
                    case Globals.ACTION_RIGHT:
                        toRight = toRight + (Globals.bumpLimit - toRight) * Globals.bumpSize;
                        break;
                    case Globals.ACTION_UP:
                        toUp = toUp + (Globals.bumpLimit - toUp) * Globals.bumpSize;
                        break;
                    case Globals.ACTION_DOWN:
                        toDown = toDown + (Globals.bumpLimit - toDown) * Globals.bumpSize;
                        break;
                }
            }
            
            if (Globals.declineWorstPills == true) {
                if (highestDirection != Globals.ACTION_LEFT && toLeft > Globals.declineLimit) {
                    toLeft = toLeft - (toLeft - Globals.declineLimit) * Globals.declineSize;
                }
                if (highestDirection != Globals.ACTION_RIGHT && toRight > Globals.declineLimit) {
                    toRight = toRight - (toRight - Globals.declineLimit) * Globals.declineSize;
                }
                if (highestDirection != Globals.ACTION_UP && toUp > Globals.declineLimit) {
                    toUp = toUp - (toUp - Globals.declineLimit) * Globals.declineSize;
                }
                if (highestDirection != Globals.ACTION_DOWN && toDown > Globals.declineLimit) {
                    toDown = toDown - (toDown - Globals.declineLimit) * Globals.declineSize;
                }
            }
        }
        
        if (toLeftPower == -1) toLeftPower = maxVal;
        if (toRightPower == -1) toRightPower = maxVal;
        if (toUpPower == -1) toUpPower = maxVal;
        if (toDownPower == -1) toDownPower = maxVal;
        
        // Normalize distance
        toLeftPower = (maxVal - toLeftPower) / maxVal;
        toRightPower = (maxVal - toRightPower) / maxVal;
        toUpPower = (maxVal - toUpPower) / maxVal;
        toDownPower = (maxVal - toDownPower) / maxVal;
        
        if (toLeftPower < 0) toLeftPower = 0;
        if (toRightPower < 0) toRightPower = 0;
        if (toUpPower < 0) toUpPower = 0;
        if (toDownPower < 0) toDownPower = 0;
        
        if (toLeftPower > 1) toLeftPower = 1;
        if (toRightPower > 1) toRightPower = 1;
        if (toUpPower > 1) toUpPower = 1;
        if (toDownPower > 1) toDownPower = 1;
        
        totalInput = toRight + toDown + toLeft + toUp;
    }

    // Recursive function used by breath-first search
    public Queue rec_BFS(int xPos, int yPos, short direction, int offset, Environment e, Queue q) {
        Queue empty = new LinkedList();

        if (e.maze[xPos][yPos] == Globals.MAZE_PILL || e.maze[xPos][yPos] == Globals.MAZE_POWER_PILL) {
            foundDistance = offset;
            return empty;
        } else if (Globals.pillRange > -1 && offset >= Globals.pillRange) {
            foundDistance = -1;
            return empty;
        }

        if (xPos > 0 && e.maze[xPos - 1][yPos] != Globals.MAZE_WALL && direction != Globals.ACTION_RIGHT) {
            instanceFind newInstance = new instanceFind(xPos - 1, yPos, Globals.ACTION_LEFT, offset + 1);
            q.offer(newInstance);
        }
        if (yPos > 0 && e.maze[xPos][yPos - 1] != Globals.MAZE_WALL && direction != Globals.ACTION_DOWN) {
            instanceFind newInstance = new instanceFind(xPos, yPos - 1, Globals.ACTION_UP, offset + 1);
            q.offer(newInstance);
        }
        if (xPos < e.mazeWidth - 1 && e.maze[xPos + 1][yPos] != Globals.MAZE_WALL && direction != Globals.ACTION_LEFT) {
            instanceFind newInstance = new instanceFind(xPos + 1, yPos, Globals.ACTION_RIGHT, offset + 1);
            q.offer(newInstance);
        }
        if (yPos < e.mazeHeight - 1 && e.maze[xPos][yPos + 1] != Globals.MAZE_WALL && direction != Globals.ACTION_UP) {
            instanceFind newInstance = new instanceFind(xPos, yPos + 1, Globals.ACTION_DOWN, offset + 1);
            q.offer(newInstance);
        }

        // Find the possible next moves by means of teleportation, if applicable
        if (xPos == 0 && e.maze[e.mazeWidth - 1][yPos] != Globals.MAZE_WALL && direction != Globals.ACTION_RIGHT) {
            instanceFind newInstance = new instanceFind(e.mazeWidth - 1, yPos, Globals.ACTION_LEFT, offset + 1);
            q.offer(newInstance);
        }
        if (xPos == e.mazeWidth - 1 && e.maze[0][yPos] != Globals.MAZE_WALL && direction != Globals.ACTION_LEFT) {
            instanceFind newInstance = new instanceFind(0, yPos, Globals.ACTION_RIGHT, offset + 1);
            q.offer(newInstance);
        }
        return q;
    }
}
