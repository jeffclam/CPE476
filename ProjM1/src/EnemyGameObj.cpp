#include "EnemyGameObj.h"
#include "EdibleGameObj.h"

EnemyGameObj::EnemyGameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex) : GameObj(shape, tex) {
    oldx = 0;
    theta = M_PI;
    was_pushed = false;
    name = "enemy";
}

vec3 EnemyGameObj::setRandomVel() {
    GameObj *target;
    int dist = 99999, current_dist;
    vec3 velocity = vec3(0, 0, 0);

    for (int i = 1; i < (worldObjs[0]).size(); i++) {
        if ((*worldObjs)[i]->name == "grass") {
            current_dist = distance(getPos(), (*worldObjs)[i]->getPos());
            if (current_dist < dist) {
                dist = current_dist;
                target = (*worldObjs)[i];
                velocity = target->getPos() - getPos();
            }
        }
    }

    velocity[1] = 0;
    velocity *= .05;
    setVel(velocity);
    return velocity;
}

void EnemyGameObj::update(GameState state) {
    float time = state.deltaTime;

    if (vel[0] != 0 || vel[2] != 0) {
        rot[1] = atan2(vel[0], vel[2]);
    }

    GameObj *collider = check_Collision_Radius();

    if (collider != NULL && collider->name != "ground" && !was_Pushed) {
        setVel(0, 0, 0);
    }
    else if (getVel() == vec3(0,0,0) && !was_Pushed) {
        setRandomVel();
    }
    pos += getVel()*((float)5 * time);
}

void EnemyGameObj::pushed() {
    was_pushed = true;
    noInteract = "enemy";
    setVel(vec3(0, 0, -5));
    pos += getVel();
}
