#pragma once
#ifndef _THING_H_
#define _THING_H_

#include <vector>

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "json/json.h"
#include "MatrixStack.h"

using namespace glm;
using namespace Json;
using namespace std;


class Thing
{
public:
    Thing(string name);
    shared_ptr<MatrixStack> getM(shared_ptr<MatrixStack> M);
    const string &getName() const {return name; };
    void setPos(float x, float y, float z);
    void setScale(float x, float y, float z);
    void setRot(float x, float y, float z);
    void setName(string newName);
    Json::Value getSave();
    void setFromSave(Json::Value root);
    Thing copy();
    int mat;
    vec3 pos; //x y z pos
    vec3 scale; //x y z scale
    vec3 rot; //x y z rotation
    
private:
    string name; //thing name
};

#endif
