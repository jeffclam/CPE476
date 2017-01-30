//
//  PlayerGameObj.cpp
//  ProjM1
//
//  Created by Andrew Adriance on 1/28/17.
//
//

#include <stdio.h>
#include "PlayerGameObj.h"

void PlayerGameObj::update(GameState state) {
    int height, width, speed = 0;
    if(oldX == 0) {
        oldX = state.mouseX;
    }
    glfwGetWindowSize(state.window, &width, &height);
    theta += (state.mouseX-oldX)/width;
    oldX = state.mouseX;
    if(glfwGetKey(state.window, GLFW_KEY_W) == GLFW_PRESS) {
        speed = 1;
    } else if(glfwGetKey(state.window, GLFW_KEY_S) == GLFW_PRESS) {
        speed = -1;
    } else {
        setVel(0, 0, 0);
    }
    setVel(sin(theta) * speed, 0, cos(theta) * speed);
    pos += getVel()*((float)5*state.deltaTime);
    setRot(0, theta, 0);
}

PlayerGameObj::~PlayerGameObj() {
    
}

PlayerGameObj::PlayerGameObj(shared_ptr<Shape> shape, Texture *tex) :
    GameObj(shape, tex)
{
    oldX = 0.0;
    theta = M_PI;
}

void PlayerGameObj::push(GameState state, vector<EnemyGameObj *> e) {
    if (glfwGetKey(state.window, GLFW_KEY_SPACE) == GLFW_PRESS) {    
        for (int i = 0; i < e.size(); i++) {
            if (check_Interact_Radius(*(e[i]))) {
                //e[i]->pushed();
            }
        }
    }
}
