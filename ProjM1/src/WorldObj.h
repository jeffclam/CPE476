//
//  WorldObject.hpp
//  Lab01
//
//  Created by Andrew Adriance on 1/19/17.
//
//

#ifndef WorldObject_hpp
#define WorldObject_hpp

#include <stdio.h>
#include <random>
#include "WorldGrid.h"
#include "GameObj.h"
#include "EdibleGameObj.h"
#include "EnemyGameObj.h"
#include <vector>
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ManualCamera.h"

class WroldGrid;

using namespace glm;
using namespace std;

class WorldObj
{
public:
    WorldObj();
    ~WorldObj();
    std::vector<GameObj*> objs;
    void render(shared_ptr<Program> prog);
    void update(double time);
    void addObj(GameObj *newObj);
    void addShape(shared_ptr<Shape> s);
    void setWindows(GLFWwindow *win);
    void spawnGrass(EdibleGameObj *e);
    void spawnEnemy(EnemyGameObj *e);
    vector<EdibleGameObj *> edibles;
    vector<EnemyGameObj *> enemies;
    WorldGrid grid = WorldGrid(5, 5);
    ManualCamera cam;
private:
    GameState state;
};

#endif /* WorldObject_hpp */
