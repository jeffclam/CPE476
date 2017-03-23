#ifndef SupportGameObj_h
#define SupportGameObj_h

#define _USE_MATH_DEFINES
#include <cmath>

#include "GameObj.h"
#include "EnemyGameObj.h"

#define SPRINKLER_MIN_HEIGHT 1.01

#define SPRAY_TIME 1.5

class SupportGameObj: public GameObj {
public:
    SupportGameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex);
    void update(GameState *state);
    void sprinkle(GameState *state);
    void spray(GameState *state);
private:
    float sprayTime = 0;
};

#endif
