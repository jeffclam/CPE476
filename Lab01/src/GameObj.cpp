#include "GameObj.h"

GameObj::GameObj() {

}

GameObj::GameObj(shared_ptr<Shape> shape) {
    
}

GameObj::~GameObj() {

}

void GameObj::update(double time) {

}

void GameObj::render(shared_ptr<Program> p, shared_ptr<Shape> shape) {

}

vec3 GameObj::getPos() {
    return pos;
}

void GameObj::setPos(float x, float y, float z) {
    pos = vec3(x, y, z);
}

void GameObj::setScale(float x, float y, float z) {

}

void GameObj::setRot(float x, float y, float z) {

}

shared_ptr<MatrixStack> GameObj::getM(shared_ptr<MatrixStack> M) {

}

void GameObj::calcBoundingSphere() {

}

bool GameObj::check_Interact_Radius() {

}

bool GameObj::check_Collision_Radius() {

}