#include "EdibleGameObj.h"

EdibleGameObj::EdibleGameObj(shared_ptr<Shape> shape, Texture *tex) : 
    GameObj(shape, tex) {
    oldX = 0;
    theta = M_PI;
    is_Edible = true;
};
