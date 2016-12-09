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
 *  VERTEX
 *  This class is called to hold information on a single vertex, which is either
 *  a corner or intersection in the maze.
 * 
 */

package environment;

import java.util.ArrayList;

public class Vertex {

    private short ID;
    private short x;
    private short y;
    private int f_score;
    private short[][] neighbors;

    public Vertex(short x, short y) {
        this.x = x;
        this.y = y;
    }

    public Vertex(short ID, short x, short y) {
        this.ID = ID;
        this.x = x;
        this.y = y;
    }

    public Vertex(short ID, short x, short y, int f_score) {
        this.ID = ID;
        this.x = x;
        this.y = y;
        this.f_score = f_score;
    }
    
    public short getID() {
        return ID;
    }

    public short getX() {
        return x;
    }

    public short getY() {
        return y;
    }

    public int getF_score() {
        return f_score;
    }
    
    public short[][] getNeighbors() {
        return neighbors;
    }

    public void setX(short x) {
        this.x = x;
    }

    public void setY(short y) {
        this.y = y;
    }

    public void setNeighbors(short[][] neighbors) {
        this.neighbors = neighbors;
    }

    public void setF_score(int f_score) {
        this.f_score = f_score;
    }
    
    @Override
    public boolean equals(Object other) {
        Vertex v = (Vertex) other;
        if (v.getX() == this.x && v.getY() == this.y) {
            return true;
        }
        return false;
    }

    @Override
    public Vertex clone() {
        return new Vertex(this.ID, this.x, this.y, this.f_score);
    }
}
