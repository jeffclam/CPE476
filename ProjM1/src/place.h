#pragma once
#ifndef _PLACE_H_
#define _PLACE_H_

#include "stuff.h"
#include "thing.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"


#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Place {
    public:
        Place();
        void addThing(Thing toAdd);
        void renderPlace(shared_ptr<Program> prog);
        void placeCursor();
        void toggleCursor();
        Thing cursor;
        Json::Value save();
        void load(Json::Value root);
    
    private:
        std::vector<Thing> things;
        string cursorName;
};



#endif
