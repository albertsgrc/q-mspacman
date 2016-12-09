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
 *  This class is called to simulate a single maze, and contains all information
 *  on the in-game environement, agents and objects, along with functions
 *  for path finding.
 * 
 */

package environment;

import global.Globals;
import pacman.Agent;
import gui.Visualize;
import util.ASTARresult;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;
import java.util.Random;

public class Environment {

    // Status variables
    public boolean gameLoaded = false;
    public boolean gameEnded = false;
    public boolean gameLost = false;
    public int pillsLeft, powerPillsLeft;
    public int steps;                       // Number of network runs
    public boolean justPowerpill = false;   // Whether Ms. Pac-man ate a powerpill in the last run
    public static boolean justPill = false;        // Whether Ms. Pac-Man ate a pill in the last run.
    // Maze storage variables
    public short[][] maze;                  // Contains static maze objects, such as walls and pills
    public int[][] visMaze;                 // Contains itemID's for the visualisation, corresponding to maze positions
    public Visualize vis = null;
    public HashMap<Integer, Vertex> vertices = new HashMap<Integer, Vertex>();
    // A map containing vertices, which are intersections and corners in the maze
    String mazeName;
    public int mazeWidth, mazeHeight;
    public int totalPills, totalPowerPills;
    int[] spawnPoint = new int[2];          // X and Y position of the ghost spawn point
    int[] firstStop = new int[2];           // X and Y position of the ghost goto point
    //these are the coordinates of the corners of the maze 
    short[][] mazeCornerPoints;
    // Agent variables
    static Agent[] ghosts;
    public static short numGhosts;
    public static Agent PacMan;
    public static short globalGhostState = Globals.startingGhostState;

    /*
     * CONSTRUCTOR
     */
    public Environment(String filename) {
        mazeName = filename;
        readMaze(this.mazeName);
    }

    /*
     * GETTERS
     */
    public Agent getPacMan() {
        return this.PacMan;
    }

    public Agent getGhost(int i) {
        return this.ghosts[i];
    }

    public Agent[] getGhosts() {
        return this.ghosts;
    }

