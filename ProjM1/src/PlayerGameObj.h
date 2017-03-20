//
//  PlayerGameObj.h
//  ProjM1
//
//  Created by Andrew Adriance on 1/28/17.
//
//

#ifndef PlayerGameObj_h
#define PlayerGameObj_h

#define _USE_MATH_DEFINES
#include <cmath>

#define SPRINT_MAX 50000

#include "GameObj.h"
#include "EnemyGameObj.h"

class PlayerGameObj: public GameObj {
public:
    PlayerGameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex);
    virtual ~PlayerGameObj();
    virtual void update(GameState *state);
    void push();
private:
    float oldX;
    float theta;
    float sprintEnergy = 2;
    bool isScaring = false;
};

#endif /* PlayerGameObj_h */
