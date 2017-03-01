#include <vector>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

using namespace std;
using namespace glm;

struct Light {
    vec4 pos;
    vec3 intensity;
    float ambCoeff;
};

class Lighting {
public:
    vector<Light> lights;
    void SetLightUniforms(shared_ptr<Program> prog);
    void push_back(Light l);
    int size();
    void initShadow();
    void initShadowProg(string resDir);
    mat4 SetOrthoMatrix();
    mat4 SetLightView();
    bool hasShadow = false;
    shared_ptr<Program> DepthProg;
    GLuint depthMapFBO;
    const GLuint S_WIDTH = 1024 * 2, S_HEIGHT = 1024 * 2;
    GLuint depthMap;
private:
};