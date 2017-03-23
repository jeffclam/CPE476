//
//  WorldObject.cpp
//  Lab01
//
//  Created by Andrew Adriance on 1/19/17.
//
//
#include "stuff.h"
#include "WorldObj.h"

double updateTime = 0.0;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(-30, 30);

WorldObj::WorldObj() :
    objs(),
    cam(),
    state(),
    charModels()
{
}

WorldObj::~WorldObj(){

}

/* VFC code starts here TODO - start here and fill in these functions!!!*/
vec4 Left, Right, Bottom, Top, Near, Far;
vec4 planes[6];

void ExtractVFPlanes(mat4 V, mat4 P) {

	/* composite matrix */
    //mat 4 is [col][row] like other GPU stuff
	mat4 comp = P*V;
	vec3 n; //use to pull out normal
	float l; //length of normal for plane normalization

	Left.x = comp[0][3] + comp[0][0]; // see handout to fill in with values from comp
	Left.y = comp[1][3] + comp[1][0]; // see handout to fill in with values from comp
	Left.z = comp[2][3] + comp[2][0]; // see handout to fill in with values from comp
	Left.w = comp[3][3] + comp[3][0]; // see handout to fill in with values from comp
    n = vec3(Left.x, Left.y, Left.z);
    l = length(n);
	planes[0] = Left = Left/l;
	
	Right.x = comp[0][3] - comp[0][0]; // see handout to fill in with values from comp
	Right.y = comp[1][3] - comp[1][0]; // see handout to fill in with values from comp
	Right.z = comp[2][3] - comp[2][0]; // see handout to fill in with values from comp
	Right.w = comp[3][3] - comp[3][0]; // see handout to fill in with values from comp
    n = vec3(Right.x, Right.y, Right.z);
    l = length(n);
	planes[1] = Right = Right/l;

	Bottom.x = comp[0][3] + comp[0][1]; // see handout to fill in with values from comp
	Bottom.y = comp[1][3] + comp[1][1]; // see handout to fill in with values from comp
	Bottom.z = comp[2][3] + comp[2][1]; // see handout to fill in with values from comp
	Bottom.w = comp[3][3] + comp[3][1]; // see handout to fill in with values from comp
    n = vec3(Bottom.x, Bottom.y, Bottom.z);
    l = length(n);
	planes[2] = Bottom = Bottom/l;
	
	Top.x = comp[0][3] - comp[0][1]; // see handout to fill in with values from comp
	Top.y = comp[1][3] - comp[1][1]; // see handout to fill in with values from comp
	Top.z = comp[2][3] - comp[2][1]; // see handout to fill in with values from comp
	Top.w = comp[3][3] - comp[3][1]; // see handout to fill in with values from comp
    n = vec3(Top.x, Top.y, Top.z);
    l = length(n);
	planes[3] = Top  = Top/l;

	Near.x = comp[0][3] + comp[0][2]; // see handout to fill in with values from comp
	Near.y = comp[1][3] + comp[1][2]; // see handout to fill in with values from comp
	Near.z = comp[2][3] + comp[2][2]; // see handout to fill in with values from comp
	Near.w = comp[3][3] + comp[3][2]; // see handout to fill in with values from comp
    n = vec3(Near.x, Near.y, Near.z);
    l = length(n);
	planes[4] = Near = Near/l;

	Far.x = comp[0][3] - comp[0][2]; // see handout to fill in with values from comp
	Far.y = comp[1][3] - comp[1][2]; // see handout to fill in with values from comp
	Far.z = comp[2][3] - comp[2][2]; // see handout to fill in with values from comp
	Far.w = comp[3][3] - comp[3][2]; // see handout to fill in with values from comp
    n = vec3(Far.x, Far.y, Far.z);
    l = length(n);
	planes[5] = Far = Far/l;
}


/* helper function to compute distance to the plane */
float DistToPlane(float A, float B, float C, float D, vec3 point) {
	return (A*point.x + B*point.y + C*point.z + D);
}

/* Actual cull on planes */
//returns 1 to CULL
int ViewFrustCull(vec3 center, float radius) {

	float dist;

    for (int i=0; i < 6; i++) {
        dist = DistToPlane(planes[i].x, planes[i].y, planes[i].z, planes[i].w, center);
        if(dist < -radius * 2) {
            //cout << "Culled " << dist << " " << radius << " " << i <<"\n";
            return 1;
        }
    }
    return 0;
}

void WorldObj::render(shared_ptr<Program> prog, bool shadowPass) {
    if(!shadowPass) {
        //cam.eyePt[1] = 2;
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(cam.getLookAt()));
        //glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(glm::lookAt(vec3(0,-7,0), vec3(0), vec3(0,1,0))));
        ExtractVFPlanes(cam.getLookAt(), PMat);
        lastRendered = "_NONE_";
    }
    for(int i = 0; i < objs.size(); i++) {
        if(shadowPass || !ViewFrustCull(objs[i]->pos, objs[i]->b_sphere.radius + 1))
            objs[i]->render(prog, (lastRendered != objs[i]->name && !shadowPass));
    }
    if(shadowPass)
        return;
    for(int i = 0; i < edibles.size(); i++) {
        if(shadowPass || !ViewFrustCull(edibles[i]->pos, edibles[i]->b_sphere.radius + 1))
            edibles[i]->render(prog, !shadowPass);
    }
    grid.renderGrid(prog, shadowPass);
}

