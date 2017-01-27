#include "GameObj.h"
#include <algorithm>

GameObj::GameObj() {
   pos = vec3(0, 0, 0);
   vel = vec3(0, 0, 0);
    srand (time(NULL));
}

GameObj::GameObj(shared_ptr<Shape> s, Texture *tex) {
    shape = s;
    pos = vec3(0, 0, 0);
    scale = vec3(1, 1, 1);
    rot = vec3(0, 0, 0);
    vel = vec3(0, 0, 0);
    srand (time(NULL));
    texture = tex;
}

GameObj::~GameObj() {

}

void GameObj::update(GameState state) {
    
    float time = state.deltaTime;
    
    GameObj *collider = check_Collision_Radius();
    
    if (collider == NULL) {
        pos += vel*((float)5*time);
    } else {
        pos -= vel*((float)5*time);
        setRandomVel();
    }
}

void GameObj::setRandomVel() {
    //srand (time(NULL));
    vel[0] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2.0 - 1.0) * 2;
    //vel[1] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    vel[2] = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2.0 - 1.0) * 2;
}

void GameObj::render(shared_ptr<Program> prog) {
    //prog->addTexture(texture);
    texture->bind();
    auto M = make_shared<MatrixStack>();
    M = getM(M);
    calcBoundingBox(M->topMatrix());
    calcBoundingSphere();
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    shape->draw(prog);
    M->popMatrix();
    texture->unbind();
}

void GameObj::setShape(shared_ptr<Shape> s) {
   shape = s;
}

void GameObj::setWorldObjs(vector<GameObj*> *objects) {
    worldObjs = objects;
}

vec3 GameObj::getPos() {
    return pos;
}

void GameObj::setPos(float x, float y, float z) {
    pos = vec3(x, y, z);
}

void GameObj::setPos(vec3 v) {
    pos = v;
}

vec3 GameObj::getVel() {
    return vel;
}

void GameObj::setVel(float x, float y, float z) {
    vel = vec3(x, y, z);
}

void GameObj::setScale(float x, float y, float z) {
    scale = vec3(x, y, z);
}

void GameObj::setRot(float x, float y, float z) {
    rot = vec3(x, y, z);
}

shared_ptr<MatrixStack> GameObj::getM(shared_ptr<MatrixStack> M) {
    M->pushMatrix();
    M->loadIdentity();
    M->translate(pos);
    M->rotate(rot[0], vec3(1,0,0));
    M->rotate(rot[1], vec3(0,1,0));
    M->rotate(rot[2], vec3(0,0,1));
    M->scale(scale);
    return M;
}

void GameObj::calcBoundingBox(mat4 transform) {
    b_box.min = vec3(transform * vec4(shape->getMin(), 0.0));
    b_box.max = vec3(transform * vec4(shape->getMax(), 0.0));
}

float GameObj::calcBoundingRadius() {
    return distance(b_box.min, b_box.max) / 16.0f;
}

void GameObj::calcBoundingSphere() {
    //b_sphere.radius = calcBoundingRadius();
    b_sphere.radius = 1;
    b_sphere.center = pos;
}

bool GameObj::check_Interact_Radius() {
    return 0;
}

GameObj *GameObj::check_Collision_Radius() {
    float dist;
    for (int i = 0; i < worldObjs->size(); i++) {
        GameObj other = *(*worldObjs)[i];
        dist = distance(getPos(), other.getPos());
        if (dist > 0 && dist <= other.b_sphere.radius) {
            return (*worldObjs)[i];
        }
    }
    return NULL;
}
