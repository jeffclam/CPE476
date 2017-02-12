#include "Lighting.h"
#include <iostream>

void Lighting::SetLightUniforms(shared_ptr<Program> prog) {
    string name = "lights";
    string index = "[x]";
    string pos = ".pos";
    string intensity = ".intensity";
    string ambCoeff = ".ambCoeff";
    //cout << lights.size() << endl;
    for (int i = 0; i < lights.size(); i++) {
        index[1] = (char)(i + '0');
        glUniform4f(prog->getUniform((name + index + pos).c_str()), lights[i].pos.x, lights[i].pos.y, lights[i].pos.z, lights[i].pos.w);
        glUniform3f(prog->getUniform((name + index + intensity).c_str()), lights[i].intensity.x, lights[i].intensity.y, lights[i].intensity.z);
        glUniform1f(prog->getUniform((name + index + ambCoeff).c_str()), lights[i].ambCoeff);
    }
}

void Lighting::push_back(Light l) {
    lights.push_back(l);
}

int Lighting::size() {
    return lights.size();
}