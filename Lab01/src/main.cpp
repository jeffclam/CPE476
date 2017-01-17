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
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
shared_ptr<Program> prog;
shared_ptr<Shape> shape;
shared_ptr<Shape> shape2;
shared_ptr<Shape> ground;

int g_width, g_height;
float sTheta;
float trans;
float handRot;
float handDir;
bool anim = true;
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
    if(key == GLFW_KEY_L && action == GLFW_PRESS) {
        Json::Value root;
        Json::Reader reader;
        std::ifstream stuffIn(RESOURCE_DIR + saveFile, std::ifstream::binary);
        
        bool parsingSuccessful = reader.parse( stuffIn, root, false );
        if ( !parsingSuccessful )
        {
            // report to the user the failure and their locations in the document.
            std::cout << "Error reading stuff file: "
            << reader.getFormatedErrorMessages()
            << "\n";
        }
        place.load(root);
    }
    if(key == GLFW_KEY_K && action == GLFW_PRESS) {
        Json::Value root;
        root = place.save();
        
        std::ofstream pOut;
        pOut.open(RESOURCE_DIR + saveFile);
        pOut << root;
    }
    if(key == GLFW_KEY_H && action == GLFW_PRESS) {
        place.toggleCursor();
    }
    
    if(key == GLFW_KEY_X && action == GLFW_PRESS) {
        workAxis = 0;
    }
    if(key == GLFW_KEY_Y && action == GLFW_PRESS) {
        workAxis = 1;
    }
    if(key == GLFW_KEY_Z && action == GLFW_PRESS) {
        workAxis = 2;
    }
    if(key == GLFW_KEY_R && action == GLFW_PRESS) {
        workAxis = 0;
        place.cursor.setScale(1, 1, 1);
        place.cursor.setRot(0, 0, 0);
    }
    
    if(key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
        place.cursor.scale[workAxis] = place.cursor.scale[workAxis] + 0.1;
    }
    if(key == GLFW_KEY_MINUS && action == GLFW_PRESS) {
        place.cursor.scale[workAxis] = place.cursor.scale[workAxis] - 0.1;
    }
    if(key == GLFW_KEY_BACKSLASH && action == GLFW_PRESS) {
        place.cursor.rot[workAxis] = place.cursor.rot[workAxis] + 0.1;
    }
    if(key == GLFW_KEY_SLASH && action == GLFW_PRESS) {
        place.cursor.rot[workAxis] = place.cursor.rot[workAxis] - 0.1;
    }
    
    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        if(place.cursor.mat == 0) {
            place.cursor.mat = MAT_NUM - 1;
        } else {
            place.cursor.mat = (place.cursor.mat - 1) % MAT_NUM;
        }
    }
    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        place.cursor.mat = (place.cursor.mat + 1) % MAT_NUM;
    }
    if(key == GLFW_KEY_UP && action == GLFW_PRESS) {
        if(curShape == 0) {
            curShape = maxShapeDex - 1;
        } else {
            curShape = (curShape - 1) % maxShapeDex;
        }
        place.cursor.setName(getShapeByIndex(curShape));
    }
    if(key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        curShape = (curShape + 1) % maxShapeDex;
        place.cursor.setName(getShapeByIndex(curShape));
    }
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
    cam.handleInputKey(key, action);
    if(key == GLFW_KEY_C && action == GLFW_PRESS) {
        sTheta = 0;
        handRot = 0;
    }
    if(key == GLFW_KEY_X && action == GLFW_PRESS) {
        anim = !anim;
    }
}


static void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
   double posX, posY;
   if (action == GLFW_PRESS) {
      glfwGetCursorPos(window, &posX, &posY);
       place.placeCursor();
      //cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
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
    
	sTheta = 0;
    handRot = 0;
    handDir = 0;
	// Set background color.
	glClearColor(.12f, .34f, .56f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize mesh.
	shape = make_shared<Shape>();
	shape->loadMesh(RESOURCE_DIR + "sphere.obj");
	shape->resize();
	shape->init();
    
    // Initialize mesh.
    shape2 = make_shared<Shape>();
    shape2->loadMesh(RESOURCE_DIR + "body.obj");
    shape2->resize();
    shape2->init();
    
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
    auto B = make_shared<MatrixStack>();
    
    
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

    if (anim) {
        if (sTheta < 1.4) {
            sTheta += 0.01;
        } else {
            if(handRot > 0.5) {
                handDir = 1;
            }
            if(handRot < -0.5) {
                handDir = 0;
            }
            if(handDir == 0) {
                handRot += 0.02;
            } else {
                handRot -= 0.02;
            }
        }
    }
    cam.walk();
}

int main(int argc, char **argv)
{
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

	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
