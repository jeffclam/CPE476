//
//  WorldObject.cpp
//  Lab01
//
//  Created by Andrew Adriance on 1/19/17.
//
//

#include "WorldObj.h"

WorldObj::WorldObj():
    objs()
{}

void WorldObj::render(shared_ptr<Program> prog) {
    for(int i = 0; i < objs.size(); i++) {
        auto M = make_shared<MatrixStack>();
        M = objs[i].getM(M);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        //setMat(things[i].mat, prog);
        //(getThing(things[i].getName()))->draw(prog);
    }
}
