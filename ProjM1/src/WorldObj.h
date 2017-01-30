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
    void growGrass(vector<EdibleGameObj *> e);
    void spawnEnemy(vector<EnemyGameObj *> e);
private:
    GameState state;
    ManualCamera cam;
};

#endif /* WorldObject_hpp */
