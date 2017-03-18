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
#include "ParticleSystem.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "WorldObj.h"
#include "GameObj.h"
#include "EdibleGameObj.h"
#include "EnemyCharModel.h"
#include "EnemyGameObj.h"
#include "PlayerCharModel.h"
#include "PlayerGameObj.h"
#include "GameObjPather.h"
#include "Texture.h"
#include "Stuff.h"
#include "Lighting.h"

#ifdef _WIN32
    #define RETSCALE 1
#else
	#define RETSCALE 2
#endif

using namespace std;
using namespace glm;

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from
shared_ptr<Program> gprog, defprog;

int g_width, g_height;

WorldObj world = WorldObj();
bool gameOver = false;
Lighting lighting = Lighting();
Sky sky;
ParticleManager pm;

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

GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(defprog->getAttribute("vertPos"));
		glVertexAttribPointer(defprog->getAttribute("vertPos") , 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(defprog->getAttribute("vertTex"));
		glVertexAttribPointer(defprog->getAttribute("vertTex") , 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

GLuint gBuffer;
GLuint gPosition, gNormal, gAlbedoSpec, rboDepth;
void defferedInit() {
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	
	// - Position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, g_width * RETSCALE, g_height * RETSCALE, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	
	// - Normal color buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, g_width * RETSCALE, g_height * RETSCALE, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	
	// - Color + Specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_width * RETSCALE, g_height * RETSCALE, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	
	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	//set up depth buffer
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, g_width * RETSCALE, g_height * RETSCALE);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
}


static void init()
{
    srand (time(NULL));
	GLSL::checkVersion();
	defferedInit();
    loadStuff(RESOURCE_DIR, "stuff.json");
	// Set background color.
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	/*glBlendFunc (GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);*/
    
	pm.init(RESOURCE_DIR + "part_vert.glsl", RESOURCE_DIR + "part_frag.glsl");

	sky.initShader(RESOURCE_DIR + "sky_vert.glsl", RESOURCE_DIR + "sky_frag.glsl");
	lighting.initShadow();
    lighting.initShadowProg(RESOURCE_DIR);

	// Initialize the GLSL program.
	gprog = make_shared<Program>();
	gprog->setVerbose(true);
	gprog->setShaderNames(RESOURCE_DIR + "gbuffer_vert.glsl", RESOURCE_DIR + "gbuffer_frag.glsl");
	gprog->init();
	gprog->addUniform("P");
	gprog->addUniform("M");
    gprog->addUniform("V");
	gprog->addAttribute("vertPos");
	gprog->addAttribute("vertNor");
    gprog->addAttribute("vertTex");
    gprog->addUniform("specShine");

	defprog = make_shared<Program>();
	defprog->setVerbose(true);
	defprog->setShaderNames(RESOURCE_DIR + "deferred_vert.glsl", RESOURCE_DIR + "deferred_frag.glsl");
	defprog->init();
	defprog->addAttribute("vertPos");
    defprog->addAttribute("vertTex");
	defprog->addUniform("numLights");
	defprog->addUniform("shadowDepth");
	defprog->addUniform("gNormal");
	defprog->addUniform("gPosition");
	defprog->addUniform("gAlbedoSpec");
	defprog->addUniform("LS");
	defprog->addUniform("height");
	defprog->addUniform("width");

    Light sun1;
	sun1.pos = vec4(0, 50, 0, 0);
    sun1.intensity = vec3(0);
    /* Based on Overcast Sky color (more blue-ish) */
	sun1.intensity = vec3(201/255.0, 226/255.0, 255/255.0) * vec3(.45);
    sun1.ambCoeff = 1;
    lighting.push_back(sun1);
    
    
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 2; j++) {
            Light newLight;
            newLight.pos = vec4(35 * i, 10.0, 35 * j, 1);
            /* Based on 100W Tungston to add orange-ish color */
            newLight.intensity = vec3(0);
            newLight.intensity = vec3(255/255.0, 214/255.0, 190/255.0) * vec3(.02);
            newLight.ambCoeff = 1;
            lighting.push_back(newLight);
        }
    }
    
    
    defprog->addUniformLights("lights", lighting.size());
	lighting.SetLightUniforms(defprog);
    
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
        make_shared<PlayerCharModel>(PlayerCharModel(playerParts, playerTexs));

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
        make_shared<EnemyCharModel>(EnemyCharModel(sheepParts, sheepTexs));
    sheep_model->init_Model();
    shared_ptr<CharModel> sheep_walk =
        make_shared<EnemyCharModel>(EnemyCharModel(sheepParts, sheepTexs));
    sheep_walk->init_Model();
    shared_ptr<CharModel> sheep_scare =
        make_shared<EnemyCharModel>(EnemyCharModel(sheepParts, sheepTexs));
    sheep_scare->init_Model();

    world.charModels.insert(
        pair<string, shared_ptr<CharModel>>("sheep_model", sheep_model));
    world.charModels.insert(
        pair<string, shared_ptr<CharModel>>("sheep_walk", sheep_walk));
    world.charModels.insert(
        pair<string, shared_ptr<CharModel>>("sheep_scare", sheep_scare));
    //dynamic_pointer_cast<EnemyCharModel>(world.charModels.at("sheep_model"))->set_Pointers(sheep_model, sheep_walk, sheep_scare);

    player->setVel(1, 0, 1);
    player->setPos(10, 2.5, 10);
    player->setScale(.5, .5, .5);
    player->setModel(player_model);
    player->getModel()->init_Model();
	world.cam.eyePt = player->getPos();
	world.addObj(player);

	world.makeFence(12, 22);

	world.state.partManager = &pm;

	sky.setRight(getTexture("skyRight"));
	sky.setLeft(getTexture("skyLeft"));
	sky.setTop(getTexture("skyUp"));
	sky.setBottom(getTexture("skyDown"));
	sky.setBack(getTexture("skyBack"));
	sky.setFront(getTexture("skyFront"));
	sky.loadCubemap();

	/*ISound* music = engine->play3D((RESOURCE_DIR + "Funky_Chunk.ogg").c_str(),
	                               vec3df(0,0,0), true, false, true);

	if (music) {
		music->setMinDistance(5.0f);
		music->setIsPaused(false);
	}
	engine->setListenerPosition(vec3df(0,0,0), vec3df(0,0,1));*/
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
   
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glDepthFunc(GL_LESS); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//geomtry render
	gprog->bind();
	glUniformMatrix4fv(gprog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));
	world.PMat = P->topMatrix();
    setMat(1, gprog);
    world.render(gprog, false);
	gprog->unbind();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	//lighting render
	defprog->bind();
	glUniform1i(defprog->getUniform("height"), g_height * RETSCALE);
	glUniform1i(defprog->getUniform("width"), g_width * RETSCALE);
	glActiveTexture(GL_TEXTURE3);
   	glBindTexture(GL_TEXTURE_2D, lighting.depthMap);
	glUniform1i(defprog->getUniform("shadowDepth"), 3);
	glUniformMatrix4fv(defprog->getUniform("LS"), 1, GL_FALSE, value_ptr(LS));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
	glUniform1i(defprog->getUniform("gPosition"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
	glUniform1i(defprog->getUniform("gNormal"), 1);
    glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glUniform1i(defprog->getUniform("gAlbedoSpec"), 2);
	RenderQuad();
	defprog->unbind();

	glDepthFunc(GL_ALWAYS); 
	glClear(GL_DEPTH_BUFFER_BIT);
   	pm.render(world.cam.getLookAt(), world.PMat);
	glDepthFunc(GL_LESS); 

   // Pop matrix stacks.
   P->popMatrix();

   //Render user interface
    ImGui_ImplGlfwGL3_NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
    ImGui::SetNextWindowSize(ImVec2(130, 75), ImGuiSetCond_Always);
    ImGui::Begin("Another Window", &show_another_window, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    ImGui::Text("Score: %d", world.state.score);
	ImGui::Text("Lawn Health: %lu%s", (world.state.grassAlive*100)/world.edibles.size(),"%");
	ImGui::Text("Retire In: %d:%02d", (int)world.state.retireIn/60, (int)world.state.retireIn%60);
    //ImGui::Text("(%.1f FPS)", ImGui::GetIO().Framerate);
	//ImGui::Image((void *)gAlbedoSpec, ImVec2(g_width/8, g_height/8));
	//ImGui::Image((void *)gNormal, ImVec2(g_width/8, g_height/8));
	//ImGui::Image((void *)gPosition, ImVec2(g_width/8, g_height/8));
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
	if(world.state.retireIn <= 0) {
        ImGui_ImplGlfwGL3_NewFrame();
		ImGui::SetNextWindowPos(ImVec2(g_width/2 - 103, g_height/2 - 75), ImGuiSetCond_Always);
		ImGui::SetNextWindowSize(ImVec2(150,75), ImGuiSetCond_Always);
		ImGui::Begin("GAME OVER", &show_another_window, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		ImGui::Text("GAME OVER");
		ImGui::Text("YOU HAVE RETIRED :D");
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
