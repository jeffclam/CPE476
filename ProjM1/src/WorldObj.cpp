//
//  WorldObject.cpp
//  Lab01
//
//  Created by Andrew Adriance on 1/19/17.
//
//

#include "WorldObj.h"

double updateTime = 0.0;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(-30, 30);

WorldObj::WorldObj() :
    objs(),
    state(),
    cam()
{
}

WorldObj::~WorldObj(){

}

void WorldObj::render(shared_ptr<Program> prog) {
    
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(cam.getLookAt()));
    for(int i = 0; i < objs.size(); i++) {
        objs[i]->render(prog);
    }
    grid.renderGrid(prog);
}

void WorldObj::update(double time) {
    updateTime += time;
    //update game state
    state.deltaTime = (float) time;
    state.worldTime += (float) time;
    state.timeSinceSpawn += (float) time;
    if(state.timeSinceSpawn > state.timeBetweenSpawn) {
        state.timeSinceSpawn = 0;

        EdibleGameObj *ed = new EdibleGameObj(objs[2]->shape, objs[2]->texture);
        ed->setPos(dis(gen), 0, 50);
        spawnGrass(ed);

        EnemyGameObj *e = new EnemyGameObj(objs[3]->shape, objs[3]->texture);
        e->setPos(dis(gen), 0, -50);
        spawnEnemy(e);
        e->setRandomVel(edibles);
    }
    glfwGetCursorPos(state.window, &(state.mouseX), &(state.mouseY));

    for(int i = 0; i < objs.size(); i++) {
        objs[i]->update(state);
    }

    for (int j = 0; j < edibles.size(); j++) {
        edibles[j]->update(state);
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

void WorldObj::spawnGrass(EdibleGameObj *e) {
    addObj(e);
    edibles.push_back(e);
}

void WorldObj::spawnEnemy(EnemyGameObj *e) {
     addObj(e);
     enemies.push_back(e);
}
