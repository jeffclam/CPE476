//
//  WorldObject.cpp
//  Lab01
//
//  Created by Andrew Adriance on 1/19/17.
//
//

#include "WorldObj.h"

double updateTime = 0.0;

WorldObj::WorldObj():
    objs()
{}

WorldObj::~WorldObj(){

}

void WorldObj::render(shared_ptr<Program> prog) {
    for(int i = 0; i < objs.size(); i++) {
        objs[i].render(prog);
    }
}

void WorldObj::update(double time) {
    updateTime += time;
    //printf("%f\n", updateTime);
    if (updateTime > 0.0002) {
        updateTime = 0;
        GameObj bun = GameObj(objs[0].shape);
        bun.setPos(static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 20.0 - 10.0, 0, static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 20.0 - 10.0);
        bun.setRandomVel();
        addObj(bun);
        printf("NEW BUN\n");
    }
    for(int i = 0; i < objs.size(); i++) {
        objs[i].update(time);
    }
}

void WorldObj::addObj(GameObj newObj) {
    newObj.worldObjs = &objs;
    objs.push_back(newObj);
}
