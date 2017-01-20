//
//  camera.cpp
//  Lab01
//
//  Created by Andrew Adriance on 1/17/17.
//
//

#include "camera.h"

Camera::Camera():
    eyePt(vec3(0,1.5,0)),
    gaze(vec3(0,0,0)),
    up (vec3(0,1,0)),
    oldX(0),
    oldY(0),
    strafe(0),
    dolly(0),
    theta(M_PI),
    phi(0)
{}

vec3 Camera::getW() {
    return normalize(-gaze);
}

vec3 Camera::getU() {
    return cross(up, getW());
}

vec3 Camera::getV() {
    return cross(getW(), getU());
}

void Camera::handleInputKey(int key, int action) {
    if(key == GLFW_KEY_A && action == GLFW_PRESS) {
        strafe = -1;
    }
    if(key == GLFW_KEY_D && action == GLFW_PRESS) {
        strafe = 1;
    }
    if((key == GLFW_KEY_D  || key == GLFW_KEY_A) && action == GLFW_RELEASE) {
        strafe = 0;
    }
    if(key == GLFW_KEY_W && action == GLFW_PRESS) {
        dolly = -1;
    }
    if(key == GLFW_KEY_S && action == GLFW_PRESS) {
        dolly = 1;
    }
    if((key == GLFW_KEY_W  || key == GLFW_KEY_S) && action == GLFW_RELEASE) {
        dolly = 0;
    }
}

void Camera::walk(double delta) {
    eyePt += getW() * dolly  * ((float)5) * (float)delta;
    eyePt += getU() * strafe * ((float)5) * (float)delta;
    eyePt[1] = 1.5;
}

void Camera::handleInputMouse(double xPos, double yPos, int g_width, int g_height){
    if(oldX == oldY && oldY == 0) {
        oldX = xPos;
        oldY = yPos;
    }
    theta += (xPos-oldX)/g_width;
    float y = sin(phi);
    float shiftY = (yPos-oldY)/g_height;
    if ((y < 0.95 && y > -0.95) || (shiftY/abs(shiftY) == phi/abs(phi))) {
        phi -= shiftY * 2;
    }
    
    oldX = xPos;
    oldY = yPos;
}

tmat4x4<float> Camera::getLookAt() {
    float x,y,z;
    
    x = cos(phi) * cos(theta);
    y = sin(phi);
    z = cos(phi) * cos(M_PI/2 - theta);
    
    gaze = vec3(x,y,z) + eyePt - eyePt;
    return lookAt(eyePt, vec3(x,y,z) + eyePt, up);
}
