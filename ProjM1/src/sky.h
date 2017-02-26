#ifndef _SKY_H
#define _SKY_H

#include "stuff.h"
#include "Texture.h"
#include "Program.h"

class Sky{
    public:
    Sky();
    GLuint loadCubemap();
    void initShader(string vert, string frag);
    void setRight(shared_ptr<Texture>text){texts.push_back(text);};//texts[0] = text;};
    void setLeft(shared_ptr<Texture>text){texts.push_back(text);};//texts[1] = text;};
    void setTop(shared_ptr<Texture>text){texts.push_back(text);};//texts[2] = text;};
    void setBottom(shared_ptr<Texture>text){texts.push_back(text);};//texts[3] = text;};
    void setBack(shared_ptr<Texture>text){texts.push_back(text);};//texts[4] = text;};
    void setFront(shared_ptr<Texture>text){texts.push_back(text);};//texts[5] = text;};
    void render(mat4 P, mat4 V);
    private:
    shared_ptr<Program> skyProg;
    GLuint skyboxVAO, skyboxVBO;
    vector<shared_ptr<Texture>> texts;
    GLuint skyboxTexture;
};

#endif