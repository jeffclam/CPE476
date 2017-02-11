//
//  WorldObject.cpp
//  Lab01
//
//  Created by Andrew Adriance on 1/19/17.
//
//
#include "stuff.h"
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
    grid.renderGrid(prog);
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(cam.getLookAt()));
    for(int i = 0; i < objs.size(); i++) {
        objs[i]->render(prog, (lastRendered != objs[i]->name));
    }
}

void WorldObj::update(double time) {
    updateTime += time;
    //update game state
    state.deltaTime = (float) time;
    state.worldTime += (float) time;
    state.timeSinceSpawn += (float) time;
    if(state.timeSinceSpawn > state.timeBetweenSpawn) {
        state.timeSinceSpawn = 0;

        /*EdibleGameObj *ed = new EdibleGameObj(objs[2]->shape, objs[2]->texture);
        ed->setPos(dis(gen), 0, 50);
        addObj(ed);

        EnemyGameObj *e = new EnemyGameObj(objs[3]->shape, objs[3]->texture);
        e->setPos(dis(gen), 0, -50);
        addObj(e);
        e->setRandomVel();*/
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
    newObj->grid = &grid;
}

void WorldObj::setWindows(GLFWwindow *win) {
    state.window = win;
}

void WorldObj::makeFence(int row, int col){
    for(int r = 0; r <= row; r++){
        GameObj *fence = new GameObj(getShape("cube"), getTexture("fence"));
        fence->setPos(r * grid.offset, 2, col * grid.offset);
        fence->setScale(1.5,1,0.5);
        addObj(fence);
        grid.addToGrid(fence);
    }
    for(int c = 0; c <= col; c++){
        if(c != col/2 && c!= col/2 + 1) {
            GameObj *fence = new GameObj(getShape("cube"), getTexture("fence"));
            fence->setPos(row * grid.offset, 2, c * grid.offset);
            fence->setScale(0.5,1,1.5);
            fence->name = "fence";
            addObj(fence);
            grid.addToGrid(fence);
        }
    }
    for(int r = 0; r < row; r++) {
        for(int c = 0; c < col; c++) {
            EdibleGameObj *grass = new EdibleGameObj(getShape("grass"), getTexture("grassUV"));
            grass->setPos(r * grid.offset, 2, c * grid.offset);
            addObj(grass);
            grid.addToGrid(grass);
        }
    }
}
