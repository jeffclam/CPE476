#include "EnemyGameObj.h"
#include "EdibleGameObj.h"

EnemyGameObj::EnemyGameObj(shared_ptr<Shape> shape, Texture *tex) : GameObj(shape, tex) {
    oldx = 0;
    theta = M_PI;
    was_pushed = false;
    name = "enemy";
}

vec3 EnemyGameObj::setRandomVel(vector<EdibleGameObj *> e) {
    EdibleGameObj *target = e[0];
    int dist = 99999, current_dist;
    vec3 velocity;

    for (int i = 0; i < e.size(); i++) {
        current_dist = distance(getPos(), e[i]->getPos());
        if (current_dist < dist) {
            dist = current_dist;
            target = e[i];
        }
    }

    velocity = target->getPos() - getPos();
    velocity[1] = 0;
    velocity *= .05;
    setVel(velocity);
    return velocity;
}

void EnemyGameObj::pushed() {
    was_pushed = true;
    //cout << "I WAS PUSHED!!!" << endl;
    setVel(vec3(0, 0, -5));
    pos += getVel();
}
