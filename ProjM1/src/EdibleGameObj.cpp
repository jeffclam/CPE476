#include "EdibleGameObj.h"

EdibleGameObj::EdibleGameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex) : 
    GameObj(shape, tex) {
    is_Edible = true;
    name = "grass";
    noInteract = "grass";
    solid = false;
};

void EdibleGameObj::update(GameState state) {
    GameObj::update(state);
    GameObj *collider = check_Collision_Radius();
    if(collider != NULL && collider->name == "enemy" && scale[1] > 0.1) {
        eat(state);
    }
    is_Edible = (scale[1] > 0.1);   
}

void EdibleGameObj::eat(GameState stat) {
    setScale(scale[0], scale[1] - (stat.deltaTime/10), scale[2]);
    setPos(pos[0], pos[1] - (stat.deltaTime/10), pos[2]);
}