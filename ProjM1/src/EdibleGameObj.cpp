#include "EdibleGameObj.h"

EdibleGameObj::EdibleGameObj(shared_ptr<Shape> shape, Texture *tex) : 
    GameObj(shape, tex) {
    oldX = 0;
    theta = M_PI;
    is_Edible = true;
    name = "grass";
    noInteract = "grass";
};

void EdibleGameObj::update(GameState state) {
    GameObj::update(state);
    GameObj *collider = check_Collision_Radius();
    if(collider != NULL && collider->name == "enemy" and scale[1] > 0) {
        eat(state);
    }
        
}

void EdibleGameObj::eat(GameState stat) {
    setScale(scale[0], scale[1] - (stat.deltaTime/10), scale[2]);
    setPos(pos[0], pos[1] - (stat.deltaTime/10), pos[2]);
}
