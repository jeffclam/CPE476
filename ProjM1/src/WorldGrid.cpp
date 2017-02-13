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

bool GridCell::hasCollision(GameObj *checker){
    if(contents.size() == 0)
        return false;
    GameObj *test = checker->check_Collision_Radius(&contents);
    if(test != NULL) {
        cout << "Collided with " << test->name << " \n";
        //tile->pos[1] = 1.5;
    }
    return NULL != test;
}

/**
 * WORLDGRID
 */
WorldGrid::WorldGrid(int rows, int cols) {
    grid.resize( rows , vector<GridCell>( cols ,  GridCell()) );
}

void WorldGrid::renderGrid(shared_ptr<Program> prog) {
    int r, c;
    bool first = true;
    for(r = 0; r < grid.size(); r++) {
        for(c = 0; c < grid[r].size(); c++) {
            grid[r][c].tile->render(prog, first);
            first = false;
        }
    }
}

void WorldGrid::initGrid() {
    int r, c;
    for(r = 0; r < grid.size(); r++) {
        for(c = 0; c < grid[r].size(); c++) {
            grid[r][c].tile = new GameObj(getShape("cube"), getTexture("dirt"));
            grid[r][c].tile->setScale(1.5,1,1.5);
            //grid[r][c].tile->pos[1] = (r + c)/10.0f;
            grid[r][c].idxX = r;
            grid[r][c].idxY = c;
            grid[r][c].setXPos(r * offset);
            grid[r][c].setYPos(c * offset);
        }
    }
}

GridCell *WorldGrid::getCellFromCoords(float x, float y) {
    int r = (x+0.5)/offset;
    int c = (y+0.5)/offset;
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
    GridCell *cell = getCellFromCoords(toRemove->getPos()[0], toRemove->getPos()[2]);
    if(cell == NULL)
        return;
    for(int i=0; i < cell->contents.size(); i++) {
        if(cell->contents[i] == toRemove) {
            cell->contents.erase(cell->contents.begin()+i);
            return;
        }
    }
}

GridCell *WorldGrid::randomGrid(){
    int i = rand() % grid.size();
    int j = rand() % grid[0].size();
    return &grid[i][j];
}

vector <GridCell *>WorldGrid::getNeighs(GridCell *start, int radius) {
    vector <GridCell *> arr;
    for(int i = -radius; i <= radius; i++) {
            for (int j = -radius; j <= radius; j++) {
                if(start->idxX + i > -1 && start->idxX + i < grid.size()
                    && start->idxY + j > -1 && start->idxY + j < grid[0].size()){
                        arr.push_back(&(grid[start->idxX + i][start->idxY + j]));
                }
            }
    }
    return arr;
}

bool WorldGrid::isNeighValid(GridCell *neigh, GameObj *mover){
    vector <GridCell *> close = getNeighs(neigh, 1);
    vec3 oldPos = mover->getPos();
    mover->pos[0] = neigh->xPos;
    mover->pos[2] = neigh->yPos;
    for(int i = 0; i < close.size(); i++){
        if(close[i]->hasCollision(mover)){
            cout << "Has collision " << close.size() << "\n";
            mover->pos = oldPos;
            return false;
        }
    }
    mover->pos = oldPos;
    return true;
}

vec3 WorldGrid::getNextPoint(GridCell *dest, GameObj *mover) {
    GridCell *start = getCellFromCoords(mover->pos[0], mover->pos[2]);
    int i,j, toClose, lookedAt = 0;
    vector<GridCell *> closedSet;
    vector<GridCell *> openSet;
    GridCell *current = NULL;
    start->accumulatedCost = 0;
    start->cameFrom = NULL;
    GridCell *neigh = NULL;
    openSet.push_back(start);
    //While things are in the openSet
    while(openSet.size() > 0) {
        lookedAt++;
        //find the theoretically cheapest node
        for(i = 0; i < openSet.size(); i++) {
            if(current == NULL || openSet[i]->toGoCost < current->toGoCost) {
                current = openSet[i];
                toClose = i;
            }
        }
        //remove the new current
        openSet.erase(openSet.begin() + toClose);
        closedSet.push_back(current);
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
                    current->idxY+j > -1 && current->idxY+j < grid[0].size() &&
                    neigh->isEmpty() && //isNeighValid(neigh, mover) &&
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
    cout << "No path: " << lookedAt << "\n";
    return vec3(start->idxX * offset ,0,start->idxY * offset);
}