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
    if(state->deltaTime == state->worldTime) {
        snd = playSound("intro", pos);
    }
    if(state->score == 1 && !sawFirstSheep) {
        snd = playSound("firstSheep", pos);
        sawFirstSheep = true;
    }
    if(state->lawnHealth < 100.0 && !sawFirstEat) {
        snd = playSound("firstEat", pos);
        sawFirstEat = true;
    }
    if(state->lawnHealth < 60.0 && !wasNearDead) {
        snd = playSound("nearDead", pos);
        wasNearDead = true;
    }
    if(state->retireIn < 1.0/60.0 && !hasWon) {
        if(sndNum == 0)
            playSound("win1", pos);
        if(sndNum == 1)
            playSound("win2", pos);
        if(sndNum == 2)
            playSound("win3", pos);
        hasWon = true;
    }
    if(state->lawnHealth < 51.0 && !hasLost) {
        if(sndNum == 0)
            playSound("fail1", pos);
        if(sndNum == 1)
            playSound("fail2", pos);
        if(sndNum == 2)
            playSound("fail3", pos);
        hasLost = true;
    }
    updateSndPos(snd, pos);
    if(sndTimer > 0) {
        sndTimer -= state->deltaTime;
    }
    vec3 startPos = getPos();
    setSndPos(pos, rot);
    int height, width;
    float speed = 0;
    grid->removeFromGrid(this);
    if(oldX == 0) {
        oldX = state->mouseX;
    }
    glfwGetWindowSize(state->window, &width, &height);
    theta -= (state->mouseX-oldX)/width;
    oldX = state->mouseX;

    if (glfwGetMouseButton(state->window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS || glfwGetKey(state->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        isScaring = true;
        push(state);
        getModel()->scare_Motion();
    }
    if (state->waterLevel >= 100.0 && (glfwGetMouseButton(state->window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS || glfwGetKey(state->window, GLFW_KEY_Q) == GLFW_PRESS)) {
        isScaring = true;
        sprinkler(state);
        getModel()->scare_Motion();
    }

    if (isScaring) {
        isScaring = getModel()->scare_Motion();
    } else {
        /* walking */
        if (glfwGetKey(state->window, GLFW_KEY_W) == GLFW_PRESS) {
            getModel()->walk_Motion();
            speed = 1;
        }
        else if (glfwGetKey(state->window, GLFW_KEY_S) == GLFW_PRESS) {
            getModel()->walk_Motion();
            speed = -1;
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
            if (stamina > SPRINT_MIN)
                speed *= 2;
            else
                speed *= .5;

            if (glfwGetKey(state->window, GLFW_KEY_W | GLFW_KEY_A | GLFW_KEY_S | GLFW_KEY_D)) {
                getModel()->walk_Motion();
                if (stamina > SPRINT_MIN)
                    stamina -= state->deltaTime;
            } else {
                if (stamina < SPRINT_MAX)
                    stamina += state->deltaTime;
            }
        }
        else {
            if (stamina < SPRINT_MAX)
                stamina += state->deltaTime;
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
    interact_radius = 12.0f;
}

void PlayerGameObj::push(GameState *state) {
    EnemyGameObj *enemy;
    if(sndTimer <= 0) {
        if(sndNum == 0)
            snd = playSound("scare1", pos);
        if(sndNum == 1)
            snd = playSound("scare2", pos);
        if(sndNum == 2)
            snd = playSound("scare3", pos);
        sndTimer = 3.0;
        sndNum = (sndNum +1)%3;
    }
    for (int i = 0; i < (*worldObjs).size(); i++) {
        enemy = (EnemyGameObj *)(*worldObjs)[i];
        if (enemy->getName() == "enemy") {
            if (check_Interact_Radius(*enemy)) {
                if(!enemy->isScared)
                    state->waterLevel += 2.0;
                enemy->scare(this);
            }
        }
    }
}

void PlayerGameObj::sprinkler(GameState *state) {
    for (auto sprinklr : (*worldObjs)) {
        if (sprinklr->getName() == "sprinkler")
            ((SupportGameObj *)sprinklr)->sprinkle(state);
    }
}
