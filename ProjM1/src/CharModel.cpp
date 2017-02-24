#include "CharModel.h"
#include <iostream>

/*
#define TORSO 0
#define HEAD 1
#define LEFT_ARM 2
#define RIGHT_ARM 3
#define LEFT_LEG 4
#define RIGHT_LEG 5
*/

CharModel::CharModel(shared_ptr<Shape> shape) {
    init();
    for (int i = 0; i < body.size(); i++) {
        body[i]->shape = shape;
    }
}

CharModel::CharModel(shared_ptr<Shape> shape, shared_ptr<MatrixStack> global) {
    init();
    for (int i = 0; i < body.size(); i++) {
        body[i]->shape = shape;
    }
    model_transform = global;
}

void CharModel::init_PlayerModel() {
    if (!body.empty()) {
        body[TORSO]->scale = vec3(1, 1.25, .5);
        body[TORSO]->offset = vec3(0, 0, 0);

        body[HEAD]->scale = vec3(.50, .50, .50);
        body[HEAD]->offset = vec3(0, .50, 0);
        body[HEAD]->attach_Limb = vec3(0, .65, 0);

        body[LEFT_ARM]->scale = vec3(.25, .75, .25);
        body[LEFT_ARM]->offset = vec3(.25, -.375, 0);
        body[LEFT_ARM]->rotation = vec3(1, 0, 0);
        body[LEFT_ARM]->attach_Limb = vec3(1, 0.25, 0);

        body[RIGHT_ARM]->scale = vec3(.25, .75, .25);
        body[RIGHT_ARM]->offset = vec3(-.25, -.375, 0);
        body[RIGHT_ARM]->rotation = vec3(1, 0, 0);
        body[RIGHT_ARM]->attach_Limb = vec3(-1, 0.25, 0);

        body[LEFT_LEG]->scale = vec3(.40, .70, .50);
        body[LEFT_LEG]->offset = vec3(.40, -.35, 0);
        body[LEFT_LEG]->attach_Limb = vec3(.20, -1.15, 0);

        body[RIGHT_LEG]->scale = vec3(.40, .70, .50);
        body[RIGHT_LEG]->offset = vec3(-.4, -.35, 0);
        body[RIGHT_LEG]->attach_Limb = vec3(-.20, -1.15, 0);
    }
}

void CharModel::render_Model(shared_ptr<Program> prog) {
    shared_ptr<MatrixStack> M = adjust_Part(TORSO, 0);
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    body[TORSO]->shape->draw(prog);

    for (int i = 1; i < body.size(); i++) {
        render_Part(prog, i);
    }

    M->popMatrix();
}

shared_ptr<MatrixStack> CharModel::adjust_Part(int part, float rotation) {
    model_transform->pushMatrix();
    model_transform->translate(body[part]->attach_Limb);
    model_transform->rotate(rotation, body[part]->rotation);
    model_transform->translate(body[part]->offset);
    model_transform->scale(body[part]->scale);
    return model_transform;
}

void CharModel::render_Part(shared_ptr<Program> prog, int part) {
    float angle;
    if (part != LEFT_ARM && part != RIGHT_ARM) {
        angle = 0;
    }
    else {
        angle = theta;
    }
    shared_ptr<MatrixStack> M = adjust_Part(part, angle);


    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    body[part]->shape->draw(prog);
    M->popMatrix();
}

void CharModel::init() {
    if (body.empty()) {
        for (int i = 0; i < RIGHT_LEG + 1; i++) {
            body.push_back(unique_ptr<Part>(new Part));
        }
    }
}

shared_ptr<MatrixStack> CharModel::getMatrix() {
    return model_transform;
}

shared_ptr<MatrixStack> CharModel::setMatrix(shared_ptr<MatrixStack> M) {
    return model_transform = M;
}

void CharModel::scare_Motion() {
    if (scare) {
        theta -= 0.25;
        if (theta < -2)
            scare = 0;
    }
    else {
        theta += 0.25;
        if (theta > 0) {
            scare = 1;
        }
    }
}