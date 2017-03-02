#include "toonshading.h"

ToonShading::ToonShading() {
    toonProg = make_shared<Program>();
}

void ToonShading::initShader(string directory) {
    toonProg->setVerbose(true);
    toonProg->setShaderNames(directory + "silhouette_vert.glsl", directory + "silhouette_frag.glsl");
    toonProg->init();
    toonProg->addUniform("P");
    toonProg->addUniform("V");
    toonProg->addUniform("M");
    toonProg->addAttribute("vertPos");
    toonProg->addAttribute("vertNor");
    toonProg->addAttribute("vertTex");
    toonProg->addUniform("silhouette_color");
    toonProg->addUniform("silhouette_offset");

    glUniform3f(toonProg->getUniform("silhouette_color"), 0.0, 0.0, 0.0);
    glUniform1f(toonProg->getUniform("silhouette_offset"), 0.65f);
}

void ToonShading::render(mat4 P, mat4 V) {
    toonProg->bind();


    toonProg->unbind();
    glClear(GL_DEPTH_BUFFER_BIT);
}