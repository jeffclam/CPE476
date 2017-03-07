#pragma once

#include "CharModel.h"

class EnemyCharModel : public CharModel {
public:
    EnemyCharModel(vector<shared_ptr<Shape>> parts, vector<shared_ptr<Texture>> texs);
    void init_Model();
    bool scare_Motion();
    void walk_Motion();
};