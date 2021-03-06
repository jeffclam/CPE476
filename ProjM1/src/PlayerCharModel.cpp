#include "PlayerCharModel.h"

PlayerCharModel::PlayerCharModel(vector<shared_ptr<Shape>> parts,
    vector<shared_ptr<Texture>> texs) {
    init();
    /* for the cane */
    body.push_back(unique_ptr<Part>(new Part));
    for (int i = 0; i < body.size(); i++) {
        body[i]->shape = parts[i];
        body[i]->tex = texs[i];
    }
}

void PlayerCharModel::init_Model() {
    if (!body.empty()) {
        body[TORSO]->offset = vec3(0, 0, 0);

        body[HEAD]->offset = vec3(0, .50, 0);
        body[HEAD]->attach_Limb = vec3(0, .5, .5);

        body[LEFT_ARM]->scale = vec3(.75, .75, .75);
        body[LEFT_ARM]->offset = vec3(0, -1.0, 0);
        body[LEFT_ARM]->rotation = vec3(0, 0, 1);
        body[LEFT_ARM]->action_rotation = vec3(1, 0, 0);
        body[LEFT_ARM]->attach_Limb = vec3(-.8, .50, .40);

        body[CANE]->scale = vec3(.25, .80, .25);
        body[CANE]->offset = vec3(0, -2.5, 0);
        body[CANE]->rotation = vec3(0, 0, 1);
        body[CANE]->action_rotation = vec3(1, 0, 0);
        body[CANE]->attach_Limb = vec3(-.8, .50, .40);

        body[RIGHT_ARM]->scale = vec3(.75, .75, .75);
        body[RIGHT_ARM]->offset = vec3(0, -1.0, 0);
        body[RIGHT_ARM]->rotation = vec3(0, 0, 1);
        body[RIGHT_ARM]->action_rotation = vec3(1, 0, 0);
        body[RIGHT_ARM]->attach_Limb = vec3(.8, .50, .40);

        body[LEFT_LEG]->scale = vec3(.75, .65, .75);
        body[LEFT_LEG]->offset = vec3(0, -1, 0);
        body[LEFT_LEG]->action_rotation = vec3(1, 0, 0);
        body[LEFT_LEG]->attach_Limb = vec3(-.50, -.75, 0.25);

        body[RIGHT_LEG]->scale = vec3(.75, .65, .75);
        body[RIGHT_LEG]->offset = vec3(0, -1, 0);
        body[RIGHT_LEG]->action_rotation = vec3(1, 0, 0);
        body[RIGHT_LEG]->attach_Limb = vec3(.50, -.75, 0.25);
    }
}

bool PlayerCharModel::scare_Motion() {
    if (scare) {
        arms -= 0.15;
        if (arms <= -3) {
            scare = 0;
        }
    }
    else {
        arms += 0.15;
        if (arms >= 0) {
            arms = 0;
            scare = 1;
            return false;
        }
    }
    return true;
}

void PlayerCharModel::walk_Motion() {
    if (walk) {
        legs -= 0.05;
        if (legs < -.5)
            walk = 0;
    }
    else {
        legs += 0.05;
        if (legs > .5) {
            walk = 1;
        }
    }
    arms = -legs;
}