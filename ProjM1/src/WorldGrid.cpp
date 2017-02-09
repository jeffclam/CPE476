#include "WorldGrid.h"
#include "stuff.h"

/**
 * GRIDCELLS
 */
void GridCell::setXPos(float x) {
    xPos = x;
    tile->setPos(x,tile->getPos()[1], tile->getPos()[2]);
}
void GridCell::setYPos(float y) {
    yPos = y;
    tile->setPos(tile->getPos()[0], tile->getPos()[1], y);
}
bool GridCell::isEmpty(){
    for(int i = 0; i < contents.size(); i++) {
        if(contents[i]->solid){
            return false;
        }
    }
    return true;
}

/**
 * WORLDGRID
 */
WorldGrid::WorldGrid(int rows, int cols) {
    grid.resize( rows , vector<GridCell>( cols ,  GridCell()) );
}

void WorldGrid::renderGrid(shared_ptr<Program> prog) {
    int r, c;
    for(r = 0; r < grid.size(); r++) {
        for(c = 0; c < grid[r].size(); c++) {
            grid[r][c].tile->render(prog);
        }
    }
}

void WorldGrid::initGrid() {
    int r, c;
    for(r = 0; r < grid.size(); r++) {
        for(c = 0; c < grid[r].size(); c++) {
            grid[r][c].tile = new GameObj(getShape("cube"), getTexture("grass"));
            grid[r][c].idxX = r;
            grid[r][c].idxY = c;
            grid[r][c].setXPos(r * offset);
            grid[r][c].setYPos(c * offset);
        }
    }
}

GridCell *WorldGrid::getCellFromCoords(float x, float y) {
    int r = (x/offset);
    int c = (y/offset);
    if(r >= grid.size() || c >= grid[0].size()) {
        cout << "NULL\n";
        return NULL;
    }
    return &(grid[r][c]);
}

void WorldGrid::addToGrid(GameObj *toAdd) {
    GridCell *cell = getCellFromCoords(toAdd->getPos()[0], toAdd->getPos()[2]);
    if(cell != NULL) {
        cell->contents.push_back(toAdd);
    }
}

void WorldGrid::removeFromGrid(GameObj *toRemove) {
    int i;
    GridCell *cell = getCellFromCoords(toRemove->getPos()[0], toRemove->getPos()[2]);
    for(i=0; i < cell->contents.size(); i++) {
        if(cell->contents[i] == toRemove) {
            cell->contents.erase(cell->contents.begin()+i);
            return;
        }
    }
}

//TO-DO: Andrew- Nothing blocks paths currently
vec3 WorldGrid::getNextPoint(GridCell *dest, GridCell *start) {
    int i,j;
    vector<GridCell *> closedSet;
    vector<GridCell *> openSet;
    GridCell *current = NULL;
    start->accumulatedCost = 0;
    start->cameFrom = NULL;
    GridCell *neigh = NULL;
    openSet.push_back(start);
    //While things are in the openSet
    while(openSet.size() > 0) {
        //find the theoretically cheapest node
        for(i = 0; i < openSet.size(); i++) {
            if(current == NULL || openSet[i]->toGoCost < current->toGoCost) {
                current = openSet[i];
                openSet.erase(openSet.begin() + i);
                closedSet.push_back(current);
            }
        }
        //If we are at the destination, loop back to find the next destination and return
        if(current == dest) {
            while(current->cameFrom != NULL && current->cameFrom != start) {
                current = current->cameFrom;
            }
            return vec3(current->idxX * offset, 0, current->idxY * offset);
        }
        //look around at neighs to populate the openSet
        for(i = -1; i < 2; i++) {
            for (j = -1; j < 2; j++) {
                neigh = &grid[current->idxX+i][current->idxY+j];
                //check if neigh is a valid location, and that it's not in the open set
                if (neigh != current && current->idxX+i > -1 && current->idxX+i < grid.size() &&
                    current->idxY+j > -1 && current->idxY+j < grid[0].size() && neigh->isEmpty() &&
                    none_of(closedSet.begin(), closedSet.end(), [=](GridCell *g){return g==neigh;})) {
                    //If it's not already in the openSet add it
                    if (none_of(openSet.begin(), openSet.end(), [=](GridCell *g){return g==neigh;})) {
                        neigh->toGoCost = abs(distance(vec3(dest->idxX,0,dest->idxY), vec3(neigh->idxX,0,neigh->idxY)));
                        neigh->cameFrom = current;
                        neigh->accumulatedCost = current->accumulatedCost + 1;
                        openSet.push_back(neigh);
                    } else {
                        //if it was in the open set, update if we found a cheaper path
                        if(neigh->accumulatedCost > current->accumulatedCost + 1) {
                           neigh->cameFrom = current;
                           neigh->accumulatedCost = current->accumulatedCost + 1; 
                        }
                    }
                }
            }
        }
        //empty out current since we are done with it
        current = NULL;
    }
    //if no path was found, return the start node as the next destination
    return vec3(start->idxX * offset ,0,start->idxY * offset);
}