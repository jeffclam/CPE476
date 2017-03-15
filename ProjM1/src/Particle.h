#pragma once
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class MatrixStack;
class Program;
class Texture;

class Particle
{
public:
	Particle(bool dies, vec4 startColor, vec3 startPos);
	virtual ~Particle();
	void load();
	void rebirth();
	void update(float deltaTime);
	const vec3 &getPosition() const { return pos; };
	const vec3 &getVelocity() const { return v; };
	const vec4 &getColor() const { return color; };
    const bool isDead() const { return timeToLive <= 0 && doesDie; };
	
private:
	vec3 pos; // position
	vec3 v; // velocity
    vec3 origin; // starting position
	float lifespan; // how long this particle lives
	float timeToLive;     // time this particle dies
	float scale;
    vec4 baseColor;
	vec4 color;
    bool doesDie;
};

#endif