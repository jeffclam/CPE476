//
//  WorldObject.cpp
//  Lab01
//
//  Created by Andrew Adriance on 1/19/17.
//
//

#include "WorldObj.h"

WorldObj::WorldObj():
    objs(),
    state()
{}

WorldObj::~WorldObj(){

}

void WorldObj::render(shared_ptr<Program> prog) {
    for(int i = 0; i < objs.size(); i++) {
        objs[i]->render(prog);
    }
}

void WorldObj::update(double time) {
    
    state.deltaTime = (float) time;
    state.worldTime += (float) time;
    
    glfwGetCursorPos(state.window, &(state.mouseX), &(state.mouseY));
    
    for(int i = 0; i < objs.size(); i++) {
        objs[i]->update(state);
    }
}

void WorldObj::addObj(GameObj *newObj) {
    newObj->worldObjs = &objs;
    objs.push_back(newObj);
}

void WorldObj::setWindows(GLFWwindow *win) {
    state.window = win;
}
