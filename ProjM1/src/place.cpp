#include "place.h"
#include "stuff.h"

Place::Place() :
    cursor(Thing("NONE")),
    things(),
    cursorName("NONE")
{
}

void Place::toggleCursor() {
    if("NONE" == cursor.getName()) {
        cursor.setName(cursorName);
    } else {
        cursorName = cursor.getName();
        cursor.setName("NONE");
    }
}

void Place::addThing(Thing toAdd) {
    things.push_back(toAdd);
}

void Place::placeCursor() {
    if(cursor.getName() != "NONE") {
        addThing(cursor.copy());
    }
}

void Place::renderPlace(shared_ptr<Program> prog) {
    if(cursor.getName() != "NONE") {
        auto M = make_shared<MatrixStack>();
        M = cursor.getM(M);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        setMat(cursor.mat, prog);
        (getThing(cursor.getName()))->draw(prog);
    }
    for(int i = 0; i < things.size(); i++) {
        auto M = make_shared<MatrixStack>();
        M = things[i].getM(M);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
        setMat(things[i].mat, prog);
        (getThing(things[i].getName()))->draw(prog);
    }
}

Json::Value Place::save(){
    Json::Value root;
    root[0] = cursor.getSave();
    for(int i = 0; i < things.size(); i++) {
        root[i+1] = things[i].getSave();
    }
    return root;
}

void Place::load(Json::Value root) {
    things.clear();
    cursor.setFromSave(root[0]);
    for(int i = 1; i < root.size(); ++i) {
        Thing newThing = Thing("NONE");
        newThing.setFromSave(root[i]);
        addThing(newThing);
    }
}
