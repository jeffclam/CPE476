#pragma once

#include "stuff.h"
#include "MatrixStack.h"
#include "Texture.h"
#include "Program.h"

class ToonShading {
public:
    ToonShading();
    void initShader(string directory);
    void render(mat4 P, mat4 V);
    shared_ptr<Program> toonProg;
};