#ifndef GAMEOBJPATHER_H
#define GAMEOBJPATHER_H

#include "GameObj.h"

class GameObjPather: public GameObj {
    public:
    GameObjPather(shared_ptr<Shape> s,shared_ptr< Texture> tex): GameObj(s, tex){};
    virtual void update(GameState state);
    vec3 dest = vec3(9,0,9);
};

#endif