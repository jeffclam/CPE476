#ifndef _GAMEOBJ_H_
#define _GAMEOBJ_H_

#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <memory>
#include <time.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "GameState.h"
#include "Texture.h"

using namespace std;
using namespace glm;

struct Bounding_Box {
    vec3 min;
    vec3 max;
};

struct Bounding_Sphere {
    vec3 center;
    float radius;
};

class GameObj {
public:
    GameObj();
    GameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex);
    virtual ~GameObj();
    virtual void update(GameState state);
    void render(shared_ptr<Program> p);
    void setShape(shared_ptr<Shape> s);
    void setWorldObjs(vector<GameObj*> *objs);
    vec3 getPos();
    void setPos(float x, float y, float z);
    void setPos(vec3 v);
    vec3 getVel();
    void setVel(float x, float y, float z);
    void setVel(vec3 v);
    void setScale(float x, float y, float z);
    void setRot(float x, float y, float z);
    shared_ptr<MatrixStack> getM(shared_ptr<MatrixStack> M);
    void calcBoundingBox(mat4 transform);
    float calcBoundingRadius();
    void calcBoundingSphere();
    bool check_Interact_Radius(GameObj obj);
    GameObj *check_Collision_Radius();
    int mat;
    vec3 vel;
    vec3 dir;
    vec3 pos; //x y z pos
    vec3 scale; //x y z scale
    vec3 rot; //x y z rotation
    vector<GameObj*>* worldObjs;
    Bounding_Sphere b_sphere;
    Bounding_Box b_box;
    void setRandomVel();
    shared_ptr<Shape> shape;
    shared_ptr<Texture> texture;
    bool is_Edible = false;
    bool was_Pushed = false;
    string name = "";
    string noInteract = "_NONE_";
private:
};

#endif
