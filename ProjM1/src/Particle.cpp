#include <iostream>
#include "Particle.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"

using namespace std;

float randFloat(float l, float h)
{
	float r = rand() / (float)RAND_MAX;
	return (1.0f - r) * l + r * h;
}

Particle::Particle(bool dies, vec4 startColor, vec3 startPos) :
	pos(0.0f, 0.0f, 0.0f),
	v(0.0f, 0.0f, 0.0f),
	lifespan(1.0f),
	scale(1.0f),
	color(1.0f, 1.0f, 1.0f, 1.0f)
{
    doesDie = dies;
    baseColor = startColor;
    origin = startPos;
}

Particle::~Particle()
{
}

void Particle::load()
{
	// Random initialization
	rebirth();
}

/* all particles born at the origin */
void Particle::rebirth(){
    //set ransom pos based around origin
    //pos.x = randFloat(-0.05, 0.05);;
    //pos.y = -0.5;
    //pos.z = randFloat(-0.05, -0.05);
    pos = origin; 
    //set random velocity
	v.x = randFloat(-2.0f, 2.0f);
	v.y = randFloat(1.5f, 3.5f);
	v.z = randFloat(-2.0f, 2.0f);
	lifespan = randFloat(5.5f, 8.0f);
    timeToLive = lifespan;
	scale = randFloat(0.2f, 1.0f);
    //set random color based around baseColor
    color.r = randFloat(-0.1f, 0.1f);
    color.g = randFloat(-0.1f, 0.1f);
    color.b = randFloat(-0.1f, 0.1f);
	color = color + baseColor;
}

void Particle::update(float deltaTime) {
    //check for particle death
    if(timeToLive <= 0 && !doesDie) {
        rebirth();
        return;
    } else if(doesDie && timeToLive <= 0) {
        color.a = 0.0;
        pos = vec3(-10, -10, -10);
        return;
    }

    pos += v * deltaTime;
    timeToLive -= deltaTime;
    v.y -= (1) * deltaTime;
}

/*void Particle::update(float t, float h, const vec3 &g, const bool *keyToggles)
{
	if(t > tEnd) {
		rebirth(t);
	}

	//very simple update
	x += h*v;
    v.y -= 0.02f;
	color.a = (tEnd-t)/lifespan;
}*/