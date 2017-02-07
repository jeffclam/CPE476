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
            grid[r][c].setXPos(r * offset);
            grid[r][c].setYPos(c * offset);
        }
    }
}