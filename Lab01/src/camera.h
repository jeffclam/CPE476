//
//  camera.h
//  Lab01
//
//  Created by Andrew Adriance on 1/16/17.
//
//

#ifndef camera_h
#define camera_h

#define GLEW_STATIC

#define _USE_MATH_DEFINES

#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

class Camera {
public:
    Camera();
    vec3 getW();
    vec3 getU();
    vec3 getV();
    void walk(double delta);
    void handleInputKey(int key, int action);
    void handleInputMouse(double xPos, double yPos, int g_width, int g_height);
    tmat4x4<float> getLookAt();
    vec3 eyePt;
private:
    vec3 gaze;
    vec3 up;
    float oldX;
    float oldY;
    float strafe;
    float dolly;
    float theta;
    float phi;
};

#endif /* camera_h */