    /*
     * INITIALIZATION
     */
    public final void readMaze(String filename) {
        try {
            BufferedReader in = new BufferedReader(new FileReader(filename));
            if (in.readLine().startsWith("[Maze]")) {
                String header = in.readLine();
                String[] headerParts = header.split(",");
                int width = Integer.parseInt(headerParts[0]);
                int height = Integer.parseInt(headerParts[1]);
                this.maze = new short[width][height];
                this.visMaze = new int[width][height];
                int pacX = 0, pacY = 0;
                this.pillsLeft = 0;
                this.powerPillsLeft = 0;
                this.steps = 1;
                int itemID = 0;
                if (Globals.showVisualisation == true) {
                    vis = new Visualize(width, height);
                }
                for (short y = 0; y < height; y++) {
                    String[] dataParts = in.readLine().split(",");
                    for (short x = 0; x < width; x++) {
                        this.maze[x][y] = Short.parseShort(dataParts[x]);

                        if (Globals.showVisualisation == true) {
                            if (this.maze[x][y] != Globals.MAZE_PACMAN) {
                                itemID = vis.addItem(maze[x][y], x, y, x, y, 0);
                            }
                            if (this.maze[x][y] == Globals.MAZE_WALL) {
                                visMaze[x][y] = itemID;
                            }
                            if (this.maze[x][y] == Globals.MAZE_PILL) {
                                visMaze[x][y] = itemID;
                            }
                            if (this.maze[x][y] == Globals.MAZE_POWER_PILL) {
                                visMaze[x][y] = itemID;
                            }
                        }

                        if (this.maze[x][y] == Globals.MAZE_PILL || this.maze[x][y] == Globals.MAZE_POWER_PILL) {
                            this.pillsLeft++;
                        }
                        if (this.maze[x][y] == Globals.MAZE_POWER_PILL) {
                            this.powerPillsLeft++;
                        }
                        if (this.maze[x][y] == Globals.MAZE_PACMAN) {
                            pacX = x;
                            pacY = y;
                        }
                        if (this.maze[x][y] == Globals.MAZE_GHOST_SPAWN_POINT) {
                            spawnPoint[0] = x;
                            spawnPoint[1] = y;
                        }
                        if (this.maze[x][y] == Globals.MAZE_GHOST_FIRST_STOP) {
                            firstStop[0] = x;
                            firstStop[1] = y;
                        }
                    }
                }
                this.mazeHeight = height;
                this.mazeWidth = width;

                if (in.ready() == true && in.readLine().startsWith("[Ghost]")) {
                    header = in.readLine();
                    short id = 0;
                    numGhosts = Short.parseShort(header);
                    ghosts = new Agent[numGhosts];
                    for (int i = 0; i < numGhosts; i++) {
                        if (Globals.showVisualisation == true) {
                            itemID = vis.addItem(Globals.MAZE_GHOST, spawnPoint[0], spawnPoint[1], spawnPoint[0], spawnPoint[1], 0);
                        }
                        ghosts[i] = new Agent(spawnPoint[0], spawnPoint[1], Globals.pacmanSpeed * Double.parseDouble(in.readLine()), itemID, Globals.MAZE_GHOST, true);
                        ghosts[i].setId(id);
                        id++;
                    }
                }

                if (in.ready() == true && in.readLine().startsWith("[Scatter]")) {
                    mazeCornerPoints = new short[numGhosts][2];
                    for (int i = 0; i < numGhosts; i++) {
                        String[] scatterParts = in.readLine().split(",");
                        mazeCornerPoints[i][0] = (short) Integer.parseInt(scatterParts[0]);
                        mazeCornerPoints[i][1] = (short) Integer.parseInt(scatterParts[1]);
                    }
                }

                if (Globals.showVisualisation == true) {
                    itemID = vis.addItem(0, pacX, pacY, pacX, pacY, 0);
                }
                PacMan = new Agent(pacX, pacY, Globals.pacmanSpeed, itemID, Globals.MAZE_PACMAN, false);
            }

            if (Globals.showVisualisation == true) {
                vis.updateVisualisation();
            }

            totalPills = pillsLeft;
            totalPowerPills = powerPillsLeft;

            gameLoaded = true;

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void setVertices() {
        // fills 'vertices' with all vertices contained in the maze        
        int id = 0;

        for (short x = 0; x < mazeWidth - 1; x++) {
            for (short y = 0; y < mazeHeight; y++) {
                if (isVertex(x, y)) {
                    Vertex v = new Vertex((short) id, x, y);
                    vertices.put(id, v);
                    id++;
                }
            }
        }
    }

    public void setNeigborVertices() {
        // for each vertex, find neighbors and corresponding distance
        for (int i = 0; i < vertices.size(); i++) {
            short[][] neighborvertices = findNeighborVertices(vertices.get(i));
            vertices.get(i).setNeighbors(neighborvertices);
        }
    }

    /*
     * PATH FINDING
     */
    public short[] findShortestPath(short x1, short y1, short x2, short y2, int[] blacklist) {
        // Uses repeated A* to find the shortest path from x1,y1 to x2,y2.
        // Returns an array with the distance and the first move starting from x1,y1.        
        short[] result = new short[2];
        int currentPath;
        short shortestPath = -1;
        short fullPath = -1;
        Vertex startVertex, endVertex;
        ASTARresult best = new ASTARresult();
        ASTARresult astar;

        if (x1 == x2 && y1 == y2) {
            return result;
        }

        // Find the enclosing vertices for the start and end point
        int startvertices[] = findEnclosingVertices(x1, y1);
        int endvertices[] = findEnclosingVertices(x2, y2);

        // Check how many of the start vertices are also end vertices
        int countStartEnd = 0;
        int same = -1;
        for (int i = 0; i < startvertices.length; i++) {
            if (Arrays.binarySearch(endvertices, startvertices[i]) >= 0) {
                countStartEnd++;
                same = i;
            }
        }

        // If all start vertices are also end vertices, then both start and goal
        // are between the same vertices.
        // We don't have to use A*, and will instead use euclidian distance towards the goal.
        if (countStartEnd == endvertices.length || countStartEnd == startvertices.length) {
            result[0] = distance(x1, y1, x2, y2);

            // Find out if teleporting would be quicker.
            boolean teleport = (Math.abs(x2 - x1)) > (mazeWidth / 2);

            // Calculate the first move to be made
            if (x2 > x1 && teleport) {
                result[1] = Globals.ACTION_LEFT;
            } else if (x2 < x1 && teleport) {
                result[1] = Globals.ACTION_RIGHT;
            } else if (x1 > x2) {
                result[1] = Globals.ACTION_LEFT;
            } else if (x1 < x2) {
                result[1] = Globals.ACTION_RIGHT;
            } else if (y1 > y2) {
                result[1] = Globals.ACTION_UP;
            } else if (y1 < y2) {
                result[1] = Globals.ACTION_DOWN;
            }

            return result;

            // If not all but some start vertices are also end vertices, then start and goal
            // are seperated by exactly one vertex.
            // We don't have to use A*, and will instead use euclidian distance towards the vertex.
        } else if (countStartEnd > 0) {
            result[0] = distance(x1, y1, x2, y2);

            endVertex = vertices.get(startvertices[same]);
            
            // Find out if teleporting would be quicker.
            boolean teleport = (Math.abs(endVertex.getX() - x1)) > (mazeWidth / 2);

            // Calculate the first move to be made
            if (endVertex.getX() > x1 && teleport) {
                result[1] = Globals.ACTION_LEFT;
            } else if (endVertex.getX() < x1 && teleport) {
                result[1] = Globals.ACTION_RIGHT;
            } else if (endVertex.getX() < x1) {
                result[1] = Globals.ACTION_LEFT;
            } else if (endVertex.getX() > x1) {
                result[1] = Globals.ACTION_RIGHT;
            } else if (endVertex.getY() < y1) {
                result[1] = Globals.ACTION_UP;
            } else if (endVertex.getY() > y1) {
                result[1] = Globals.ACTION_DOWN;
            }

            return result;

        } else {
            // Loop through the vertices and perform A*.
            for (int i = 0; i < startvertices.length; i++) {
                for (int j = 0; j < endvertices.length; j++) {
                    astar = ASTAR(startvertices[i], endvertices[j], blacklist);
                    if (astar.totalPathSize >= 0) {
                        // Calculate the full path length based on A* and distance to relevant vertices.
                        startVertex = vertices.get(startvertices[i]);
                        endVertex = vertices.get(endvertices[j]);
                        currentPath = astar.totalPathSize + distance(x1, y1, (short) startVertex.getX(), (short) startVertex.getY()) + distance(x2, y2, (short) endVertex.getX(), (short) endVertex.getY());

                        // Store this path if it is the shortest one yet.
                        if (astar.totalPathSize >= 0 && (currentPath < shortestPath || shortestPath == -1)) {
                            shortestPath = (short) currentPath;
                            best = astar;
                        }
                    }
                }
            }

            // If a path was found, return the path length and the first move.
            if (shortestPath != -1) {
                result[0] = shortestPath;
                int startX, startY;

                startVertex = vertices.get((int) best.reconstructedPath.get(0));

                // If we started on the exact spot of the first vertex in our path...
                if (startVertex.getX() == x1 && startVertex.getY() == y1) {
                    // move towards second vertex
                    if (best.reconstructedPath.size() > 1) {
                        startVertex = vertices.get((int) best.reconstructedPath.get(1));
                        startX = startVertex.getX();
                        startY = startVertex.getY();
                    } else {
                        // Or if there is no second vertex, calculate the move by using
                        // the direct path to the goal.
                        startX = x2;
                        startY = y2;
                    }
                } else {
                    // Or if neither if this applies, just move towards the first vertex in the path
                    startX = startVertex.getX();
                    startY = startVertex.getY();
                }

                // We have now established a certain (x,y)-point in the maze,
                // which is the next vertex on the path towards the goal,
                // or the actual goal if it can be reached directly.
                // This means the point is reachable in a straight line from the current position
                
                // Find out if teleporting would be quicker.
                boolean teleport = (Math.abs(startX - x1)) > (mazeWidth / 2);

                // Calculate the first move to be made
                if (startX > x1 && teleport) {
                    result[1] = Globals.ACTION_LEFT;
                } else if (startX < x1 && teleport) {
                    result[1] = Globals.ACTION_RIGHT;
                } else if (startX < x1) {
                    result[1] = Globals.ACTION_LEFT;
                } else if (startX > x1) {
                    result[1] = Globals.ACTION_RIGHT;
                } else if (startY < y1) {
                    result[1] = Globals.ACTION_UP;
                } else if (startY > y1) {
                    result[1] = Globals.ACTION_DOWN;
                }
            } else {
                result[0] = -1;
                result[1] = -1;
            }
        }
        return result;
    }

    public short[] findAllPaths(short x1, short y1, short x2, short y2) {
        // Uses repeated A* to find the paths from x1,y1 to x2,y2.
        // An array will be returned containing the shortest path for every moving position.
        // If that value is -1, no path was found.
        short[] result = new short[4];
        int currentPath;
        short[] shortestPath = new short[4];
        Vertex startVertex, endVertex;
        ASTARresult astar;

        for (int i = 0; i < 4; i++) {
            shortestPath[i] = -1;
            result[i] = -1;
        }

        if (x1 == x2 && y1 == y2) {
            return result;
        }

        // Find the enclosing vertices for the start and end point
        int startvertices[] = findEnclosingVertices(x1, y1);
        int endvertices[] = findEnclosingVertices(x2, y2);

        // Check how many of the start vertices are also end vertices
        int countStartEnd = 0;
        for (int i = 0; i < startvertices.length; i++) {
            if (Arrays.binarySearch(endvertices, startvertices[i]) >= 0) {
                countStartEnd++;
            }
        }

        // If all start vertices are also end vertices, then both start and goal
        // are between the same vertices.
        // We don't have to use A*, and will instead use euclidian distance.
        if (countStartEnd == endvertices.length || countStartEnd == startvertices.length) {
            int resultstorage;
            resultstorage = distance(x1, y1, x2, y2);

            if (x1 > x2) {
                result[Globals.ACTION_LEFT] = (short) resultstorage;
            } else if (x1 < x2) {
                result[Globals.ACTION_RIGHT] = (short) resultstorage;
            } else if (y1 > y2) {
                result[Globals.ACTION_UP] = (short) resultstorage;
            } else if (y1 < y2) {
                result[Globals.ACTION_DOWN] = (short) resultstorage;
            }

            return result;

            // If not all start vertices are also end vertices, then start and goal are seperated by at least one vertex.
            // This means there are multiple acceptable paths, and we should use A* to find them.

        } else {
            // Loop through the vertices and perform A*.
            for (int i = 0; i < startvertices.length; i++) {
                for (int j = 0; j < endvertices.length; j++) {
                    int[] blacklist = new int[startvertices.length - 1];
                    int l = 0;
                    for (int k = 0; k < startvertices.length; k++) {
                        if (startvertices[k] != startvertices[i]) {
                            blacklist[l] = startvertices[k];
                            l++;
                        }
                    }
                    astar = ASTAR(startvertices[i], endvertices[j], blacklist);
                    if (astar.totalPathSize >= 0) {
                        // Calculate the full path length based on A* and distance to relevant vertices.
                        startVertex = vertices.get(startvertices[i]);
                        endVertex = vertices.get(endvertices[j]);
                        currentPath = astar.totalPathSize + distance(x1, y1, (short) startVertex.getX(), (short) startVertex.getY()) + distance(x2, y2, (short) endVertex.getX(), (short) endVertex.getY());

                        short direction = -1;
                        short position = 0;
                        short orientationx = 0;
                        short orientationy = 0;
                        if (vertices.get((int) astar.reconstructedPath.get(0)).getX() == x1 && vertices.get((int) astar.reconstructedPath.get(0)).getY() == y1) {
                            position++;
                        }
                        if (position >= astar.reconstructedPath.size()) {
                            orientationx = x2;
                            orientationy = y2;
                        } else {
                            orientationx = vertices.get((int) astar.reconstructedPath.get(position)).getX();
                            orientationy = vertices.get((int) astar.reconstructedPath.get(position)).getY();
                        }

                        if (orientationx < x1) {
                            direction = Globals.ACTION_LEFT;
                        } else if (orientationx > x1) {
                            direction = Globals.ACTION_RIGHT;
                        } else if (orientationy < y1) {
                            direction = Globals.ACTION_UP;
                        } else if (orientationy > y1) {
                            direction = Globals.ACTION_DOWN;
                        }
                        // Store this path if it is the shortest one yet.
                        if (currentPath < result[direction] || result[direction] == -1) {
                            result[direction] = (short) currentPath;
                        }
                    }
                }
            }
        }
        return result;
    }

    private ASTARresult ASTAR(int startID, int goalID, int[] blacklist) {
        // A* implementation
        Vertex current;
        int numvertices = vertices.size();
        Comparator<Vertex> comparator = new VertexComparator();

        // Resulting class, which will contain distance and path.
        ASTARresult result = new ASTARresult();

        Vertex start = vertices.get(startID);

        // set of tentative nodes
        PriorityQueue<Vertex> openSet = new PriorityQueue<Vertex>(10, comparator);

        // map of navigated nodes
        short[] cameFrom = new short[numvertices];

        // little house keeping 
        boolean[] inClosedSet = new boolean[numvertices];
        boolean[] inOpenSet = new boolean[numvertices];

        // initializations, for reconstructing path later..
        for (int i = 0; i < numvertices; i++) {
            cameFrom[i] = -1;
            inClosedSet[i] = false;
            inOpenSet[i] = false;
            if (blacklist != null) {
                for (int j = 0; j < blacklist.length; j++) {
                    if (i == blacklist[j]) {
                        inClosedSet[i] = true;
                    }
                }
            }
        }

        // individual node scores
        int[] g_score = new int[numvertices];
        int[] h_score = new int[numvertices];
        int[] f_score = new int[numvertices];

        // get x and y values of goal and start node
        short xStart = vertices.get(startID).getX();
        short yStart = vertices.get(startID).getY();
        short xGoal = vertices.get(goalID).getX();
        short yGoal = vertices.get(goalID).getY();

        // set initial values
        g_score[startID] = 0;
        h_score[startID] = distance(xStart, yStart, xGoal, yGoal);
        f_score[startID] = g_score[startID] + h_score[startID];
        start.setF_score(f_score[startID]);

        // add start node to openset
        openSet.add(start);
        inOpenSet[startID] = true;

        while (openSet.size() > 0) {
            // retrieve node with lowest f_score from open set
            current = openSet.peek();
            short currentID = current.getID();

            if (currentID == goalID) {
                result.totalPathSize = (short) g_score[currentID];
                result.reconstructedPath = reconstructPath(cameFrom, currentID);
                return (result);
            }

            // little housekeeping
            inClosedSet[currentID] = true;
            openSet.remove(current);
            inOpenSet[currentID] = false;

            // get neighbor nodes from current
            short[][] neighborNodes = current.getNeighbors();
            int neighbor;

            for (int i = 0; i < neighborNodes.length; i++) {
                neighbor = neighborNodes[i][0];

                if (inClosedSet[neighbor]) {
                    continue;
                }

                int tentative_g_score = g_score[currentID] + neighborNodes[i][1];
                boolean betterScore;

                if (!inOpenSet[neighbor]) {
                    betterScore = true;
                } else if (tentative_g_score < g_score[neighbor]) {
                    betterScore = true;
                } else {
                    betterScore = false;
                }

                if (betterScore) {
                    cameFrom[neighbor] = currentID;
                    g_score[neighbor] = tentative_g_score;
                    f_score[neighbor] = g_score[neighbor] + h_score[neighbor];

                    Vertex v = vertices.get(neighbor);
                    if (!inOpenSet[neighbor]) {
                        v.setF_score(f_score[neighbor]);
                        inOpenSet[neighbor] = true;
                        openSet.add(v);
                    } else {
                        openSet.remove(v);
                        v.setF_score(f_score[neighbor]);
                        openSet.add(v);
                    }
                }
            }
        }
        // we should not be able to reach this point
        return new ASTARresult();
    }

    private ArrayList<Short> reconstructPath(short[] cameFrom, short current) {
        ArrayList<Short> path = new ArrayList<Short>();
        if (cameFrom[current] != -1) {
            path = reconstructPath(cameFrom, cameFrom[current]);
            path.add(current);
            return path;
        } else {
            path.add(current);
            return path;
        }
    }

    public short distance(short xStart, short yStart, short xGoal, short yGoal) {
        short shortestPath = Short.MAX_VALUE;
        short realDist = (short) (Math.abs(xStart - xGoal) + Math.abs(yStart - yGoal));
        short virtualDist = (short) (xStart + (mazeWidth - xGoal) + Math.abs(yStart - yGoal));

        if (realDist < shortestPath) {
            shortestPath = realDist;
        }
        if (virtualDist < shortestPath) {
            shortestPath = virtualDist;
        }

        return shortestPath;
    }

    public short oppositeDirection(short direction) {
        if (direction == Globals.ACTION_LEFT) {
            return Globals.ACTION_RIGHT;
        }
        if (direction == Globals.ACTION_RIGHT) {
            return Globals.ACTION_LEFT;
        }
        if (direction == Globals.ACTION_UP) {
            return Globals.ACTION_DOWN;
        }
        if (direction == Globals.ACTION_DOWN) {
            return Globals.ACTION_UP;
        }
        return direction;
    }

    public void propagateGhostBehaviour() {
        // This function resets the ghost states to -1,
        // so that on their next move they'll assume the global ghost state.

        for (int i = 0; i < ghosts.length; i++) {
            ghosts[i].state = -1;
        }
    }

    /*
     * GHOST BEHAVIOR
     */
    public void moveGhost(int ghostId) {
        short dir = 0;

        short state = ghosts[ghostId].state;
        if (state != globalGhostState) {
            if (state == -1 || state == Globals.GHOST_CHASE || state == Globals.GHOST_RANDOM) {
                state = globalGhostState;
                ghosts[ghostId].state = state;
            }
        }

        short PacManX = (short) getPacMan().getX2();
        short PacManY = (short) getPacMan().getY2();

        short x = (short) ghosts[ghostId].getX1();
        short y = (short) ghosts[ghostId].getY1();

        Random generator = new Random();

        if (maze[x][y] == Globals.MAZE_GHOST_SPAWN_AREA || maze[x][y] == Globals.MAZE_GHOST_SPAWN_POINT) {
            getGhost(ghostId).setVisType(Globals.MAZE_GHOST); // Set the proper image in the visualisation

            if (x < firstStop[0]) {
                dir = Globals.ACTION_RIGHT;
            } else if (x > firstStop[0]) {
                dir = Globals.ACTION_LEFT;
            } else if (y < firstStop[1]) {
                dir = Globals.ACTION_DOWN;
            } else {
                dir = Globals.ACTION_UP;
            }

            getGhost(ghostId).setDirection(dir);
            getGhost(ghostId).move(dir, this);
        } else {
            short[] res;

            switch (state) {

                case Globals.GHOST_SCATTER:

                    /*
                     * The aim of this behaviour is to send the ghosts to the
                     * corners of the maze. This behaviour makes use of the
                     * findShortestPath method to calculate the shortest path to
                     * each corner.
                     */

                    getGhost(ghostId).setVisType(Globals.MAZE_GHOST); // Set the proper image in the visualisation

                    res = findShortestPath(x, y, mazeCornerPoints[ghostId][0], mazeCornerPoints[ghostId][1], null);
                    dir = res[1];
                    if (res[0] < 1) {
                        getGhost(ghostId).state = -1; // already there -> continue behaviour
                    }
                    getGhost(ghostId).setDirection(dir);
                    getGhost(ghostId).move(dir, this);

                    break;

                case Globals.GHOST_CHASE:

                    /*
                     * The aim of this behaviour is to chase Ms. Pac Man. The
                     * implementation of this behaviour makes use of the
                     * findShortestPath method to calculate the shortest path to Ms.
                     * Pac Man.
                     */

                    getGhost(ghostId).setVisType(Globals.MAZE_GHOST); // Set the proper image in the visualisation

                    if (isVertex(x, y)) {
                        res = findShortestPath(x, y, PacManX, PacManY, null);
                        dir = res[1];
                    } else {
                        dir = getGhost(ghostId).getDirection();
                    }

                    getGhost(ghostId).setDirection(dir);
                    getGhost(ghostId).move(dir, this);

                    break;
                case Globals.GHOST_AFRAID:
                    /*
                     * The aim of this behaviour is to avoid Ms. Pac Man. The
                     * implementation of this behaviour makes use of the
                     * findShortestPath method to calculate the shortest path to Ms.
                     * Pac Man. The next step is to direct the ghosts to the
                     * opposite direction of the one that was found to be the
                     * shortest to Ms Pac Man.
                     */

                    getGhost(ghostId).setVisType(Globals.MAZE_FRIGHT_GHOST);
                    if (isVertex(x, y)) {
                        res = findShortestPath(x, y, PacManX, PacManY, null);
                        ArrayList<Short> pa = possibleActions(x, y);
                        if (pa.contains(oppositeDirection(res[1]))) {
                            dir = oppositeDirection(res[1]);
                        } else {
                            dir = res[1];
                            while (dir == res[1]) {
                                //random number generator
                                dir = pa.get(generator.nextInt(pa.size()));
                            }
                        }
                    } else {
                        dir = getGhost(ghostId).getDirection();
                    }

                    getGhost(ghostId).setDirection(dir);
                    getGhost(ghostId).move(dir, this);

                    break;

                case Globals.GHOST_RANDOM:

                    getGhost(ghostId).setVisType(Globals.MAZE_GHOST);

                    /*
                     * The aim of this behaviour is to move around arbitrarily. The
                     * implementation of this behaviour makes use of a random number
                     * generator to "draw" a random number that specifies the index
                     * of the next direction from the array of all possible next
                     * directions.
                     */

                    ArrayList<Short> pa = possibleActions(x, y);
                        
                    if (isVertex(x, y) || !pa.contains(getGhost(ghostId).direction)) {
                        dir = pa.get((short) generator.nextInt(pa.size()));
                    } else {
                        dir = getGhost(ghostId).getDirection();
                    }
                    getGhost(ghostId).setDirection(dir);
                    getGhost(ghostId).move(dir, this);

            }
        }

    }

    public void moveInDifferentDirections() {
        /*
         * The aim of this method is to make sure that ghosts do not overlap.
         * The first step is to find out some of the characteristics of the
         * ghosts like what their location and direction will be in the next
         * step. The next step is to determine if there are overlaps in the
         * sense that ghosts have the same location and direction. Finally, if
         * possible, the ghosts that overlap should be sent in different
         * directions, otherwise nothing changes and the overlap remains.
         */
        short characteristics[][] = new short[numGhosts][3];
        for (int i = 0; i < numGhosts; i++) {
            characteristics[i][0] = (short) ghosts[i].getX1();
            characteristics[i][1] = (short) ghosts[i].getY1();
            characteristics[i][2] = ghosts[i].getDirection();
        }



        for (int i = 0; i < numGhosts; i++) {
            for (int j = i + 1; j < numGhosts; j++) {
                boolean a = (characteristics[i][0] == characteristics[j][0]);
                boolean b = (characteristics[i][1] == characteristics[j][1]);
                boolean c = (characteristics[i][2] == characteristics[j][2]);
                if ((a && b) && c) {
                    ArrayList<Short> pa = possibleActions(characteristics[i][0], characteristics[i][1]);
                    for (int k = 0; k < pa.size(); k++) {
                        if (pa.get(k) != characteristics[i][2]) {
                            ghosts[i].setDirection(pa.get(k));
                            break;
                        }
                    }
                }
            }
        }
    }

    /*
     * VERTEX FUNCTIONS
     */
    public boolean isVertex(short x, short y) {
        ArrayList pa = possibleActions(x, y);

        // ignore spawn point area
        if (maze[x][y] == Globals.MAZE_GHOST_SPAWN_AREA) {
            return false;
        }

        // the ghosts' first stop is always a vertex
        if (maze[x][y] == Globals.MAZE_GHOST_FIRST_STOP) {
            return true;
        }
        if (pa.size() >= 3) {
            return true;
        } else {
            if (pa.size() == 2) {
                // top 
                if (pa.contains(Globals.ACTION_DOWN) && !pa.contains(Globals.ACTION_UP)) {
                    return true;
                }
                // bottom
                if (pa.contains(Globals.ACTION_UP) && !pa.contains(Globals.ACTION_DOWN)) {
                    return true;
                }
            }
        }
        return false;
    }

    public short[][] findNeighborVertices(Vertex location) {
        // returns an array containing id's from neighbouring vertices
        ArrayList<short[]> neighborvertices = new ArrayList<short[]>();
        ArrayList<Short> pa = possibleActions(location.getX(), location.getY());
        short x = location.getX();
        short y = location.getY();

        int currentAction;
        Vertex temp = location.clone();

        for (int i = 0; i < pa.size(); i++) {
            currentAction = pa.get(i);
            do {
                if (currentAction == Globals.ACTION_DOWN) {
                    y++;
                }
                if (currentAction == Globals.ACTION_UP) {
                    y--;
                }
                if (currentAction == Globals.ACTION_LEFT) {
                    x--;
                    if (x == 0) {
                        x = (short) (mazeWidth - 2);
                    }
                }
                if (currentAction == Globals.ACTION_RIGHT) {
                    x++;
                    if (x == (mazeWidth - 1)) {
                        x = 0;
                    }
                }

                temp.setX(x);
                temp.setY(y);
            } while (!isVertex(x, y));

            // find corresponding vertex id
            short id = 0;
            for (int j = 0; j < vertices.size(); j++) {
                if (vertices.get(j).equals(temp)) {
                    id = vertices.get(j).getID();
                }
            }

            // find distance to original node
            short dist = distance(location.getX(), location.getY(), temp.getX(), temp.getY());

            // add new element to neighborList
            short neighbor[] = {id, dist, (short) currentAction};
            neighborvertices.add(neighbor);

            // reset currentLocation to interest point
            x = location.getX();
            y = location.getY();
        }

        short[][] neighborList = new short[neighborvertices.size()][];
        for (int i = 0; i < neighborvertices.size(); i++) {
            neighborList[i] = neighborvertices.get(i);
        }
        return neighborList;
    }

    public int[] findEnclosingVertices(short x, short y) {
        ArrayList<Short> pa = possibleActions(x, y);

        if (isVertex(x, y)) {
            int[] result = {getKey(x, y)};
            return result;
        }

        int toLeftID = -1;
        int toTopID = -1;
        int toDownID = -1;
        int toRightID = -1;

        int foundCount = 0;

        short i, j;
        boolean stop;

        i = x;
        j = y;
        stop = false;
        // Find if there is any vertex to our left
        while (stop == false) {
            i--;
            // If we have reached the maze edge, teleport.
            if (i < 0) {
                i = (short) (mazeWidth - 1);
            }

            // And stop looking if we encounter a wall
            if (maze[i][j] != Globals.MAZE_WALL) {
                if (isVertex(i, j)) {
                    toLeftID = getKey(i, j);
                    foundCount++;
                    stop = true;
                }
            } else {
                stop = true;
            }
        }

        i = x;
        j = y;
        stop = false;
        // Find if there is any vertex above us
        while (stop == false) {
            j--;
            // If we have reached the maze edge, teleport.
            if (j < 0) {
                j = (short) (mazeHeight - 1);
            }

            // And stop looking if we encounter a wall
            if (maze[i][j] != Globals.MAZE_WALL) {
                if (isVertex(i, j)) {
                    toTopID = getKey(i, j);
                    foundCount++;
                    stop = true;
                }
            } else {
                stop = true;
            }
        }

        i = x;
        j = y;
        stop = false;
        // Find if there is any vertex to our right
        while (stop == false) {
            i++;
            // If we have reached the maze edge, teleport.
            if (i > mazeWidth - 1) {
                i = 0;
            }

            // And stop looking if we encounter a wall
            if (maze[i][j] != Globals.MAZE_WALL) {
                if (isVertex(i, j)) {
                    toRightID = getKey(i, j);
                    foundCount++;
                    stop = true;
                }
            } else {
                stop = true;
            }
        }

        i = x;
        j = y;
        stop = false;
        // Find if there is any vertex below us
        while (stop == false) {
            j++;
            // If we have reached the maze edge, teleport.
            if (j > mazeHeight - 1) {
                j = 0;
            }

            // And stop looking if we encounter a wall
            if (maze[i][j] != Globals.MAZE_WALL) {
                if (isVertex(i, j)) {
                    toDownID = getKey(i, j);
                    foundCount++;
                    stop = true;
                }
            } else {
                stop = true;
            }
        }

        int enclosingvertices[] = new int[foundCount];
        if (foundCount > 0) {
            i = 0;
            if (toLeftID != -1) {
                enclosingvertices[i] = toLeftID;
                i++;
            }
            if (toTopID != -1) {
                enclosingvertices[i] = toTopID;
                i++;
            }
            if (toRightID != -1) {
                enclosingvertices[i] = toRightID;
                i++;
            }
            if (toDownID != -1) {
                enclosingvertices[i] = toDownID;
                i++;
            }
        }

        return enclosingvertices;
    }

    public int getKey(int x, int y) {
        // returns the key that belongs to the vertex located at (x,y)
        // only available if 'vertices' is set.
        Vertex v = new Vertex((short) x, (short) y);
        for (int i = 0; i < vertices.size(); i++) {
            if (vertices.get(i).equals(v)) {
                return vertices.get(i).getID();
            }
        }
        return -1;
    }

    /*
     * OTHER
     */
    public ArrayList<Short> possibleActions(int x, int y) {
        ArrayList<Short> possibleActions = new ArrayList<Short>();

        if (x < 0 || y < 0) {
            System.out.println(x + "," + y);
        }

        if (maze[x][y] == Globals.MAZE_WALL) {
            return possibleActions;
        }

        // right
        if (x < mazeWidth - 1 && maze[x + 1][y] != Globals.MAZE_WALL && maze[x + 1][y] != Globals.MAZE_GHOST_SPAWN_AREA) {
            possibleActions.add(Globals.ACTION_RIGHT);
        }

        // left
        if (x > 0 && maze[x - 1][y] != Globals.MAZE_WALL && maze[x - 1][y] != Globals.MAZE_GHOST_SPAWN_AREA) {
            possibleActions.add(Globals.ACTION_LEFT);
        }

        // up
        if (y > 0 && maze[x][y - 1] != Globals.MAZE_WALL && maze[x][y - 1] != Globals.MAZE_GHOST_SPAWN_AREA) {
            possibleActions.add(Globals.ACTION_UP);
        }

        // down
        if (y < mazeHeight - 1 && maze[x][y + 1] != Globals.MAZE_WALL && maze[x][y + 1] != Globals.MAZE_GHOST_SPAWN_AREA) {
            possibleActions.add(Globals.ACTION_DOWN);
        }

        return possibleActions;
    }

    public short ghostCollision() {
        short result = Globals.COLLISION_NO;

        double pacX;
        double pacY;
        double ghostX;
        double ghostY;

        double distX;
        double distY;
        double dist;

        if (this.getPacMan().getX1() > this.getPacMan().getX2()) {
            pacX = this.getPacMan().getX2() + this.getPacMan().getMovefloat();
        } else if (this.getPacMan().getX1() < this.getPacMan().getX2()) {
            pacX = this.getPacMan().getX2() - this.getPacMan().getMovefloat();
        } else {
            pacX = this.getPacMan().getX2();
        }
        if (this.getPacMan().getY1() > this.getPacMan().getY2()) {
            pacY = this.getPacMan().getY2() + this.getPacMan().getMovefloat();
        } else if (this.getPacMan().getY1() < this.getPacMan().getY2()) {
            pacY = this.getPacMan().getY2() - this.getPacMan().getMovefloat();
        } else {
            pacY = this.getPacMan().getY2();
        }

        for (short i = 0; i < numGhosts; i++) {
            if (this.getGhosts()[i].getX1() > this.getGhosts()[i].getX2()) {
                ghostX = this.getGhosts()[i].getX2() + this.getGhosts()[i].getMovefloat();
            } else if (this.getGhosts()[i].getX1() < this.getGhosts()[i].getX2()) {
                ghostX = this.getGhosts()[i].getX2() - this.getGhosts()[i].getMovefloat();
            } else {
                ghostX = this.getGhosts()[i].getX1();
            }
            if (this.getGhosts()[i].getY1() > this.getGhosts()[i].getY2()) {
                ghostY = this.getGhosts()[i].getY2() + this.getGhosts()[i].getMovefloat();
            } else if (this.getGhosts()[i].getX1() < this.getGhosts()[i].getY2()) {
                ghostY = this.getGhosts()[i].getY2() - this.getGhosts()[i].getMovefloat();
            } else {
                ghostY = this.getGhosts()[i].getY1();
            }

            distX = Math.abs(pacX - ghostX);
            distY = Math.abs(pacY - ghostY);
            dist = distX + distY;

            if (dist < 1) {

                short gx = (short) getGhost(i).getX1();
                short gy = (short) getGhost(i).getY1();

                if (getGhost(i).state == Globals.GHOST_AFRAID) {
                    result = Globals.COLLISION_ALLOWED;
                    getGhost(i).state = -1;
                    short[] res = findShortestPath(gx, gy, (short) spawnPoint[0], (short) spawnPoint[1], null);
                    getGhost(i).setWaitTime((res[0] / getGhost(i).getSpeed()) / Globals.effectEaten);
                    getGhost(i).setPos(spawnPoint);
                    getGhost(i).drawItems(this);
                } else if (this.getGhost(i).getWaitTime() == 0) {
                    return Globals.COLLISION_FATAL;
                }
            }
        }
        return result;
    }

    public double eatItem(int x, int y, double movefloat) {
        double eatReward = 0;
        if (this.maze[x][y] == Globals.MAZE_PILL) {
            eatReward += Globals.rewardPill;
            justPill = true;
            pillsLeft--;
        } else if (this.maze[x][y] == Globals.MAZE_POWER_PILL) {
            eatReward += Globals.rewardPowerPill;
            justPowerpill = true;
            pillsLeft--;
            powerPillsLeft--;
        }

        // Update environment after eating item
        if (maze[x][y] > 0) {
            this.maze[x][y] = 0;
        }

        // Update visualization
        if (visMaze[x][y] > 0) {
            if (maze[x][y] != Globals.MAZE_WALL) {
                vis.editItem(visMaze[x][y], 0, x, y, x, y, 0);
            }
        }

        // return reward
        return eatReward;
    }

    /*
     * PRINT FUNCTIONS
     */
    public void printVertices() {
        for (int i = 0; i < vertices.size(); i++) {
            short id = vertices.get(i).getID();
            short x = vertices.get(i).getX();
            short y = vertices.get(i).getY();

            vis.addItem(Globals.MAZE_GHOST, x, y, x, y, 0);
            short[][] neighbors = vertices.get(i).getNeighbors();
            for (int j = 0; j < neighbors.length; j++) {
                System.out.print(neighbors[j][0] + "(" + neighbors[j][1] + "), ");
            }
        }
    }

    public void printMaze() {
        System.out.print("Maze: " + mazeName + "\n" + mazeWidth + "," + mazeHeight);

        for (int y = 0; y < mazeHeight; y++) {
            for (int x = 0; x < mazeWidth; x++) {
                System.out.print(maze[x][y]);
            }
            System.out.println("");
        }
    }
}
