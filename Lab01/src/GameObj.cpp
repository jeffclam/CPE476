#include "GameObj.h"

GameObj::GameObj() {
   pos = vec3(0, 0, 0);
   vel = vec3(0, 0, 0);
}

GameObj::GameObj(shared_ptr<Shape> s) {
    shape = s;
    pos = vec3(0,0,0);
    scale = vec3(1,1,1);
    rot = vec3(0,0,0);
}

GameObj::~GameObj() {

}

void GameObj::update(double time) {

}

void GameObj::render(shared_ptr<Program> prog) {
    auto M = make_shared<MatrixStack>();
    M = getM(M);
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    shape->draw(prog);
    M->popMatrix();
}

void GameObj::setShape(shared_ptr<Shape> s) {
   shape = s;
}

vec3 GameObj::getPos() {
    return pos;
}

void GameObj::setPos(float x, float y, float z) {
    pos = vec3(x, y, z);
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

void GameObj::calcBoundingSphere() {
   b_sphere.center = pos;
}

bool GameObj::check_Interact_Radius() {
    return 0;
}

bool GameObj::check_Collision_Radius() {
    return 0;
}
