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

CharModel::CharModel() {
    //printf("You need to put parameters into this\n");
}

CharModel::CharModel(shared_ptr<Shape> shape) {
    init();
    for (int i = 0; i < body.size(); i++) {
        body[i]->shape = shape;
    }
}

CharModel::CharModel(shared_ptr<Shape> head, shared_ptr<Shape> torso,
    shared_ptr<Shape> arm, shared_ptr<Shape> leg) {
    init();
    body[HEAD]->shape = head;
    body[TORSO]->shape = torso;
    body[LEFT_ARM]->shape = body[RIGHT_ARM]->shape = arm;
    body[LEFT_LEG]->shape = body[RIGHT_LEG]->shape = leg;
}

CharModel::CharModel(shared_ptr<Shape> shape, shared_ptr<MatrixStack> global) {
    init();
    for (int i = 0; i < body.size(); i++) {
        body[i]->shape = shape;
    }
    model_transform = global;
}

CharModel::CharModel(vector<shared_ptr<Shape>> parts,
    vector<shared_ptr<Texture>> texs) {
    init();
    for (int i = 0; i < body.size(); i++) {
        body[i]->shape = parts[i];
        body[i]->tex = texs[i];
    }
}

void CharModel::init_Model() {
    if (!body.empty()) {
        body[TORSO]->offset = vec3(0, 0, 0);

        body[HEAD]->offset = vec3(0, .50, 0);
        body[HEAD]->attach_Limb = vec3(0, .65, 0);

        body[LEFT_ARM]->offset = vec3(0, -.50, 0);
        body[LEFT_ARM]->rotation = vec3(0, 0, 1);
        body[LEFT_ARM]->action_rotation = vec3(1, 0, 0);
        body[LEFT_ARM]->attach_Limb = vec3(-1, -.20, 0);

        body[RIGHT_ARM]->offset = vec3(0, -.5, 0);
        body[RIGHT_ARM]->rotation = vec3(0, 0, 1);
        body[RIGHT_ARM]->action_rotation = vec3(1, 0, 0);
        body[RIGHT_ARM]->attach_Limb = vec3(1, -.20, 0);

        body[LEFT_LEG]->offset = vec3(0, -.35, 0);
        body[LEFT_LEG]->action_rotation = vec3(1, 0, 0);
        body[LEFT_LEG]->attach_Limb = vec3(-.50, -1.5, 0);

        body[RIGHT_LEG]->offset = vec3(0, -.35, 0);
        body[RIGHT_LEG]->action_rotation = vec3(1, 0, 0);
        body[RIGHT_LEG]->attach_Limb = vec3(.50, -1.5, 0);
    }
}


void CharModel::render_Model(shared_ptr<Program> prog) {

    shared_ptr<MatrixStack> M = adjust_Part(TORSO, 0);
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,
        value_ptr(M->topMatrix()));

    body[TORSO]->tex->bind();
    body[TORSO]->shape->draw(prog);

    for (int i = 1; i < body.size(); i++) {
        render_Part(prog, i);
    }

    M->popMatrix();
}

void CharModel::render_Part(shared_ptr<Program> prog, int part) {
    float rotation = 0;
    if (part == LEFT_LEG || part == RIGHT_LEG || part == RIGHT_ARM) {
        if (part == LEFT_LEG || part == RIGHT_ARM)
            rotation = legs;
        else
            rotation = -legs;
    }

    if (part == LEFT_ARM || part == 6) {
       rotation = arms;
    }

    model_transform->pushMatrix();
    shared_ptr<MatrixStack> M = adjust_Part(part, rotation);

    body[part]->tex->bind();
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    body[part]->shape->draw(prog);
    model_transform->popMatrix();
}

void CharModel::init() {
    if (body.empty()) {
        for (int i = 0; i < RIGHT_LEG + 1; i++) {
            body.push_back(unique_ptr<Part>(new Part));
        }
    }
}

shared_ptr<MatrixStack> CharModel::adjust_Part(int part, float rotation) {
    model_transform->translate(body[part]->attach_Limb);
    model_transform->rotate(rotation, body[part]->action_rotation);
    model_transform->translate(body[part]->offset);
    model_transform->scale(body[part]->scale);

    if (body[part]->rotation != vec3(0)) {
        model_transform->rotate(-M_PI / 2.0, body[part]->rotation);
    }

    return model_transform;
}

shared_ptr<MatrixStack> CharModel::getMatrix() {
    return model_transform;
}

shared_ptr<MatrixStack> CharModel::setMatrix(shared_ptr<MatrixStack> M) {
    return model_transform = M;
}

bool CharModel::scare_Motion() {
    if (scare) {
        arms -= 0.10;
        if (arms < -2) {
            scare = 0;
        }
    }
    else {
        arms += 0.10;
        if (arms > 0) {
            scare = 1;
            return false;
        }
    }
    return true;
}

void CharModel::walk_Motion() {
    if (walk) {
        legs -= 0.05;
        if (legs < -1)
            walk = 0;
    }
    else {
        legs += 0.05;
        if (legs > 1) {
            walk = 1;
        }
    }
}