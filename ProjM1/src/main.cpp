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
#include <map>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "sky.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "WorldObj.h"
#include "GameObj.h"
#include "EdibleGameObj.h"
#include "EnemyGameObj.h"
#include "PlayerGameObj.h"
#include "GameObjPather.h"
#include "Texture.h"
#include "toonshading.h"
#include "Stuff.h"
#include "Lighting.h"

using namespace std;
using namespace glm;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
shared_ptr<Program> main_prog, prog;

int g_width, g_height;

WorldObj world = WorldObj();
bool gameOver = false;
Lighting lighting = Lighting();
ToonShading toonshading = ToonShading();
Sky sky;

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
	/*glBlendFunc (GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);*/
    
	sky.initShader(RESOURCE_DIR + "sky_vert.glsl", RESOURCE_DIR + "sky_frag.glsl");
	lighting.initShadow();
    lighting.initShadowProg(RESOURCE_DIR);
    toonshading.initShader(RESOURCE_DIR);

	// Initialize the GLSL program.
	main_prog = make_shared<Program>();
    prog = main_prog;
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
	prog->addUniform("numLights");
	prog->addUniform("shadowDepth");
	prog->addUniform("LS");

    //prog = toonshading.toonProg;

    Light sun1;
	sun1.pos = vec4(20.0, 30.0, 20.0, 0);
	sun1.intensity = vec3(1.0, 1.0, 1.0);
	sun1.ambCoeff = 1.0;
	lighting.push_back(sun1);

    Light other;
    other.pos = vec4(0, 10, -.5, 0);
    other.intensity = vec3(1, .8, .6);
    other.ambCoeff = 0.20;
    lighting.push_back(other);

    prog->addUniformLights("lights", lighting.size());
	lighting.SetLightUniforms(prog);
    
	world.grid.initGrid();

    PlayerGameObj *player = new PlayerGameObj(getShape("pointer"), getTexture("test"));
    
    vector<shared_ptr<Shape>> playerParts = {
        getShape("manBody"), getShape("manHead"),
        getShape("manArm"),getShape("manArm"),
        getShape("manLeg"), getShape("manLeg")};
    vector<shared_ptr<Texture>> playerTexs = { 
        getTexture("manBodyText"), getTexture("faceText"),
        getTexture("manArmText"), getTexture("manArmText"),
        getTexture("manLegText"), getTexture("manLegText") };
    shared_ptr<CharModel> player_model = 
        make_shared<CharModel>(CharModel(playerParts, playerTexs));

    vector<shared_ptr<Shape>> sheepParts = {
        getShape("sheepBod"), getShape("sheepHead"),
        getShape("sheepLeg"), getShape("sheepLeg"),
        getShape("sheepLeg"), getShape("sheepLeg")
    };
    vector<shared_ptr<Texture>> sheepTexs = {
        getTexture("sheepBodText"), getTexture("sheepHeadText"),
        getTexture("legText"), getTexture("legText"),
        getTexture("legText"), getTexture("legText")
    };
    shared_ptr<CharModel> sheep_model =
        make_shared<CharModel>(CharModel(sheepParts, sheepTexs));
    world.charModels.insert(
        pair<string, shared_ptr<CharModel>>("sheep_model", sheep_model));

    player->setVel(1, 0, 1);
    player->setPos(10, 2.2, 10);
    player->setScale(.5, .5, .5);
    player->setModel(player_model);
    player->getModel()->init_PlayerModel();
	world.cam.eyePt = player->getPos();
	world.addObj(player);

	world.makeFence(12, 22);

    //EnemyGameObj *enemy = new EnemyGameObj(getShape("manPants"), getTexture("manPantText"));
    //shared_ptr<CharModel> enemy_model = make_shared<CharModel>();

    //enemy->setPos(0, 0, 0);
    //enemy->setVel(0, 0, 0);

    //world.addObj(enemy);
	//world.grid.addToGrid(enemy);

	sky.setRight(getTexture("skyRight"));
	sky.setLeft(getTexture("skyLeft"));
	sky.setTop(getTexture("skyUp"));
	sky.setBottom(getTexture("skyDown"));
	sky.setBack(getTexture("skyBack"));
	sky.setFront(getTexture("skyFront"));
	sky.loadCubemap();
}

