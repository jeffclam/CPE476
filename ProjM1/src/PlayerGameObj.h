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

#include "GameObj.h"
#include "EnemyGameObj.h"

class PlayerGameObj: public GameObj {
public:
    PlayerGameObj(shared_ptr<Shape> shape, Texture *tex);
    virtual ~PlayerGameObj();
    virtual void update(GameState state);
    void push(GameState state, vector<EnemyGameObj *> e);
private:
    float oldX;
    float theta;
};

#endif /* PlayerGameObj_h */
