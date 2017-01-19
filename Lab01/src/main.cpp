/* Lab 6 base code - transforms using matrix stack built on glm 
	CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <vector>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "json/json.h"

#include "stuff.h"
#include "thing.h"
#include "place.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

using namespace std;
using namespace glm;

/* to use glee */
//#define GLEE_OVERWRITE_GL_FUNCTIONS
//#include "glee.hpp"

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
shared_ptr<Program> prog;
shared_ptr<Shape> ground;

int g_width, g_height;
float xPos[10];
float zPos[10];
float xPos2[10];
float zPos2[10];

Camera cam = Camera();

Json::Value save;
string saveFile = "save.place";


Place place = Place();

int maxShapeDex = 0;
int curShape = 0;

int workAxis = 0;


static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
    cam.handleInputKey(key, action);
}


static void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
   if (action == GLFW_PRESS) {
      //user pressed
	}
}

static void mouseMove_callback(GLFWwindow *window, double xPos, double yPos)
{
    cam.handleInputMouse(xPos, yPos, g_width, g_height);
}

static void resize_callback(GLFWwindow *window, int width, int height) {
   g_width = width;
   g_height = height;
   glViewport(0, 0, width, height);
}


static void init()
{
	GLSL::checkVersion();

    srand (time(NULL));
    for(int i = 0; i < 10; i++) {
        xPos[i] = rand() % 40 - 20;
        zPos[i] = rand() % 40 - 20;
        xPos2[i] = rand() % 40 - 20;
        zPos2[i] = rand() % 40 - 20;
    }
    
	// Set background color.
	glClearColor(.12f, .34f, .56f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    ground = make_shared<Shape>();
    ground->loadMesh(RESOURCE_DIR + "cube.obj");
    ground->resize();
    ground->init();

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
    
    loadStuff(RESOURCE_DIR, "stuff.stuff");
    
    maxShapeDex = getShapeCount();
    place.cursor.setName(getShapeByIndex(0));
}

static void renderGround(std::shared_ptr<MatrixStack> P) {
    auto M = make_shared<MatrixStack>();
    
    
    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(cam.getLookAt()));
    
    M->pushMatrix();
    M->loadIdentity();
    M->translate(vec3(0,-1,0));
    M->scale(vec3(20,0.1,20));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    ground->draw(prog);
    M->popMatrix();
}

static void render()
{
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the matrix stack for Lab 6
   float aspect = width/(float)height;

   // Create the matrix stacks - please leave these alone for now
   auto P = make_shared<MatrixStack>();
   auto M = make_shared<MatrixStack>();
    
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 100.0f);

	// Draw a stack of cubes with indiviudal transforms 
	prog->bind();
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(cam.getLookAt()));
    setMat(1, prog);
    renderGround(P);
    
    vec3 cursorPos = vec3(0,0,0);
    
    place.cursor.setPos(cursorPos[0], cursorPos[1], cursorPos[2]);
    place.renderPlace(prog);
    
	prog->unbind();

   // Pop matrix stacks.
   P->popMatrix();

}

int main(int argc, char **argv)
{
    double lastTime;
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
   //set the mouse call back
   glfwSetMouseButtonCallback(window, mouse_callback);
    //set the mouse move call back
    glfwSetCursorPosCallback(window, mouseMove_callback);
   //set the window resize call back
   glfwSetFramebufferSizeCallback(window, resize_callback);

	// Initialize scene. Note geometry initialized in init now
	init();
    lastTime = glfwGetTime();
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
        
        cam.walk(glfwGetTime() - lastTime);
        lastTime = glfwGetTime();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
