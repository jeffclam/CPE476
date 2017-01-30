//
//  WorldObject.cpp
//  Lab01
//
//  Created by Andrew Adriance on 1/19/17.
//
//

#include "WorldObj.h"

double updateTime = 0.0;

WorldObj::WorldObj() :
    objs(),
    state(),
    cam()
{}

WorldObj::~WorldObj(){

}

void WorldObj::render(shared_ptr<Program> prog) {
    
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(cam.getLookAt()));
    for(int i = 0; i < objs.size(); i++) {
        objs[i]->render(prog);
    }
}

void WorldObj::update(double time) {
    updateTime += time;
    //update game state
    state.deltaTime = (float) time;
    state.worldTime += (float) time;
    glfwGetCursorPos(state.window, &(state.mouseX), &(state.mouseY));

    for(int i = 0; i < objs.size(); i++) {
        objs[i]->update(state);
    }
    
    //update camera
    cam.avatar = objs[0];
    cam.updateCamera();
}

void WorldObj::addObj(GameObj *newObj) {
    newObj->worldObjs = &objs;
    objs.push_back(newObj);
}

void WorldObj::setWindows(GLFWwindow *win) {
    state.window = win;
}

void WorldObj::growGrass(vector<EdibleGameObj *> e) {
    for (int i = 0; i < e.size(); i++) {
        addObj(e[i]);
    }
}

void WorldObj::spawnEnemy(vector<EnemyGameObj *> e) {
    for (int i = 0; i < e.size(); i++) {
        addObj(e[i]);
    }
}