#pragma once
#ifndef _GAMEOBJ_H_
#define _GAMEOBJ_H_

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

struct Bounding_Sphere
{
    vec3 center;
    float radius;
}

class GameObj
{
public:
    GameObj();
    ~GameObj();
    void update();
    void render();
    shared_ptr<MatrixStack> getM(shared_ptr<MatrixStack> M);
    void setPos(float x, float y, float z);
    void setScale(float x, float y, float z);
    void setRot(float x, float y, float z);
    void calcBoundingSphere();
    bool check_Interact_Radius();
    bool check_Collision_Radius();
    int mat;
    vec3 vel;
    vec3 pos; //x y z pos
    vec3 scale; //x y z scale
    vec3 rot; //x y z rotation
    
private:
   Bounding_Sphere bsphere;
};

#endif