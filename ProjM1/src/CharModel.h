#pragma once

#include <vector>
#include <memory>
#include <string>

#define _USE_MATH_DEFINES
#include <math.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"

#include "GameState.h"
#include "Texture.h"

#define TORSO 0
#define HEAD 1
#define LEFT_ARM 2
#define RIGHT_ARM 3
#define LEFT_LEG 4
#define RIGHT_LEG 5

using namespace std;

struct Part {
    shared_ptr<Shape> shape;
    shared_ptr<Texture> tex;
    vec3 offset = vec3(0,0,0);
    vec3 attach_Limb = vec3(0, 0, 0);
    vec3 rotation = vec3(0, 0, 0);
    vec3 action_rotation = vec3(1, 0, 0);
    vec3 scale = vec3(1.0, 1.0, 1.0);
} ;

class CharModel 
{
public:
    CharModel();
    CharModel(shared_ptr<Shape> s);
    CharModel(shared_ptr<Shape> head, shared_ptr<Shape> torso, shared_ptr<Shape> arm, shared_ptr<Shape> leg);
    CharModel(shared_ptr<Shape> s, shared_ptr<MatrixStack> g);
    CharModel(vector<shared_ptr<Shape>>, vector<shared_ptr<Texture>>);
    void init();
    virtual void init_Model();
    void render_Model(shared_ptr<Program> prog);
    shared_ptr<MatrixStack> adjust_Part(int part, float rotation);
    void render_Part(shared_ptr<Program> prog, int part);
    shared_ptr<MatrixStack> getMatrix();
    shared_ptr<MatrixStack> setMatrix(shared_ptr<MatrixStack> M);
    virtual bool scare_Motion();
    virtual void walk_Motion();
protected:
    vector<unique_ptr<Part>> body;
    vector<shared_ptr<Texture>> body_texs;
    shared_ptr<MatrixStack> model_transform;
    bool walk, scare;
    float arms = 0, legs = 0;
};