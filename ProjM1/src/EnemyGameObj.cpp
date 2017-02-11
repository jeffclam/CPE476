#include "EnemyGameObj.h"
#include "EdibleGameObj.h"

EnemyGameObj::EnemyGameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex) : GameObj(shape, tex) {
    was_pushed = false;
    name = "enemy";
    noInteract = "grass";
}

vec3 EnemyGameObj::setRandomVel() {
    GameObj *target;
    int dist = 99999, current_dist;
    vec3 velocity = vec3(0, 0, 0);

    for (int i = 1; i < (worldObjs[0]).size(); i++) {
        if ((*worldObjs)[i]->name == "grass") {
            current_dist = distance(getPos(), (*worldObjs)[i]->getPos());
            if (current_dist < dist) {
                dist = current_dist;
                target = (*worldObjs)[i];
                velocity = target->getPos() - getPos();
            }
        }
    }

    velocity[1] = 0;
    velocity *= .05;
    setVel(velocity);
    return velocity;
}

void EnemyGameObj::update(GameState state) {
    grid->removeFromGrid(this);
    GameObj::update(state);
    if(goal == NULL){
        GridCell *next = grid->randomGrid();
        while(!canEatCell(next)) {
            next = grid->randomGrid();
        }
        goal = next;
        nextGoal = grid->getNextPoint(goal, grid->getCellFromCoords(pos[0],pos[2]));
    }
    if(abs(distance(vec3(nextGoal[0], 0, nextGoal[2]), vec3(pos[0], 0, pos[2]))) < 0.1) {
        nextGoal = grid->getNextPoint(goal, grid->getCellFromCoords(pos[0],pos[2]));
    }
    cout << pos[0] << " " << pos[1] << " " << pos[2] << " Pos\n";
    cout << nextGoal[0] << " " << nextGoal[1] << " " << nextGoal[2] << " Goal\n";
    if(abs(distance(vec3(goal->xPos, pos[1], goal->yPos),getPos())) > 0.1) {
        vec3 normGoal = vec3(nextGoal[0] - pos[0], vel[1], nextGoal[2] - pos[2]);
        if(normGoal[0] + normGoal[1] + normGoal[2] != 0)
            normGoal = normalize(normGoal);
        setVel(normGoal[0]/1.2f, vel[1], normGoal[2]/1.2f);
        cout << "Moving\n";
    } else {
        if(grass->is_Edible)
            grass->eat(state);
        else
            goal = NULL;
        setVel(0,0,0);
        cout << "Not moving\n";
    }
    grid->addToGrid(this);
}

bool EnemyGameObj::canEatCell(GridCell *cell){
    for(int i = 0; i < cell->contents.size(); i++){
        if(cell->contents[i]->is_Edible) {
            grass = (EdibleGameObj *)cell->contents[i];
            return true;
        }
    }
    return false;
}
