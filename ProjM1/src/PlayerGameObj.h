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

#define SPRINT_MAX .5
#define SPRINT_MIN -.5

#define MAX_SPRINT_TIME 4
#define EXHAUST_TIMER_MULTIPLIER (MAX_SPRINT_TIME + 3.5)

#include "GameObj.h"
#include "EnemyGameObj.h"
#include "SupportGameObj.h"

class PlayerGameObj: public GameObj {
public:
    PlayerGameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex);
    virtual ~PlayerGameObj();
    virtual void update(GameState *state);
    void push(GameState *state);
    void sprinkler(GameState *state);
    float stamina = SPRINT_MAX;
    float exhaustionTimer = 0;
private:
    float oldX;
    float theta;
    bool isScaring = false;
    bool sawFirstSheep = false;
    bool sawFirstEat = false;
    bool wasNearDead = false;
    bool hasWon = false;
    bool hasLost = false;
    void *snd = 0;
    float sndTimer = 5.0;
    int sndNum = 0;
};

#endif /* PlayerGameObj_h */
