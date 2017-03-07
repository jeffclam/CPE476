#pragma once

#include "EdibleGameObj.h"
#include "GameObj.h"

class EnemyGameObj: public GameObj {
public:
    EnemyGameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex);
    vec3 setRandomVel();
    virtual void update(GameState *state);
    void scare(GameObj *scarer);
private:
    bool was_pushed;
    bool canEatCell(GridCell *cell);
    vec3 nextGoal = vec3(-1,-1,-1);
    GridCell *goal = NULL;
    EdibleGameObj *grass;
    bool scareMotion = false;
    bool isScared = false;
    bool isLeaving = false;
};
