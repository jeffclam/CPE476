/* Base code for shadow mapping lab */
/* This code is incomplete - follow tasks listed in handout */
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "Texture.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"

void initQuad();

using namespace std;
using namespace glm;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
shared_ptr<Program> DepthProg;
shared_ptr<Program> DepthProgDebug;
shared_ptr<Program> ShadowProg;
shared_ptr<Program> DebugProg;
shared_ptr<Shape> cube;
shared_ptr<Shape> shape;

shared_ptr<Texture> texture0;
shared_ptr<Texture> texture1;
shared_ptr<Texture> texture2;

int DEBUG_LIGHT = 0;
int GEOM_DEBUG = 0;
int g_GiboLen;
int g_width, g_height;
int shadow = 1;
int FirstTime = 1;

GLuint depthMapFBO;
const GLuint S_WIDTH = 1024, S_HEIGHT = 1024;
GLuint depthMap;

float g_ObjRot = 0;
glm::vec3 g_light(1, 1, 1);
float updateDir = 0.5;
double g_phi, g_theta;

vec3 view = vec3(0, 0, 1);
vec3 strafe = vec3(1, 0, 0);
vec3 g_eye = vec3(0, 1, 0);
vec3 g_lookAt = vec3(0, 1, -4);

//global data for ground plane
GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj, GrndTanBO, GrndBNBO;
//geometry for texture render
GLuint quad_VertexArrayID;
GLuint quad_vertexbuffer;

static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   float speed = 0.2;

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
      g_eye -= speed*strafe;
      g_lookAt -= speed*strafe;
      }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
      g_eye += speed*strafe;
      g_lookAt += speed*strafe;
      }
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
      g_eye -= speed*view;
      g_lookAt -= speed*view;
      }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
      g_eye += speed*view;
      g_lookAt += speed*view;
      }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
     g_light.x += 0.25;
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
     g_light.x -= 0.25;
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    g_ObjRot += 0.2;
    if (key == GLFW_KEY_N && action == GLFW_PRESS)
     g_ObjRot -= 0.2;
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
     DEBUG_LIGHT = !DEBUG_LIGHT;
}

void scroll_callback(GLFWwindow* window, double deltaX, double deltaY) {
  vec3 diff, newV;
   //cout << "xDel + yDel " << deltaX << " " << deltaY << endl;
   g_theta += deltaX;
   g_phi += deltaY;
   newV.x = cosf(g_phi*(3.14/180.0))*cosf(g_theta*(3.14/180.0));
   newV.y = -1.0*sinf(g_phi*(3.14/180.0));
   newV.z = 1.0*cosf(g_phi*(3.14/180.0))*cosf((90.0-g_theta)*(3.14/180.0));
   diff.x = (g_lookAt.x - g_eye.x) - newV.x;
   diff.y = (g_lookAt.y - g_eye.y) - newV.y;
   diff.z = (g_lookAt.z - g_eye.z) - newV.z;
   g_lookAt.x = g_lookAt.x - diff.x;
   g_lookAt.y = g_lookAt.y - diff.y;
   g_lookAt.z = g_lookAt.z - diff.z;
   view = g_eye - g_lookAt;
   strafe = cross(vec3(0, 1,0), view);
}

static void resize_callback(GLFWwindow *window, int width, int height) {
	g_width = width;
	g_height = height;
	glViewport(0, 0, width, height);
}