void WorldObj::cleanUp() {
    for(int i = 0; i < objs.size(); i++) {
        if(objs[i]->dead) {
            GameObj *dead = objs[i];
            objs.erase(objs.begin() + i);
            delete dead;
        }
    }
}

void WorldObj::update(double time) {
    state.grassAlive = 0;
    state.retireIn -= state.deltaTime;
    cleanUp();
    updateTime += time;
    //update game state
    state.deltaTime = (float) time;
    state.worldTime += (float) time;
    state.timeSinceSpawn += (float) time;
    state.waterLevel += (float) time;
    if(state.timeSinceSpawn > state.timeBetweenSpawn && state.enemyCount < 15) {
        state.enemyCount++;
        state.score++;
        state.timeSinceSpawn = 0;

        EnemyGameObj *enemy = new EnemyGameObj(getShape("sheepLeg"), getTexture("legText"));
        enemy->setModel(charModels.at("sheep_model"));
        enemy->setScale(.75, .75, .75);
        enemy->setPos(40, 2, 60);
        enemy->normal = enemy->getModel();
        enemy->walking = charModels.at("sheep_walk");
        enemy->scared = charModels.at("sheep_scare");
        addObj(enemy);
        grid.addToGrid(enemy);
    }
    glfwGetCursorPos(state.window, &(state.mouseX), &(state.mouseY));

    for(int i = 0; i < objs.size(); i++) {
        objs[i]->update(&state);
    }

    for (int j = 0; j < edibles.size(); j++) {
        edibles[j]->update(&state);
    }
    //update camera
    cam.avatar = objs[0];
    cam.updateCamera();
    state.partManager->update(state.deltaTime);
    state.lawnHealth = (state.grassAlive * 100) / edibles.size();
    if(state.waterLevel > 100.0){
        state.waterLevel = 100.0;
    }
}

void WorldObj::addObj(GameObj *newObj) {
    newObj->worldObjs = &objs;
    objs.push_back(newObj);
    newObj->grid = &grid;
}

void WorldObj::setWindows(GLFWwindow *win) {
    state.window = win;
}

void WorldObj::makeFence(int row, int col){
    for(int r = 0; r <= row; r++){
        if(r != row/2 && r!= row/2 + 1) {
            GameObj *fence = new GameObj(getShape("fence"), getTexture("fenceText"));
            fence->name = "fence";
            fence->setPos(r * grid.offset, 2, col * grid.offset);
            fence->setScale(0.5,1.5,1.5);
            fence->setRot(0,M_PI/2,0);
            addObj(fence);
            grid.addToGrid(fence);
        }

        GameObj *fence = new GameObj(getShape("fence"), getTexture("fenceText"));
        fence->name = "fence";
        fence->setPos(r * grid.offset, 2, 0);
        fence->setScale(0.5,1.5,1.5);
        fence->setRot(0,-M_PI/2,0);
        addObj(fence);
        grid.addToGrid(fence);
    }
    for(int c = 0; c <= col; c++){
        if(c != col/2 && c!= col/2 + 1) {
            GameObj *fence = new GameObj(getShape("fence"), getTexture("fenceText"));
            fence->setPos(row * grid.offset, 2, c * grid.offset);
            fence->setScale(0.5,1.5,1.5);
            fence->name = "fence";
            addObj(fence);
            grid.addToGrid(fence);
        }
        GameObj *fence = new GameObj(getShape("fence"), getTexture("fenceText"));
        fence->setPos(0, 2, c * grid.offset);
        fence->setScale(0.5,1.5,1.5);
        fence->setRot(0,-M_PI,0);
        fence->name = "fence";
        addObj(fence);
        grid.addToGrid(fence);
    }
    for(int r = 1; r < row; r++) {
        for(int c = 1; c < col; c++) {
            EdibleGameObj *grass = new EdibleGameObj(getShape("goodGrass"), getTexture("grassText"));
            grass->setPos(r * grid.offset, 1.5, c * grid.offset);
            grass->setScale(1.5, 1, 1.5);
            grass->worldObjs = &objs;
            edibles.push_back(grass);
            grass->grid = &grid;
            grid.addToGrid(grass);
            state.grassAlive++;
        }
    }

    for (int r = 0; r < row; r += 2) {
        for (int c = 0; c < col; c += 2) {
            GameObj *sprinkler = new SupportGameObj(getShape("sprinkler"), getTexture("sprinklerText"));
            sprinkler->setPos(r * grid.offset + grid.offset / 2 , 1.25, c * grid.offset + grid.offset / 2);
            sprinkler->setScale(.15, .15, .15);
            addObj(sprinkler);
        }
    }
}
