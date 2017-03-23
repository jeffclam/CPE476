#ifndef SupportGameObj_h
#define SupportGameObj_h

#define _USE_MATH_DEFINES
#include <cmath>

#include "GameObj.h"
#include "EnemyGameObj.h"

class SupportGameObj: public GameObj {
public:
    SupportGameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex);
    void sprinkle(GameState *state);
};

#endif
