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
    setSndPos(pos, rot);
    int height, width, speed = 0;
    grid->removeFromGrid(this);
    if(oldX == 0) {
        oldX = state->mouseX;
    }
    glfwGetWindowSize(state->window, &width, &height);
    theta -= (state->mouseX-oldX)/width;
    oldX = state->mouseX;

    if (glfwGetMouseButton(state->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || glfwGetKey(state->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        isScaring = true;
        push();
        getModel()->scare_Motion();
    }

    if (isScaring) {
        isScaring = getModel()->scare_Motion();
    } else {
        /* walking */
        if (glfwGetKey(state->window, GLFW_KEY_W) == GLFW_PRESS) {
            getModel()->walk_Motion();
            speed = 1.5;
        }
        else if (glfwGetKey(state->window, GLFW_KEY_S) == GLFW_PRESS) {
            getModel()->walk_Motion();
            speed = -1.5;
        }
        else {
            setVel(0, getVel()[1], 0);
        }
        /* strafing */
        if (glfwGetKey(state->window, GLFW_KEY_A) == GLFW_PRESS) {
            getModel()->walk_Motion();
            vec3 oldPos = getPos();
            vec3 strafe = cross(vec3(0, 1, 0), -normalize(vec3(sin(theta), getVel()[1], cos(theta))));
            strafe = strafe * ((float)2.5 * state->deltaTime);
            setPos(oldPos[0] - strafe[0], oldPos[1], oldPos[2] - strafe[2]);
        }
        if (glfwGetKey(state->window, GLFW_KEY_D) == GLFW_PRESS) {
            getModel()->walk_Motion();
            vec3 oldPos = getPos();
            vec3 strafe = cross(vec3(0, 1, 0), -normalize(vec3(sin(theta), getVel()[1], cos(theta))));
            strafe = strafe * ((float)2.5 * state->deltaTime);
            setPos(oldPos[0] + strafe[0], oldPos[1], oldPos[2] + strafe[2]);
        }
        if (glfwGetKey(state->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            if (sprintEnergy > 0)
                speed *= sprintEnergy;
            if (glfwGetKey(state->window, GLFW_KEY_W | GLFW_KEY_A | GLFW_KEY_S | GLFW_KEY_D)) {
                getModel()->walk_Motion();
            }
        }
        setVel(sin(theta) * speed, getVel()[1], cos(theta) * speed);
        pos += getVel()*((float)5 * state->deltaTime);
    }

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
