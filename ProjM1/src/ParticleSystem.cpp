#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(bool dies, vec4 startColor, vec3 startPos) {
    origin = startPos;
    baseColor = startColor;
    isInfinite = !dies;
    for(int i = 0; i < partNum; i++) {
        Particle newPart = Particle(dies, startColor, startPos);
        newPart.load();
        particles.push_back(newPart);
    }
    initGeom();
}

void ParticleSystem::update(float deltaTime) {
    for(int i = 0; i < particles.size(); i++) {
        particles[i].update(deltaTime);
        if(particles[i].isDead()) {
            particles.erase(particles.begin() + i);
        }
    }
}

void ParticleSystem::initGeom() {

   //generate the VAO
   glGenVertexArrays(1, &VertexArrayID);
   glBindVertexArray(VertexArrayID);

   //generate vertex buffer to hand off to OGL - using instancing
   glGenBuffers(1, &pointsbuffer);
   //set the current state to focus on our vertex buffer
   glBindBuffer(GL_ARRAY_BUFFER, pointsbuffer);
   //actually memcopy the data - only do this once
   glBufferData(GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STREAM_DRAW);
   
	glGenBuffers(1, &colorbuffer);
   //set the current state to focus on our vertex buffer
   glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
   //actually memcopy the data - only do this once
   glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), NULL, GL_STREAM_DRAW);
   //cout << glGetError() << "\n";
	//assert(glGetError() == GL_NO_ERROR);

}

//Note you could add scale later for each particle - not implemented
void ParticleSystem::updateGeom() {
	vec3 pos;
	vec4 col;

	//go through all the particles and update the CPU buffer
   for (int i = 0; i < partNum; i++) {
		pos = particles[i].getPosition();
		col = particles[i].getColor();
		points[i*3+0] =pos.x; 
		points[i*3+1] =pos.y; 
		points[i*3+2] =pos.z; 
		pointColors[i*4+0] =col.r + col.a/10; 
		pointColors[i*4+1] =col.g + col.g/10; 
		pointColors[i*4+2] =col.b + col.b/10;
		pointColors[i*4+3] =col.a;
	} 

	//update the GPU data
	glBindBuffer(GL_ARRAY_BUFFER, pointsbuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STREAM_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*partNum*3, points);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), NULL, GL_STREAM_DRAW);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*partNum*4, pointColors);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::render(shared_ptr<Program> prog) {
    updateGeom();
  
   glEnableVertexAttribArray(0);
   glBindBuffer(GL_ARRAY_BUFFER, pointsbuffer);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,(void*)0);
   
	glEnableVertexAttribArray(1);
   glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0,(void*)0);
   
	glVertexAttribDivisor(0, 1);
	glVertexAttribDivisor(1, 1);
   // Draw the points !
   glBindVertexArray(VertexArrayID);
	glDrawArraysInstanced(GL_POINTS, 0, 1, partNum);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}




ParticleManager::ParticleManager() {
}

void ParticleManager::init(string vert, string frag) {
    partProg = make_shared<Program>();
    partProg->setVerbose(true);
	partProg->setShaderNames(vert, frag);
	partProg->init();
    partProg->addUniform("P");
    partProg->addUniform("MV");
    partProg->addUniform("alphaTexture");
	partProg->addAttribute("vertPos");
	partProg->addAttribute("Pcolor");
    tex = getTexture("particle");
}

void ParticleManager::update(float deltaTime) {
    for(int i = 0; i < systems.size(); i++) {
        systems[i].update(deltaTime);
        if(systems[i].isEmpty()) {
            systems.erase(systems.begin() + i);
        }
    }
}

void ParticleManager::addParticleSystem(bool dies, vec4 startColor, vec3 startPos) {
    ParticleSystem newSystem = ParticleSystem(dies, startColor, startPos);
    systems.push_back(newSystem);
}

void ParticleManager::render(tmat4x4<float> MV, tmat4x4<float> P) {
    partProg->bind();
    glUniformMatrix4fv(partProg->getUniform("P"), 1, GL_FALSE, value_ptr(P));
	glUniformMatrix4fv(partProg->getUniform("MV"), 1, GL_FALSE, value_ptr(MV));
    glActiveTexture(GL_TEXTURE0);
    tex->bind();
    glUniform1i(partProg->getUniform("alphaTexture"), 0);
    for(int i = 0; i < systems.size(); i++) {
        systems[i].render(partProg);
    }
    partProg->unbind();
}