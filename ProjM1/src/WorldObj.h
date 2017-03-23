//
//  WorldObject.hpp
//  Lab01
//
//  Created by Andrew Adriance on 1/19/17.
//
//

#ifndef WorldObject_hpp
#define WorldObject_hpp

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <random>
#include <vector>
#include <math.h>

#include "WorldGrid.h"
#include "GameObj.h"
#include "EdibleGameObj.h"
#include "EnemyGameObj.h"
#include "SupportGameObj.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ManualCamera.h"
#include "camera.h"

class WroldGrid;

using namespace glm;
using namespace std;

class WorldObj
{
public:
    WorldObj();
    ~WorldObj();
    std::vector<GameObj*> objs;
    void render(shared_ptr<Program> prog, bool shadowPass);
    void update(double time);
    void addObj(GameObj *newObj);
    void setWindows(GLFWwindow *win);
    void makeFence(int row, int col);
    void cleanUp();
    vector<EdibleGameObj *> edibles;
    vector<EnemyGameObj *> enemies;
    map<string, shared_ptr<CharModel>> charModels;
    WorldGrid grid = WorldGrid(15, 25);
    ManualCamera cam;
    GameState state;
    mat4 PMat = mat4(0);
private:
    string lastRendered = "";
};

#endif /* WorldObject_hpp */
