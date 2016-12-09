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
 *  ENTRAPMENTFINDER
 *  This class implements the entrapment finder as an input algorithm.
 * 
 */

package util;

import global.Globals;
import environment.Environment;
import environment.Vertex;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;
import pacman.Agent;

public class Entrapmentfinder {
    
    // Storage variables for the results of the search for entrapment    
    public double[] safeDegree;
    public boolean safeFound;
    
    private short xStart;
    private short yStart;
    
    private static ArrayList<Integer> forbiddenVertices;
    private static ArrayList<Integer> allowedVertices;
    private int[] totalRoutes;
    private int[][] routeCount;
    
    static class instanceFind {        
        int xPos = -1;
        int yPos = -1;
        int vertexID = -1;
        int previousVertex = -1;
        short initialDirection = -1;
        
        short distance = 1;
    }
    
    static class instanceVertex {        
        int id;
        short distance = 1;
        boolean skip;
    }
    
    public Entrapmentfinder() {
    
    }
    
    // Function to find escape routes
    // Will alter the public result variables
    public void findEntrapment(short x, short y, Agent[] ghost, Environment e) {
        int actualmaxVertex = Globals.maxVertex;
        
        totalRoutes = new int[actualmaxVertex];
        routeCount = new int[4][actualmaxVertex];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < actualmaxVertex; j++) {
                routeCount[i][j] = 0;
            }
        }
        
        xStart = x;
        yStart = y;
        
        safeFound = true;
        
        forbiddenVertices = new ArrayList<Integer>();
        allowedVertices = new ArrayList<Integer>();
        
        Queue possibleVertices = new LinkedList();
        
        // For starters, find the enclosing vertices around Ms. Pac-man,
        // and at them to a list
        int[] vertices = e.findEnclosingVertices(x, y);
        for (int vertex : vertices) {
            instanceVertex newVertex = new instanceVertex();
            newVertex.id = vertex;
            possibleVertices.add(newVertex);
        }
        
        // Now perform node expansion to find the neighbouring vertices, and
        // calculate the safeness of each vertex found.
        while (!possibleVertices.isEmpty()) {
            // This calls the recursive function that does the actual node expansion
            possibleVertices = recVertex((instanceVertex)possibleVertices.poll(), e, possibleVertices, ghost);
        }
        
        // Now that we have a list with unsafe vertices, add the current position of
        // Ms. Pac-man to a list.
        Queue patrolList = new LinkedList();

        instanceFind newInstance = new instanceFind();
        newInstance.xPos = xStart;
        newInstance.yPos = yStart;
        newInstance.distance = 1;
        
        patrolList.add(newInstance);
        
        for (int j=0; j<actualmaxVertex; j++) {
            totalRoutes[j] = 0;
        }
        
        // Once again perform node expansion starting from Ms. Pac-man's current position,
        // finding safe routes which will take us away from the ghosts.
        while (!patrolList.isEmpty()) {
            // This calls the recursive function that does the actual node expansion
            patrolList = recEntrapment((instanceFind)patrolList.poll(), e, patrolList);
        }
        
        safeDegree = new double[4];
        for (int i = 0; i < 4; i++) {
            safeDegree[i] = 0;
        }
        
        // Then loop through the routes found and calculate our resulting variables
        boolean foundEscape = false;
        
        // Start with the routes that take us [Globals.maxVertex] number of vertices away.
        // If none are found, consider the routes that take us [Globals.maxVertex - 1] number
        // of vertices away. And so on.
        for (int j = actualmaxVertex - 1; j >= 0; j--) {
            if (totalRoutes[j] > 0) {
                foundEscape = true;
                if (j < actualmaxVertex - 1) {
                    safeFound = false;
                }
                for (int i = 0; i < 4; i++) {
                    safeDegree[i] = (double) routeCount[i][j] / (double) totalRoutes[j];
                }
                j = -1;
            }
        }
        
        if (Globals.invertEntrapment == true) {
            for (int i = 0; i < 4; i++) {
                safeDegree[i] = 1 - safeDegree[i];
            }
        }
        
        // Or if it turns out that there are no safe routes at all, anywhere...
        if (foundEscape == false) {
            //System.out.println("- Entrapment finder could not find safe routes. We are doomed!");
        }
    }

    public Queue recVertex(instanceVertex o, Environment e, Queue q, Agent ghost[]) {
        // This recursive function is handed the next item in its queue.
        
        int ghostCount = ghost.length;
        Vertex curVertex = e.vertices.get(o.id);
        boolean forbidden = false;
        double pacdistance = 0, ghostdistance = 0;

        // If the current vertex has already been considered, it is in our forbidden or allowed list.
        // If the vertex isn't an intersection, but part of a hallway, then o.skip will be set to true.

        // Skip vertices which have been previously considered or are part of a hallway.
        int tmp = new Integer(o.id);
        if (o.skip == false && forbiddenVertices.indexOf(tmp) == -1 && allowedVertices.indexOf(tmp) == -1) {
            // Calculate the steps between Ms.Pac-man and the current vertex,
            // using the distance, Ms. Pac-man's speed and Globals.safeHeadstart.
            pacdistance = ((double)e.findShortestPath((short) e.getPacMan().x1, (short) e.getPacMan().y1, curVertex.getX(), curVertex.getY(), null)[0] + Globals.safeHeadstart) / e.getPacMan().speed;
            
            judgeVertex:
            // Loop through the ghosts
            for (int i = 0; i < ghostCount; i++) {
                // Only consider this ghost if it is not currently afraid.
                if (ghost[i].state != Globals.GHOST_AFRAID) {
                    // And calculate the steps between the current ghost and the current vertex,
                    // using the distance and the ghost's speed.
                    ghostdistance = ((double) e.findShortestPath((short) ghost[i].x1, (short) ghost[i].y1, curVertex.getX(), curVertex.getY(), null)[0]) / ghost[i].speed;

                    // If the ghost is partly on the vertex, or if the ghost can get to the vertex
                    // sooner than Ms. Pac-man can...
                    if ((curVertex.getX() == ghost[i].x1 && curVertex.getY() == ghost[i].y1)
                            || (curVertex.getX() == ghost[i].x2 && curVertex.getY() == ghost[i].y2)
                            || (pacdistance > ghostdistance)) {
                        // Then we should forbid this vertex.
                        forbiddenVertices.add((int) curVertex.getID());
                        // And is there is no nead to consider the vertex in relation to other ghosts,
                        // break out if this loop.
                        forbidden = true;
                        break judgeVertex;
                    }
                }
            }
        }
        // If the vertex wasn't forbidden, add it to the allowed list.
        if (forbidden == false) {
            allowedVertices.add((int) curVertex.getID());
        }

        // Now its time for some node expansion.
        // Find out the vertices which are connected to the current vertex.
        short[][] newVertices = e.findNeighborVertices(curVertex);
        
        // Find out if this vertex is an intersection, or if it is just part of a hallway.
        boolean skip = false;
        int isIntersection;
        if (newVertices.length > 2) {
            isIntersection = 1;
            skip = false;
        } else {
            isIntersection = 0;
            skip = true;
        }
        
        // If we haven't reached Globals.maxVertex yet...
        if (o.distance + isIntersection <= Globals.maxVertex) {
            // Loop through the neighbouring vertices.
            for (int i = 0; i < newVertices.length; i++) {
                int newtmp = new Integer(newVertices[i][0]);
                // Ignore those vertices who have been previously explored.
                if (forbiddenVertices.indexOf(newtmp) == -1 && allowedVertices.indexOf(newtmp) == -1) {
                    // And add the rest to our queue.
                    instanceVertex newVertex = new instanceVertex();
                    newVertex.id = newtmp;
                    newVertex.distance = (short) (o.distance + isIntersection);
                    newVertex.skip = skip;
                    q.add(newVertex);
                }
            }
        }
        
        return q;
    }

    // Recursive function used to find escape routes
    public Queue recEntrapment(instanceFind o, Environment e, Queue q) {
        // If this is the start point of our safe route...
        if (o.xPos != -1 && o.yPos != -1) {
            // Then find the vertex Ms Pac-man is on, or the vertices that enclose her.
            int[] pacVertices = e.findEnclosingVertices((short) o.xPos, (short) o.yPos);
            // Loop through these vertices.
            for (int j = 0; j < pacVertices.length; j++) {
                // If they are not forbidden...
                if (forbiddenVertices.indexOf((int) pacVertices[j]) == -1) {
                    // We found a new point that is safe for Ms. Pac-man to travel to.
                    instanceFind newInstance = new instanceFind();
                    Vertex currentVertex = e.vertices.get(pacVertices[j]);
                    
                    newInstance.vertexID = currentVertex.getID();
                    
                    // Store the starting direction for this new (partial) safe route.
                    if (currentVertex.getX() < xStart) {
                        newInstance.initialDirection = Globals.ACTION_LEFT;
                    } else if (currentVertex.getX() > xStart) {
                        newInstance.initialDirection = Globals.ACTION_RIGHT;
                    } else if (currentVertex.getY() < yStart) {
                        newInstance.initialDirection = Globals.ACTION_UP;
                    } else if (currentVertex.getY() > yStart) {
                        newInstance.initialDirection = Globals.ACTION_DOWN;
                    } else {
                        newInstance.initialDirection = -1;
                    }
                    
                    newInstance.previousVertex = pacVertices[pacVertices.length - 1 - j];
                    newInstance.distance = o.distance;
                    
                    // And add it to our queue.
                    q.add(newInstance);
                }
            }
        } else {
            // If this isn't the start point of our safe route, than we have somehow
            // managed to find a safe point for Ms. Pac-man to travel to.
            // Add it to our resulting variables.
            
            // Then find the vertices connected to this current safe point.
            Vertex currentVertex = e.vertices.get(o.vertexID);
            short[][] nextVertices = e.findNeighborVertices(currentVertex);
            
            // Find out if this vertex is an intersection, or if it is just part of a hallway.
            int isIntersection = nextVertices.length > 2 ? 1 : 0;
            
            if (isIntersection == 1) {
                if (o.initialDirection >= 0) {
                    routeCount[o.initialDirection][o.distance - 1]++;
                    totalRoutes[o.distance - 1]++;
                }
            }
            
            // If we haven't reached Globals.maxVertex yet...
            if (o.distance + isIntersection <= Globals.maxVertex) {
                // Loop through these vertices.
                for (int j = 0; j < nextVertices.length; j++) {
                    // If this vertex isn't forbidden, and we didn't just come from this vertex...
                    if (o.previousVertex != nextVertices[j][0] && forbiddenVertices.indexOf((int) nextVertices[j][0]) == -1) {
                        // We have found yet another safe point for Ms. Pac-man to travel to.
                        instanceFind newInstance = new instanceFind();
                        newInstance.vertexID = (int) nextVertices[j][0];
                        newInstance.distance = (short) (o.distance + isIntersection);
                        newInstance.previousVertex = o.vertexID;
                        
                        // Store the initial direction that started the path to this safe point.
                        if (o.initialDirection == -1) {
                            newInstance.initialDirection = nextVertices[j][2];
                        } else {
                            newInstance.initialDirection = o.initialDirection;
                        }
                        
                        // Add it to our queue
                        q.add(newInstance);
                    }
                }
            }
        }
        return q;
    }
}