static void render()
{
	// Get current frame buffer size.
	int width, height;
	mat4 LS;
    bool show_another_window = true;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the matrix stack for Lab 6
   float aspect = width/(float)height;

   // Create tperspective matrix
   auto P = make_shared<MatrixStack>();
    
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, 100.0f);

	//shadow render
	if (lighting.hasShadow) {
	  //set up light's depth map
		glViewport(0, 0, lighting.S_WIDTH, lighting.S_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, lighting.depthMapFBO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//set up shadow shader
		//render scene
   		lighting.DepthProg->bind();
		//TODO you will need to fix these
  		mat4 LO = lighting.SetOrthoMatrix();
   		mat4 LV = lighting.SetLightView();
		LS = LO * LV;
		world.render(lighting.DepthProg, true);
   		lighting.DepthProg->unbind();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//sky render
	sky.render(P->topMatrix(), world.cam.getLookAt());

    toonshading.toonProg->bind();
    glUniformMatrix4fv(toonshading.toonProg->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
    glEnable(GL_CULL_FACE);
    glCullFace(GL_CCW);
    glDepthMask(GL_TRUE);
    glUniform3f(toonshading.toonProg->getUniform("silhouette_color"), 0.0, 0.0, 0.0);
    glUniform1f(toonshading.toonProg->getUniform("silhouette_offset"), -0.65f);
    world.render(toonshading.toonProg, false);

    glCullFace(GL_CW);
    glDepthMask(GL_FALSE);
    glUniform3f(toonshading.toonProg->getUniform("silhouette_color"), 1.0, 1.0, 1.0);
    glUniform1f(toonshading.toonProg->getUniform("silhouette_offset"), 0.0);
    world.render(toonshading.toonProg, false);
    toonshading.toonProg->unbind();

    glDisable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);

	prog->bind();
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	world.PMat = P->topMatrix();
    setMat(1, prog);
	glActiveTexture(GL_TEXTURE1);
   	glBindTexture(GL_TEXTURE_2D, lighting.depthMap);
	glUniform1i(prog->getUniform("shadowDepth"), 1);
	glUniformMatrix4fv(prog->getUniform("LS"), 1, GL_FALSE, value_ptr(LS));
    world.render(prog, false);
	prog->unbind();

	

   // Pop matrix stacks.
   P->popMatrix();

   //Render user interface
    ImGui_ImplGlfwGL3_NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
    ImGui::SetNextWindowSize(ImVec2(130 , 75), ImGuiSetCond_Always);
    ImGui::Begin("Another Window", &show_another_window, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    ImGui::Text("Score: %d", world.state.score);
	ImGui::Text("Lawn Health: %lu%s", (world.state.grassAlive*100)/world.edibles.size(),"%");
    ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
	//ImGui::Image((void *)lighting.depthMap, ImVec2(lighting.S_WIDTH/8 * SHOW_SM, lighting.S_HEIGHT/8 * SHOW_SM));
    ImGui::End();
    ImGui::Render();
	if(world.state.grassAlive < world.edibles.size()/2) {
        ImGui_ImplGlfwGL3_NewFrame();
		ImGui::SetNextWindowPos(ImVec2(g_width/2 - 103, g_height/2 - 75), ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(130,75), ImGuiSetCond_Always);
		ImGui::Begin("GAME OVER", &show_another_window, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		ImGui::Text("GAME OVER");
		ImGui::Text("YOUR LAWN DIED D:");
		ImGui::End();
    	ImGui::Render();
		gameOver = true;
    }

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
	window = glfwCreateWindow(g_width, g_height, "Oh Mah Lawn!", NULL, NULL);
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
	while(!glfwWindowShouldClose(window)) {
        
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
        if(!gameOver)
            world.update(glfwGetTime() - lastTime);
        lastTime = glfwGetTime();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
