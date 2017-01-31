//
//  ManualCamera.h
//  ProjM1
//
//  Created by Andrew Adriance on 1/27/17.
//
//

#ifndef ManualCamera_h
#define ManualCamera_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GameObj.h"

using namespace std;
using namespace glm;

class ManualCamera {
public:
    ManualCamera();
    tmat4x4<float> getLookAt();
    vec3 eyePt;
    vec3 lookAtPt;
    GameObj *avatar;
    void updateCamera();
private:
    float oldTheta = 0;
};

#endif /* ManualCamera_h */
