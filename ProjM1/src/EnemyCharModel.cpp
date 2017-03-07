#include "EnemyCharModel.h"

EnemyCharModel::EnemyCharModel(vector<shared_ptr<Shape>> parts,
    vector<shared_ptr<Texture>> texs) {
    init();
    for (int i = 0; i < body.size(); i++) {
        body[i]->shape = parts[i];
        body[i]->tex = texs[i];
    }
}

void EnemyCharModel::init_Model() {
    if (!body.empty()) {
        body[TORSO]->scale = vec3(1.25, 1.25, 1.25);
        body[TORSO]->offset = vec3(0, 0, 0);

        body[HEAD]->offset = vec3(0, 0, 1);
        body[HEAD]->attach_Limb = vec3(0, .65, 0);

        body[LEFT_ARM]->scale = vec3(.25, .25, .25);
        body[LEFT_ARM]->offset = vec3(0, -.25, 0);
        body[LEFT_ARM]->action_rotation = vec3(1, 0, 0);
        body[LEFT_ARM]->attach_Limb = vec3(-.35, -.35, .35);

        body[RIGHT_ARM]->scale = vec3(.25, .25, .25);
        body[RIGHT_ARM]->offset = vec3(0, -.25, 0);
        body[RIGHT_ARM]->action_rotation = vec3(1, 0, 0);
        body[RIGHT_ARM]->attach_Limb = vec3(.35, -.35, .35);

        body[LEFT_LEG]->scale = vec3(.25, .25, .25);
        body[LEFT_LEG]->offset = vec3(0, -.25, 0);
        body[LEFT_LEG]->action_rotation = vec3(1, 0, 0);
        body[LEFT_LEG]->attach_Limb = vec3(-.35, -.35, -.35);

        body[RIGHT_LEG]->scale = vec3(.25, .25, .25);
        body[RIGHT_LEG]->offset = vec3(0, -.25, 0);
        body[RIGHT_LEG]->action_rotation = vec3(1, 0, 0);
        body[RIGHT_LEG]->attach_Limb = vec3(.35, -.35, -.35);
    }
}

bool EnemyCharModel::scare_Motion() {
    if (scare) {
        body[TORSO]->scale += .25;
        if (body[TORSO]->scale == vec3(2.5))
            scare = false;
    }
    else {
        body[TORSO]->scale -= .25;
        if (body[TORSO]->scale[0] <= 1.25) {
            scare = true;
            return false;
        }
    }
    return true;
}

void EnemyCharModel::walk_Motion() {
    if (walk) {
        legs -= 0.05;
        arms += 0.05;
        if (legs < -1)
            walk = 0;
    }
    else {
        legs += 0.05;
        arms -= 0.05;
        if (legs > 1) {
            walk = 1;
        }
    }
}