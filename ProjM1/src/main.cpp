/* Lab 6 base code - transforms using matrix stack built on glm 
	CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <vector>
#include <random>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "WorldObj.h"
#include "GameObj.h"
#include "EdibleGameObj.h"
#include "EnemyGameObj.h"
#include "PlayerGameObj.h"
#include "Texture.h"
#include "Stuff.h"

using namespace std;
using namespace glm;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
shared_ptr<Program> prog;

int g_width, g_height;

WorldObj world = WorldObj();

float FPS;


static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

static void resize_callback(GLFWwindow *window, int width, int height) {
   g_width = width;
   g_height = height;
   glViewport(0, 0, width, height);
}


static void init()
{
    srand (time(NULL));
	GLSL::checkVersion();
    loadStuff(RESOURCE_DIR, "stuff.json");
	// Set background color.
	glClearColor(.12f, .34f, .56f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
	// Initialize the GLSL program.
	prog = make_shared<Program>();
	prog->setVerbose(true);
	prog->setShaderNames(RESOURCE_DIR + "simple_vert.glsl", RESOURCE_DIR + "simple_frag.glsl");
	prog->init();
	prog->addUniform("P");
	prog->addUniform("M");
    prog->addUniform("V");
	prog->addAttribute("vertPos");
	prog->addAttribute("vertNor");
    prog->addAttribute("vertTex");
    prog->addUniform("specColor");
    prog->addUniform("specShine");
    prog->addUniform("diffuseColor");
    prog->addUniform("ambColor");
    prog->addUniform("lightPos");
    prog->addUniform("lightColor");
    
    GameObj *ground = new GameObj(getShape("cube"), getTexture("grass"));
    ground->setPos(0, -1, 0);
    ground->setScale(60, 0.1, 60);
    ground->setVel(0, 0, 0);
    
    PlayerGameObj *player = new PlayerGameObj(getShape("pointer"), getTexture("test"));
    player->setVel(1, 0, 1);
    player->setPos(0, 0, 55);
    player->setEnemiesList(&(world.enemies));
    EdibleGameObj *edible = new EdibleGameObj(getShape("cube"), getTexture("grass"));
    edible->setPos(0, -20, 0);

    EnemyGameObj *enemy = new EnemyGameObj(getShape("sphere"), getTexture("fur"));
    enemy->setPos(0, -20, 0);

    world.addObj(player);
    world.addObj(ground);
    world.addObj(edible);
    world.addObj(enemy);
}

void setMater(int mat, shared_ptr<Program> prog) {
    glUniform3f(prog->getUniform("lightColor"), 1, 1, 1);
    glUniform3f(prog->getUniform("lightPos"), 0, 0, 0);
    switch (mat) {
        case 0: //shiny blue plastic
            glUniform3f(prog->getUniform("ambColor"), 0.02, 0.04, 0.2);
            glUniform3f(prog->getUniform("diffuseColor"), 0.0, 0.16, 0.9);
            glUniform3f(prog->getUniform("specColor"), 0.14, 0.2, 0.8);
            glUniform1f(prog->getUniform("specShine"), 120.0);
            break;
        case 1: // flat grey
            glUniform3f(prog->getUniform("ambColor"), 0.13, 0.13, 0.14);
            glUniform3f(prog->getUniform("diffuseColor"), 0.3, 0.3, 0.4);
            glUniform3f(prog->getUniform("specColor"), 0.3, 0.3, 0.4);
            glUniform1f(prog->getUniform("specShine"), 4.0);
            break;
        case 2: //brass
            glUniform3f(prog->getUniform("ambColor"), 0.3294, 0.2235, 0.02745);
            glUniform3f(prog->getUniform("diffuseColor"), 0.7804, 0.5686, 0.11373);
            glUniform3f(prog->getUniform("specColor"), 0.9922, 0.941176, 0.80784);
            glUniform1f(prog->getUniform("specShine"), 27.9);
            break;
        case 3: //copper
            glUniform3f(prog->getUniform("ambColor"), 0.1913, 0.0735, 0.0225);
            glUniform3f(prog->getUniform("diffuseColor"), 0.7038, 0.27048, 0.0828);
            glUniform3f(prog->getUniform("specColor"), 0.257, 0.1376, 0.08601);
            glUniform1f(prog->getUniform("specShine"), 12.8);
            break;
        case 4: //emerald
            glUniform3f(prog->getUniform("ambColor"), 0.0215, 0.1745, 0.0215);
            glUniform3f(prog->getUniform("diffuseColor"), 0.07568, 0.61424, 0.07568);
            glUniform3f(prog->getUniform("specColor"), 0.633, 0.727811, 0.633);
            glUniform1f(prog->getUniform("specShine"), 76.8);
            break;
    }
}

static void render()
{
	// Get current frame buffer size.
	int width, height;
    bool show_another_window = true;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glfwSwapBuffers(window);

	//Use the matrix stack for Lab 6
   float aspect = width/(float)height;

   // Create tperspective matrix
   auto P = make_shared<MatrixStack>();
    
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 100.0f);

	prog->bind();
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    setMater(1, prog);
    world.render(prog);
    
	prog->unbind();

   // Pop matrix stacks.
   P->popMatrix();
    ImGui_ImplGlfwGL3_NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
    ImGui::SetNextWindowSize(ImVec2(100,50), ImGuiSetCond_Always);
    ImGui::Begin("Another Window", &show_another_window, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    ImGui::Text("Hello");
    ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
    ImGui::End();
    ImGui::Render();

}

int main(int argc, char **argv)
{
    double lastTime, start, end;
    long frames = 0;
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
   //request the highest possible version of OGL - important for mac
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Create a windowed mode window and its OpenGL context.
    g_width = 640;
    g_height = 480;
	window = glfwCreateWindow(g_width, g_height, "WORLD EDIT", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
    ImGui_ImplGlfwGL3_Init(window, true);
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	//weird bootstrap of glGetError
   glGetError();
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
   cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
   //set the window resize call back
   glfwSetFramebufferSizeCallback(window, resize_callback);

	// Initialize scene. Note geometry initialized in init now
	init();
    world.setWindows(window);
    
    lastTime = glfwGetTime();
	// Loop until the user closes the window.
    start = time(NULL);
	while(!glfwWindowShouldClose(window)) {
        
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
        
        end = time(NULL);
        frames++;
        //printf("FPS: %f\n", (float)frames/(float)(end-start));
        FPS = (float)frames/(float)(end-start);

        world.update(glfwGetTime() - lastTime);
        //cam.walk(glfwGetTime() - lastTime);
        lastTime = glfwGetTime();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
