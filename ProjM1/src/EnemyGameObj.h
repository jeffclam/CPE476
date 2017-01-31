#pragma once

#include "EdibleGameObj.h"
#include "GameObj.h"

class EnemyGameObj: public GameObj {
public:
    EnemyGameObj(shared_ptr<Shape> shape, Texture *tex);
    //virtual ~EnemyGameObj();
    vec3 setRandomVel(vector<EdibleGameObj *> e);
    void pushed();
private:
    float oldx;
    float theta;
    bool was_pushed;
};