/* code to define the ground plane */
static void initGeom() {

   float g_groundSize = 20;
   float g_groundY = -1.5;

  // A x-z plane at y = g_groundY of dimension [-g_groundSize, g_groundSize]^2
    float GrndPos[] = {
    -g_groundSize, g_groundY, -g_groundSize,
    -g_groundSize, g_groundY,  g_groundSize,
     g_groundSize, g_groundY,  g_groundSize,
     g_groundSize, g_groundY, -g_groundSize
    };

    float GrndNorm[] = {
     0, 1, 0,
     0, 1, 0,
     0, 1, 0,
     0, 1, 0,
     0, 1, 0,
     0, 1, 0
    };
   
	/* These are junk right now  - fix! */ 
	float GTBO[] = {
     1, 0, 0,
     1, 0, 0,
     1, 0, 0,
     1, 0, 0,
     1, 0, 0,
     1, 0, 0
    };
	
	float GBNBO[] = {
     0, 0, -1,
     0, 0, -1,
     0, 0, -1,
     0, 0, -1,
     0, 0, -1,
     0, 0, -1
    };

  static GLfloat GrndTex[] = {
      0, 0, // back
      0, 4,
      4, 4,
      4, 0 };

   unsigned short idx[] = {0, 1, 2, 0, 2, 3};

   GLuint VertexArrayID;
	//generate the VAO
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

    g_GiboLen = 6;
    glGenBuffers(1, &GrndBuffObj);
    glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

    glGenBuffers(1, &GrndNorBuffObj);
    glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);
    
	 glGenBuffers(1, &GrndTexBuffObj);
    glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);
	
	/*TODO Make the the VBO for tangents and biNormals */ 
	glGenBuffers(1 , &GrndTanBO);
	glBindBuffer(GL_ARRAY_BUFFER , GrndTanBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GTBO), GTBO, GL_STATIC_DRAW);

	glGenBuffers(1 , &GrndBNBO);
	glBindBuffer(GL_ARRAY_BUFFER , GrndBNBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GBNBO), GBNBO, GL_STATIC_DRAW);

    glGenBuffers(1, &GIndxBuffObj);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

	initQuad();
}

/**** geometry set up for a quad *****/
void initQuad() {

   //now set up a simple quad for rendering FBO
   glGenVertexArrays(1, &quad_VertexArrayID);
   glBindVertexArray(quad_VertexArrayID);

   static const GLfloat g_quad_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
   };

   glGenBuffers(1, &quad_vertexbuffer);
   glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

}


