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
 *  INSTANCEFIND
 *  This class is used for queuing an item for BFS.
 * 
 */

package util;

public class instanceFind {
    public int xPos;
    public int yPos;
    public short direction;
    public int offset = 1;

    public instanceFind(int xPos, int yPos, short direction) {
        this.xPos = xPos;
        this.yPos = yPos;
        this.direction = direction;
    }

    public instanceFind(int xPos, int yPos, short direction, int offset) {
        this.xPos = xPos;
        this.yPos = yPos;
        this.direction = direction;
        this.offset = offset;
    }
}