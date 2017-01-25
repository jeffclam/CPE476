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
    
    for(int i = 0; i < objs.size(); i++) {
        objs[i].update(time);
    }
}

void WorldObj::addObj(GameObj newObj) {
    newObj.worldObjs = &objs;
    objs.push_back(newObj);
}
