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

void EnemyGameObj::update(GameState *state) {
    grid->removeFromGrid(this);
    if(sndTimer > 0) {
        sndTimer -= state->deltaTime;
    }
    if(partCountDown >= 0) {
        partCountDown -= state->deltaTime;
    }
    if (walking != NULL && getVel() != vec3(0, 0, 0)) {
        setModel(walking);
        getModel()->walk_Motion();
    }

    if (scareMotion) {
        setModel(scared);
        scareMotion = scared->scare_Motion();
    }
    else {
        GameObj::update(state);

        if (goal == NULL) {
            GridCell *next = grid->randomGrid();
            while (!canEatCell(next)) {
                next = grid->randomGrid();
            }
            goal = next;
            //DEBUG GOALS goal->tile->pos[1] = 1.2;
            nextGoal = grid->getNextPoint(goal, this);
        }
        else if (abs(distance(vec3(nextGoal[0], 0, nextGoal[2]), vec3(pos[0], 0, pos[2]))) < 0.1) {
            nextGoal = grid->getNextPoint(goal, this);
        }

        if (abs(distance(vec3(goal->xPos, pos[1], goal->yPos), getPos())) > 0.1) {
            vec3 normGoal = vec3(nextGoal[0] - pos[0], vel[1], nextGoal[2] - pos[2]);
            if (normGoal[0] != 0 || normGoal[1] != 0 || normGoal[2] != 0) {
                normGoal = normalize(normGoal);
            }
            else {
                if (!isLeaving)
                    goal = NULL;
            }
            setVel(normGoal[0] * (isScared + 1), vel[1], normGoal[2] * (isScared + 1));
        }
        else {
            setModel(normal);
            if (isLeaving) {
                dead = true;
                state->enemyCount--;
            }
            else if (isScared) {
                goal = grid->getCellFromCoords((grid->grid.size() - 1) * grid->offset, (grid->grid[0].size() - 1) * grid->offset);
                isLeaving = true;
            }
            else {
                setVel(0, 0, 0);
                if (grass->is_Edible) {
                    grass->eat(*state);
                    if(partCountDown <= 0) {
                        state->partManager->addParticleSystem(true, vec4(0.1, 0.9, 0.1, 1.0), pos);
                        partCountDown = 7.0;
                    }
                }
                else
                    goal = NULL;
            }
        }
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

void EnemyGameObj::scare(GameObj *scarer){
    if (sndTimer <= 0) {
        playSound("sheepScare", pos);
        sndTimer = 1.0;
    }
    isScared = true;
    scareMotion = true;
    isLeaving = false;
    GridCell *next = grid->randomGrid();
    while(abs(distance(vec3(pos[0], 0, pos[2]), vec3(next->xPos, 0, next->yPos))) > abs(distance(vec3(scarer->pos[0], 0, scarer->pos[2]), vec3(next->xPos, 0, next->yPos)))) {
        next = grid->randomGrid();
    }
    goal = next;
    nextGoal = grid->getNextPoint(goal, this); 
}