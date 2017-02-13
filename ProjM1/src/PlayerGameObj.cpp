//
//  PlayerGameObj.cpp
//  ProjM1
//
//  Created by Andrew Adriance on 1/28/17.
//
//

#include <stdio.h>
#include "PlayerGameObj.h"

void PlayerGameObj::update(GameState *state) {
    vec3 startPos = getPos();
    int height, width, speed = 0;
    grid->removeFromGrid(this);
    if(oldX == 0) {
        oldX = state->mouseX;
    }
    glfwGetWindowSize(state->window, &width, &height);
    theta += (state->mouseX-oldX)/width;
    oldX = state->mouseX;
    if(glfwGetKey(state->window, GLFW_KEY_W) == GLFW_PRESS) {
        speed = 2;
    } else if(glfwGetKey(state->window, GLFW_KEY_S) == GLFW_PRESS) {
        speed = -2;
    } else {
        setVel(0, getVel()[1], 0);
    }

    setVel(sin(theta) * speed, getVel()[1], cos(theta) * speed);
    if (glfwGetKey(state->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        isJumping = true;
        push();
    }
    //strafing
    if(glfwGetKey(state->window, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 oldPos = getPos();
        vec3 strafe = cross(vec3(0,1,0), -normalize(vec3(sin(theta), getVel()[1], cos(theta))));
        strafe = strafe * ((float)10*state->deltaTime);
        setPos(oldPos[0]-strafe[0], oldPos[1], oldPos[2]-strafe[2]);
    } 
    if(glfwGetKey(state->window, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 oldPos = getPos();
        vec3 strafe = cross(vec3(0,1,0), -normalize(vec3(sin(theta), getVel()[1], cos(theta))));
        strafe = strafe * ((float)10*state->deltaTime);
        setPos(oldPos[0]+strafe[0], oldPos[1], oldPos[2]+strafe[2]);
    } 
    //jump(state);
    pos += getVel()*((float)5*state->deltaTime);

    GameObj *collider = check_Collision_Radius(worldObjs);
    if(collider != NULL && collider->getName() != "grass") {
        pos = startPos;
    }
    setRot(0, theta, 0);
    grid->addToGrid(this);
}

PlayerGameObj::~PlayerGameObj() {
    
}

PlayerGameObj::PlayerGameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex) :
    GameObj(shape, tex)
{
    oldX = 0.0;
    theta = M_PI;
    name = "player";
}

void PlayerGameObj::jump(GameState state) {
    if(getVel()[1] != 0 && isJumping && getPos()[1] <= 0) {
        setPos(getPos()[0], 0, getPos()[2]);
        setVel(getVel()[0], 0, getVel()[2]);
        isJumping = false;
        return;
    }
    if(getVel()[1] == 0 && isJumping) {
        setVel(getVel()[0], 0.3, getVel()[2]);
    }
    if(isJumping && getPos()[1] > 1) {
        setVel(getVel()[0], -0.3, getVel()[2]);
    }
}

void PlayerGameObj::push() {
    EnemyGameObj *enemy;
    for (int i = 0; i < (*worldObjs).size(); i++) {
        enemy = (EnemyGameObj *)(*worldObjs)[i];
        if (enemy->getName() == "enemy") {
            if (check_Interact_Radius(*enemy)) {
                enemy->scare(this);
            }
        }
    }
}