/* set up the FBO for the light's depth */
void initShadow() {

	//generate the FBO for the shadow depth
	glGenFramebuffers(1, &depthMapFBO);

	//generate the texture
	glGenTextures(1, &depthMap);
 	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, S_WIDTH, S_HEIGHT, 
										 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//bind with framebuffer's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


static void init()
{
	GLSL::checkVersion();

   g_phi = 0;
   g_theta = -90;

	// Set background color.
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

  // Initialize mesh.
   shape = make_shared<Shape>();
   shape->loadMesh(RESOURCE_DIR + "dog.obj");
   shape->resize();
   shape->init();
   
	cube = make_shared<Shape>();
   cube->loadMesh(RESOURCE_DIR + "cube.obj");
   cube->resize();
   cube->init();

	// Initialize the GLSL programs
	DepthProg = make_shared<Program>();
	DepthProg->setVerbose(true);
	DepthProg->setShaderNames(RESOURCE_DIR + "depth_vert.glsl", RESOURCE_DIR + "depth_frag.glsl");
	DepthProg->init();
	
	DepthProgDebug = make_shared<Program>();
	DepthProgDebug->setVerbose(true);
	DepthProgDebug->setShaderNames(RESOURCE_DIR + "depth_vertDebug.glsl", RESOURCE_DIR + "depth_fragDebug.glsl");
	DepthProgDebug->init();
	
	ShadowProg = make_shared<Program>();
	ShadowProg->setVerbose(true);
	ShadowProg->setShaderNames(RESOURCE_DIR + "shadow_vert.glsl", RESOURCE_DIR + "shadow_frag.glsl");
	ShadowProg->init();
	
	DebugProg = make_shared<Program>();
	DebugProg->setVerbose(true);
	DebugProg->setShaderNames(RESOURCE_DIR + "pass_vert.glsl", RESOURCE_DIR + "pass_texfrag.glsl");
	DebugProg->init();
	//////////////////////////////////////////////////////
   // Intialize textures
   //////////////////////////////////////////////////////
	texture0 = make_shared<Texture>();
   texture0->setFilename(RESOURCE_DIR + "bumpmap.bmp");
   texture0->init();
   texture0->setUnit(0);
   texture0->setWrapModes(GL_REPEAT, GL_REPEAT);

   texture1 = make_shared<Texture>();
   texture1->setFilename(RESOURCE_DIR + "brick.bmp");
   texture1->init();
   texture1->setUnit(1);
   texture1->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
   
	texture2 = make_shared<Texture>();
   texture2->setFilename(RESOURCE_DIR + "grass.jpg");
   texture2->init();
   texture2->setUnit(1);
   texture2->setWrapModes(GL_REPEAT, GL_REPEAT);

	/// Add uniform and attributes to each of the programs
	DepthProg->addUniform("LP");
	DepthProg->addUniform("LV");
	DepthProg->addUniform("M");
	DepthProg->addAttribute("vertPos");
	//un-needed, but easier then modifying shape
	DepthProg->addAttribute("vertNor");
	DepthProg->addAttribute("vertTex");
	DepthProg->addAttribute("vertTan");
	DepthProg->addAttribute("vertBN");
	
	DepthProgDebug->addUniform("LP");
	DepthProgDebug->addUniform("LV");
	DepthProgDebug->addUniform("M");
	DepthProgDebug->addAttribute("vertPos");
	//un-needed, but easier then modifying shape
	DepthProgDebug->addAttribute("vertNor");
	DepthProgDebug->addAttribute("vertTex");
	DepthProgDebug->addAttribute("vertTan");
	DepthProgDebug->addAttribute("vertBN");
	
	ShadowProg->addUniform("P");
	ShadowProg->addUniform("M");
	ShadowProg->addUniform("V");
	ShadowProg->addUniform("LS");
	ShadowProg->addUniform("lightDir");
	ShadowProg->addAttribute("vertPos");
   ShadowProg->addAttribute("vertNor");
	ShadowProg->addAttribute("vertTex");
	ShadowProg->addAttribute("vertTan");
	ShadowProg->addAttribute("vertBN");
   ShadowProg->addUniform("colorTex");
   ShadowProg->addUniform("normalTex");
   ShadowProg->addUniform("shadowDepth");
	
	DebugProg->addUniform("texBuf");
	DebugProg->addAttribute("vertPos");

	initShadow();
}

void SetProjectionMatrix(shared_ptr<Program> curShade) {
	int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   float aspect = width/(float)height;
  	mat4 Projection = perspective(radians(50.0f), aspect, 0.1f, 100.0f);
	glUniformMatrix4fv(curShade->getUniform("P"), 1, GL_FALSE, value_ptr(Projection));
}

mat4 SetOrthoMatrix(shared_ptr<Program> curShade) {
	mat4 OP = glm::ortho(-10.0, 10.0, -10.0, 10.0, 0.1, 30.0);
	//fill in the glUniform call to send to the right shader!
	glUniformMatrix4fv(curShade->getUniform("LP"), 1, GL_FALSE, value_ptr(OP));
   return OP;
}

/* camera controls - do not change */
void SetView(shared_ptr<Program> curShade) {
 	mat4 Cam = glm::lookAt(g_eye, g_lookAt, vec3(0, 1, 0));
	glUniformMatrix4fv(curShade->getUniform("V"), 1, GL_FALSE, value_ptr(Cam));
}

mat4 SetLightView(shared_ptr<Program> curShade, vec3 pos, vec3 LA, vec3 up) {
 	mat4 Cam = glm::lookAt(pos, LA, up); 
	//fill in the glUniform call to send to the right shader!
	glUniformMatrix4fv(curShade->getUniform("LV"), 1, GL_FALSE, value_ptr(Cam));
	return Cam;
}

/* model transforms */
void SetModel(vec3 trans, float rotY, float rotX, float sc, shared_ptr<Program> curS) {
  mat4 Trans = glm::translate( glm::mat4(1.0f), trans);
  mat4 RotY = glm::rotate(mat4(1.0f), rotY, vec3(0, 1, 0));
  mat4 ctm = Trans*RotY;
  glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
}

void SetModel(mat4 ctm, shared_ptr<Program> curS) {
  glUniformMatrix4fv(curS->getUniform("M"), 1, GL_FALSE, value_ptr(ctm));
}

/*
Draw the dog, sphere and ground plane
Textures can be turned on an off (as shadow map depth drawing does not need textures)
*/
void drawScene(shared_ptr<Program> shader, GLint texID0, GLint texID1, int TexOn) {

	if (TexOn) {
   		texture0->bind(texID0);
	   texture1->bind(texID1);
	}
	//draw the textured box	
	SetModel(vec3(0, 0, -6), g_ObjRot, 0, 1, shader);
	cube->draw(shader);
	
	if (TexOn) { 
   		texture0->bind(texID0);
	   texture2->bind(texID1);
	}	
	//draw the ground plane only if not shadow mapping	
	SetModel(vec3(0, -1, 0), 0, 0, 1, shader);
	glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	 
	glEnableVertexAttribArray(2);
   glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

   glEnableVertexAttribArray(3);
   glBindBuffer(GL_ARRAY_BUFFER, GrndTanBO);
   glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glEnableVertexAttribArray(4);
   glBindBuffer(GL_ARRAY_BUFFER, GrndBNBO);
   glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	//TODO set up tangent and Binormal VBOs

   // draw!
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
   glDrawElements(GL_TRIANGLES, g_GiboLen, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
	
}

/* let's draw */
static void render() {

	mat4 LSpace;
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	if (shadow) {
	  //set up light's depth map
		glViewport(0, 0, S_WIDTH, S_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		
		//set up shadow shader
		//render scene
   	DepthProg->bind();
  		mat4 LO = SetOrthoMatrix(DepthProg);
   	mat4 LV = SetLightView(DepthProg, g_light, vec3(0, 0, 0), vec3(0, 1, 0));
		drawScene(DepthProg, 0, 0, 0);
   	DepthProg->unbind();
		glCullFace(GL_BACK);
	
		LSpace = LO*LV;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	glViewport(0, 0, width, height);
	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (DEBUG_LIGHT) {	
		/* code to draw the light depth buffer */
		//geometry style debug on light - test transforms, draw geometry from light
		//perspective
		if (GEOM_DEBUG) { 
  			DepthProgDebug->bind();
			//render scene from light's point of view
  			SetOrthoMatrix(DepthProgDebug);
   		SetLightView(DepthProgDebug, g_light, vec3(0, 0, 0), vec3(0, 1, 0));
		   drawScene(DepthProgDebug, DepthProgDebug->getUniform("Texture0"), 0, 0);
  			DepthProgDebug->unbind();
		} else {
			//actually draw the light depth map
   		DebugProg->bind();
   		glActiveTexture(GL_TEXTURE0);
   		glBindTexture(GL_TEXTURE_2D, depthMap);
   		glUniform1i(DebugProg->getUniform("texBuf"), 0);
   		glEnableVertexAttribArray(0);
   		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
   		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
   		glDrawArrays(GL_TRIANGLES, 0, 6);
   		glDisableVertexAttribArray(0);
   		DebugProg->unbind();
	  }
	} else {
		//now render the scene like normal
		//set up shadow shader
  		ShadowProg->bind();
		/* also set up light depth map */
   	glActiveTexture(GL_TEXTURE2);
   	glBindTexture(GL_TEXTURE_2D, depthMap);
		glUniform1i(ShadowProg->getUniform("shadowDepth"), 2);
		glUniform3f(ShadowProg->getUniform("lightDir"), g_light.x, g_light.y, g_light.z);
		//render scene
  		SetProjectionMatrix(ShadowProg);
   	SetView(ShadowProg);
   	glUniformMatrix4fv(ShadowProg->getUniform("LS"), 1, GL_FALSE, value_ptr(LSpace))	;
		drawScene(ShadowProg, ShadowProg->getUniform("colorTex"), ShadowProg->getUniform("colorTex"), 1);
  		ShadowProg->unbind();
	}
}

int main(int argc, char **argv)
{

	g_width = 1024;
	g_height = 1024;
	/* we will always need to load external shaders to set up where */
	if(argc < 2) {
      cout << "Please specify the resource directory." << endl;
      return 0;
   }
   RESOURCE_DIR = argv[1] + string("/");

	/* your main will always include a similar set up to establish your window
      and GL context, etc. */

	// Set error callback as openGL will report errors but they need a call back
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
	window = glfwCreateWindow(g_width, g_height, "shadows", NULL, NULL);
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

	glGetError();
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
   glfwSetScrollCallback(window, scroll_callback);
	//set the window resize call back
	glfwSetFramebufferSizeCallback(window, resize_callback);

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	initGeom();
	cout << "done initializing geometry" << endl;
	init();
	cout << "done initializing shaders" << endl;

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
