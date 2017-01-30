#include "EnemyGameObj.h"
#include "EdibleGameObj.h"

EnemyGameObj::EnemyGameObj(shared_ptr<Shape> shape, Texture *tex) : GameObj(shape, tex) {
    oldx = 0;
    theta = M_PI;
    was_pushed = false;
}

vec3 EnemyGameObj::setRandomVel(vector<EdibleGameObj *> e) {
    EdibleGameObj *target;
    int dist = 99999;
    vec3 velocity;

    for (int i = 0; i < e.size(); i++) {
        if (distance(getPos(), e[i]->getPos()) < dist) {
            target = e[i];
        }
    }

    velocity = getPos() - target->getPos();
    setVel(velocity);
    return velocity;
}

void EnemyGameObj::pushed() {
    was_pushed = true;
    if (getVel() != vec3(0, 0, 0)) {
        setVel(-getVel());
    }
    else {
        setVel(vec3(-10, 0, -10));
    }
}