#include "SupportGameObj.h"

SupportGameObj::SupportGameObj(shared_ptr<Shape> shape, shared_ptr<Texture> tex) : 
    GameObj(shape, tex)
{
    name = "sprinkler";
    interact_radius = 30.0f;
}

void SupportGameObj::update(GameState *state) {
    float up = .025;
    if (was_Pushed) {
        if (getPos().y < 1.25)
            setPos(getPos().x, getPos().y + up, getPos().z);
        else {
            if (sprayTime >= SPRAY_TIME) {
                spray(state);
                was_Pushed = false;
            } else {
                sprayTime += state->deltaTime;
            }
        }
    } else {
        if (sprayTime <= 0) {
            if (getPos().y > SPRINKLER_MIN_HEIGHT)
                setPos(getPos().x, getPos().y - up, getPos().z);
        }
        else {
            sprayTime -= state->deltaTime;
        }
    }
}

void SupportGameObj::sprinkle(GameState *state) {
    was_Pushed = true;
}

void SupportGameObj::spray(GameState *state) {
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