#include "thing.h"

Thing::Thing(string name) :
    mat(0),
    pos(vec3(0,0,0)),
    scale(vec3(1,1,1)),
    rot(vec3(0,0,0)),
    name(name)
{
}

void Thing::setPos(float x, float y, float z) {
    pos = vec3(x,y,z);
}

void Thing::setScale(float x, float y, float z) {
    scale = vec3(x,y,z);
}

void Thing::setRot(float x, float y, float z) {
    rot = vec3(x,y,z);
}

Thing Thing::copy() {
    Thing thingCopy = Thing(name);
    thingCopy.setPos(pos[0], pos[1], pos[2]);
    thingCopy.setScale(scale[0], scale[1], scale[2]);
    thingCopy.setRot(rot[0], rot[1], rot[2]);
    thingCopy.mat = mat;
    return thingCopy;
}

void Thing::setName(string newName) {
    name = newName;
}

shared_ptr<MatrixStack> Thing::getM(shared_ptr<MatrixStack> M) {
    M->pushMatrix();
    M->loadIdentity();
    M->translate(pos);
    M->rotate(rot[0], vec3(1,0,0));
    M->rotate(rot[1], vec3(0,1,0));
    M->rotate(rot[2], vec3(0,0,1));
    M->scale(scale);
    return M;
}

Json::Value Thing::getSave() {
    Json::Value root;
    
    root["pX"] = pos[0];
    root["pY"] = pos[1];
    root["pZ"] = pos[2];
    
    root["sX"] = scale[0];
    root["sY"] = scale[1];
    root["sZ"] = scale[2];
    
    root["rX"] = rot[0];
    root["rY"] = rot[1];
    root["rZ"] = rot[2];
    
    root["name"] = name;
    root["mat"] = mat;
    
    return root;
}

void Thing::setFromSave(Json::Value root) {
    
    mat = root["mat"].asInt();
    name = root["name"].asString();
    
    pos[0] = root["pX"].asFloat();
    pos[1] = root["pY"].asFloat();
    pos[2] = root["pZ"].asFloat();
    
    scale[0] = root["sX"].asFloat();
    scale[1] = root["sY"].asFloat();
    scale[2] = root["sZ"].asFloat();
    
    rot[0] = root["rX"].asFloat();
    rot[1] = root["rY"].asFloat();
    rot[2] = root["rZ"].asFloat();
}
