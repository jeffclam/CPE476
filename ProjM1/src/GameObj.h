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
#include "WorldGrid.h"
#include "CharModel.h"
#include "stuff.h"

using namespace std;
using namespace glm;

class WorldGrid;

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
    virtual void update(GameState *state);
    void render(shared_ptr<Program> p, bool sendData);
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
    void setName(string name);
    string getName();
    void setModel(shared_ptr<CharModel> c);
    shared_ptr<CharModel> getModel();
    shared_ptr<MatrixStack> getM(shared_ptr<MatrixStack> M);
    void calcBoundingBox(mat4 transform);
    float calcBoundingRadius();
    void calcBoundingSphere();
    bool check_Interact_Radius(GameObj obj);
    GameObj *check_Collision_Radius(vector<GameObj *> *objs);
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
    WorldGrid *grid = NULL;
    bool solid = true;
    bool dead;
    float interact_radius = 0;
private:
    vec3 oldScale = vec3(0,0,0);
    shared_ptr<CharModel> model = NULL;
};

#endif
