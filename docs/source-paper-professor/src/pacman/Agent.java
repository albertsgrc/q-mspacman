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
 *  AGENT
 *  This class is called to hold information all on a single agent.
 * 
 */

package pacman;

import environment.Environment;
import global.Globals;
import gui.Visualize;

public class Agent {

    public boolean reachedNewVertex = true;
    private short id;
    public int lastVertex = -1;
    public int x1 = 0;  // aankomende locatie
    public int y1 = 0;
    public int x2 = 0; // vertrekkende locatie
    public int y2 = 0;
    private double movefloat = 0;
    public short direction;
    private boolean limitedMovement;
    private int itemID = 0;
    public double speed = 0.5;
    private short vistype;
    public short state = -1; // afraid, scatter or chase
    private double waitTime;

    // Constructor
    public Agent(int xPos, int yPos, double movingspeed, int currentitemID, short visType, boolean limitedMovement) {
        x1 = xPos;
        y1 = yPos;
        x2 = xPos;
        y2 = yPos;
        this.speed = movingspeed;
        this.itemID = currentitemID;
        this.vistype = visType;
        this.waitTime = 0;
        this.limitedMovement = limitedMovement;
    }

    public void setVisType(short type) {
        this.vistype = type;
    }

    public short getId() {
        return id;
    }

    // Getters 
    public int getX1() {
        return x1;
    }

    public int getX2() {
        return x2;
    }

    public int getY1() {
        return y1;
    }

    public int getY2() {
        return y2;
    }

    public short getDirection() {
        return direction;
    }

    public double getMovefloat() {
        return movefloat;
    }

    public short getState() {
        return state;
    }

    public double getSpeed() {
        return this.speed;
    }

    // Setters
    public void setDirection(short direction) {
        this.direction = direction;
    }

    public void setSpeed(double speed) {
        this.speed = speed;
    }

    public void setId(short id) {
        this.id = id;
    }

    public void setPos(int[] pos) {
        this.x1 = pos[0];
        this.y1 = pos[1];
        updateOldPos();
    }

    public double getWaitTime() {
        return waitTime;
    }

    public void setWaitTime(double waitTime) {
        this.waitTime = waitTime;
    }

    // Data functions
    public short move(short direction, Environment e) {
        if (waitTime > 0) {
            waitTime--;
            this.direction = 0;
            return this.direction;
        } else {
            waitTime = 0;
        }
        
        reachedNewVertex = false; 
            if (limitedMovement == false) {
                if (e.isVertex((short)x1, (short)y1) && lastVertex != e.getKey(x1, y1)) {
                reachedNewVertex = true;
                lastVertex = e.getKey(x1, y1);
            }   
            }

        if (this.state == Globals.GHOST_AFRAID) {
            movefloat += Globals.effectPowerpill * speed;
        } else {
            movefloat += speed;
        }

        if (movefloat <= 1) {
                if (limitedMovement == false && Globals.boldExploration == false &&
                        ((this.direction == Globals.ACTION_LEFT && direction == Globals.ACTION_RIGHT) ||
                        (this.direction == Globals.ACTION_RIGHT && direction == Globals.ACTION_LEFT) ||
                        (this.direction == Globals.ACTION_UP && direction == Globals.ACTION_DOWN) ||
                        (this.direction == Globals.ACTION_DOWN && direction == Globals.ACTION_UP))) {
  
                    // agent has apparently suddenly changed direction
                    int temp;
                    temp = x1;
                    x1 = x2;
                    x2 = temp;
                    temp = y1;
                    y1 = y2;
                    y2 = temp;
                    movefloat = 1 - movefloat;
                    this.direction = direction;
                }
                
                // moving towards newPos
                // update visualization
                if (Globals.showVisualisation == true) {
                    drawItems(e);
                }
                return this.direction;
        } else {
            this.direction = direction;

            // arrived at newPos
            movefloat--;

            // update oldPos and newPos            
            switch (direction) {
                case Globals.ACTION_DOWN:
                    updateOldPos();
                    y1++;
                    break;

                case Globals.ACTION_UP:
                    updateOldPos();
                    y1--;
                    break;

                case Globals.ACTION_LEFT:
                    updateOldPos();
                    x1--;
                    break;

                case Globals.ACTION_RIGHT:
                    updateOldPos();
                    x1++;
                    break;
            }

            // if you're trying to move through a wall, 
            if (e.maze[x1][y1] == Globals.MAZE_WALL) {
                x1 = x2;
                y1 = y2;
            }

            // check for teleport
            teleport(direction, e);
        }

        // update visualization
        if (Globals.showVisualisation == true) {
            drawItems(e);
        }
        return direction;
    }

    private void teleport(short direction, Environment e) {
        // checks if teleport is needed 
        // and if needed, teleports agent to opposite of the board
        boolean teleported = false;
        if (x1 <= 0) {
            x1 = e.mazeWidth - 1;
            updateOldPos();
            x1 = (e.mazeWidth - 2);
            teleported = true;
        } else if (x1 >= e.mazeWidth - 1) {
            x1 = 0;
            updateOldPos();
            x1 = 1;
            teleported = true;
        }
    }

    public void drawItems(Environment e) {
        if (Globals.showVisualisation == true) {
            e.vis.editItem(this.itemID, this.vistype, this.x1, this.y1, this.x2, this.y2, this.movefloat);
        }
    }

    private void updateOldPos() {
        // arrived at newPos!
        // update oldPos to newPos
        x2 = x1;
        y2 = y1;
    }
}