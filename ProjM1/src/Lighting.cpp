#include "Lighting.h"
#include <iostream>

void Lighting::SetLightUniforms(shared_ptr<Program> prog) {
    string name = "lights";
    string index = "[x]";
    string pos = ".pos";
    string intensity = ".intensity";
    string ambCoeff = ".ambCoeff";
    prog->bind();
    glUniform1i(prog->getUniform("numLights"), lights.size());
    for (int i = 0; i < lights.size(); i++) {
        index[1] = (char)(i + '0');
        glUniform4f(prog->getUniform((name + index + pos).c_str()), lights[i].pos.x, lights[i].pos.y, lights[i].pos.z, lights[i].pos.w);
        glUniform3f(prog->getUniform((name + index + intensity).c_str()), lights[i].intensity.x, lights[i].intensity.y, lights[i].intensity.z);
        glUniform1f(prog->getUniform((name + index + ambCoeff).c_str()), lights[i].ambCoeff);
    }
    prog->unbind();
}

void Lighting::push_back(Light l) {
    lights.push_back(l);
}

int Lighting::size() {
    return lights.size();
}

/* set up the FBO for the light's depth */
void Lighting::initShadow() {

	//generate the FBO for the shadow depth
	glGenFramebuffers(1, &depthMapFBO);

	//generate the texture
	glGenTextures(1, &depthMap);
 	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, S_WIDTH, S_HEIGHT, 
										 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//bind with framebuffer's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Lighting::initShadowProg(string resDir) {
    hasShadow = true;
    // Initialize the GLSL programs
	DepthProg = make_shared<Program>();
	DepthProg->setVerbose(true);
	DepthProg->setShaderNames(resDir + "depth_vert.glsl", resDir + "depth_frag.glsl");
	DepthProg->init();

    // Add uniform and attributes to each of the programs
	DepthProg->addUniform("LP");
	DepthProg->addUniform("LV");
	DepthProg->addUniform("M");
	DepthProg->addAttribute("vertPos");
    DepthProg->addAttribute("vertTex");
    DepthProg->addAttribute("vertNor");
}

mat4 Lighting::SetOrthoMatrix() {
	mat4 ortho = glm::ortho(-50.0, 50.0, -50.0, 50.0, 0.1, 100.0);
	glUniformMatrix4fv(DepthProg->getUniform("LP"), 1, GL_FALSE, value_ptr(ortho));
  return ortho;
}

mat4 Lighting::SetLightView() {
    vec3 pos = vec3(lights[0].pos);
 	mat4 Cam = glm::lookAt(pos, vec3(pos[0],0,pos[1]-1), vec3(0,1,0));
	glUniformMatrix4fv(DepthProg->getUniform("LV"), 1, GL_FALSE, value_ptr(Cam));
	return Cam;
}