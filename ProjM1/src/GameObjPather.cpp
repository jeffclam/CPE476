#include "GameObjPather.h"

void GameObjPather::update(GameState *state) {
    GameObj::update(state);
    GridCell *start = grid->getCellFromCoords(pos[0], pos[2]);
    if (start != NULL) {
        vec3 next = grid->getNextPoint(grid->getCellFromCoords(dest[0], dest[2]), this);
        vec3 goal = vec3(next[0] - pos[0], vel[1], next[2] - pos[2]);
        goal = normalize(goal);
        setVel(goal[0]/5.0f, vel[1], goal[2]/5.0f);
        cout << vel[0] << " " << vel[1] << " " << vel[2] << "\n";
        cout << pos[0] << "-" << vel[1] << "-" << pos[2] << "\n";
        cout << next[0] << "=" << vel[1] << "=" << next[2] << "\n";
    } else {
        cout << "NOPE\n";
        setVel(0,0,0);
    }
    //pos += getVel()*((float)5 * state.deltaTime);
}