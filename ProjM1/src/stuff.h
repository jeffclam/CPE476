#pragma once
#ifndef _STUFF_H_
#define _STUFF_H_

#define MAT_NUM 5

#include <iostream>
#include <unordered_map>
#include <fstream>


#include "Shape.h"
#include "Texture.h"
#include "json/json.h"

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "GLSL.h"
#include "Program.h"
#include <vector>

using namespace glm;
using namespace Json;
using namespace std;

void loadStuff(string resourceDir, string stuffFile);
shared_ptr<Shape> getShape(string name);
shared_ptr<Texture> getTexture(string name);
void setMat(int mat, shared_ptr<Program> prog);




#endif
