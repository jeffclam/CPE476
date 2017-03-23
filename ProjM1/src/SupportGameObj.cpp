#include "SupportGameObj.h"

SupportGameObj::SupportGameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex) : 
    GameObj(shape, tex)
{
    name = "sprinkler";
    interact_radius = 30.0f;
}

void SupportGameObj::sprinkle(GameState *state) {
    EnemyGameObj *enemy;

    state->waterLevel = 0;
    for (int i = 0; i < (*worldObjs).size(); i++) {
        enemy = (EnemyGameObj *)(*worldObjs)[i];
        if (enemy->getName() == "enemy") {
            if (check_Interact_Radius(*enemy)) {
                if (!enemy->isScared)
                    state->waterLevel += 2.0;
                enemy->scare(this);
            }
        }
    }
    state->partManager->addParticleSystem(true, vec4(0.1, 0.1, 0.9, 1.0), getPos() + vec3(0, .1, 0));
}