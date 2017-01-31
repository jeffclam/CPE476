//
//  ManualCamera.cpp
//  ProjM1
//
//  Created by Andrew Adriance on 1/27/17.
//
//

#include "ManualCamera.h"
#include <stdio.h>

ManualCamera::ManualCamera():
eyePt(vec3(0,1.5,0)),
lookAtPt(vec3(4,1.5,0)),
avatar(NULL)
{}

void ManualCamera::updateCamera() {
    /*vec3 vel = avatar->getVel();
    if(vel[0] != vel[2] && vel[0] != 0) {
        vel = normalize(vel);
    } else {
        vel = vec3(sin(avatar->rot[1]),0,cos(avatar->rot[1]));
    }
    vec3 goal = avatar->getPos() - vel * (float)5.0;
    float deltaX = 0.1;
    float deltaZ = 0.1;
    if(deltaX > abs(goal[0] - eyePt[0])) {
        deltaX = goal[0] - eyePt[0];
    } else {
        deltaX *= abs(goal[0] - eyePt[0])/(goal[0] - eyePt[0]) * abs(goal[0] - eyePt[0]);
    }
    if(deltaZ > abs(goal[2] - eyePt[2])) {
        deltaZ = goal[2] - eyePt[2];
    } else {
        deltaZ *= abs(goal[2] - eyePt[2])/(goal[2] - eyePt[2]) * abs(goal[2] - eyePt[2]);
    }
     eyePt[0] += deltaX;
     eyePt[1] = avatar->getPos()[1] + 2;
     eyePt[2] += deltaZ;
     */
    
    float theta = (avatar->rot[1] + oldTheta * 8)/9;
    float r = 4;
    /*if(avatar->rot[1] - theta < M_PI/10) {
        theta = avatar->rot[1] - theta;
    }*/
    oldTheta = avatar->rot[1];
    
    eyePt[0] = -r * sin(theta) + avatar->getPos()[0] - avatar->getVel()[0]/7;
    eyePt[1] = avatar->getPos()[1] + 2;
    eyePt[2] = -r * cos(theta) + avatar->getPos()[2] - avatar->getVel()[2]/7;
    lookAtPt = avatar->getPos();
}

tmat4x4<float> ManualCamera::getLookAt() {
    
    return lookAt(eyePt, lookAtPt, vec3(0,1,0));
}

