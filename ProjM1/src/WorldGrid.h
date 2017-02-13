#ifndef _WORLDGRID_H_
#define _WORLDGRID_H_

#include <vector>
#include "GameObj.h"
#include "Program.h"
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class GameObj;

class GridCell {
    public:
    GridCell(){};
    vector<GameObj *> contents;
    int idxX;
    int idxY;
    GameObj *tile;
    void setXPos(float x);
    void setYPos(float y);
    bool isEmpty();
    bool hasCollision(GameObj *checker);
    //scratch space variable for A*
    int accumulatedCost = -1;
    int toGoCost = -1;
    GridCell *cameFrom = NULL;
    float xPos;
    float yPos;
    private:
};

class WorldGrid {
    public:
    WorldGrid(int rows, int cols);
    WorldGrid();
    void renderGrid(shared_ptr<Program> prog);
    void initGrid();
    void removeFromGrid(GameObj *toRemove);
    void addToGrid(GameObj *toAdd);
    bool isNeighValid(GridCell *neigh, GameObj *mover);
    GridCell *randomGrid();
    vector<vector<GridCell>> grid;
    GridCell *getCellFromCoords(float x, float y);
    vec3 getNextPoint(GridCell *dest, GameObj *mover);
    vector <GridCell *>getNeighs(GridCell *start, int radius);
    float offset = 3;
    private:
};

#endif