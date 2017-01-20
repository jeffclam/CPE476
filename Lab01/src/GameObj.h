//#pragma once
#ifndef _GAMEOBJ_H_
#define _GAMEOBJ_H_

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"

using namespace std;
using namespace glm;

struct Bounding_Sphere
{
    vec3 center;
    float radius;
};

class GameObj {
public:
    GameObj();
    GameObj(shared_ptr<Shape> shape);
    ~GameObj();
    void update(double time);
    void render(shared_ptr<Program> p);
    void setShape(shared_ptr<Shape> s);
    vec3 getPos();
    void setPos(float x, float y, float z);
    vec3 getVel();
    void setVel(float x, float y, float z);
    void setScale(float x, float y, float z);
    void setRot(float x, float y, float z);
    shared_ptr<MatrixStack> getM(shared_ptr<MatrixStack> M);
    void calcBoundingSphere();
    bool check_Interact_Radius();
    bool check_Collision_Radius();
    int mat;
    vec3 vel;
    vec3 dir;
    vec3 pos; //x y z pos
    vec3 scale; //x y z scale
    vec3 rot; //x y z rotation
    vector<GameObj>* worldObjs;
    
    
private:
   Bounding_Sphere b_sphere;
   shared_ptr<Shape> shape;
};

#endif
