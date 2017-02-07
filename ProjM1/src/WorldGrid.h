#ifndef _WORLDGRID_H_
#define _WORLDGRID_H_

#include <vector>
#include "GameObj.h"
#include "Program.h"

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
    private:
    float xPos;
    float yPos;
};

class WorldGrid {
    public:
    WorldGrid(int rows, int cols);
    WorldGrid();
    void renderGrid(shared_ptr<Program> prog);
    void initGrid();
    vector<vector<GridCell>> grid;
    private:
    float offset = 2;
};

#endif