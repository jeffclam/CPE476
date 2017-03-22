#pragma once

#include "EdibleGameObj.h"
#include "EnemyCharModel.h"
#include "GameObj.h"

class EnemyGameObj: public GameObj {
public:
    EnemyGameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex);
    vec3 setRandomVel();
    virtual void update(GameState *state);
    void scare(GameObj *scarer);
    shared_ptr<CharModel> normal;
    shared_ptr<CharModel> scared;
    shared_ptr<CharModel> walking;
    bool isScared = false;
private:
    bool was_pushed;
    bool canEatCell(GridCell *cell);
    vec3 nextGoal = vec3(-1,-1,-1);
    GridCell *goal = NULL;
    EdibleGameObj *grass;
    bool scareMotion = false;
    bool isLeaving = false;
    float partCountDown = 0;
    float sndTimer = 0;
};
