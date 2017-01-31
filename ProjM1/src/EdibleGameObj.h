#ifndef Edible_h
#define Edible_h

#include "GameObj.h"

class EdibleGameObj : public GameObj {
public:
    EdibleGameObj(shared_ptr<Shape> shape, Texture *tex);
    //virtual ~EdibleGameObj();
    virtual void update(GameState state);
    void eat(GameState stat);
private:
    float oldX;
    float theta;
};

#endif
