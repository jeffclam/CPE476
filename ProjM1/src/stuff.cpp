#include "stuff.h"

unordered_map<string, shared_ptr<Shape>> loadedThings;
unordered_map<string, shared_ptr<Texture>> loadedText;

void loadStuff(string resourceDir, string stuffFile){
    Json::Value root;
    Json::Reader reader;
    std::ifstream stuffIn(resourceDir + stuffFile, std::ifstream::binary);
    
    bool parsingSuccessful = reader.parse( stuffIn, root, false );
    if ( !parsingSuccessful )
    {
        // report to the user the failure and their locations in the document.
        std::cout << "Error reading stuff file: "
        << reader.getFormatedErrorMessages()
        << "\n";
    }
    Json::Value things;
    things = root["shapes"];
    for ( int index = 0; index < things.size(); ++index ) {
        Json::Value rootThing = things[index];

        std::string name = rootThing.get("name", "NAME_NONE" ).asString();
        std::string obj = rootThing.get("obj", "OBJ_NONE" ).asString();
        
        auto shape = make_shared<Shape>();
        shape->loadMesh(resourceDir + obj);
        shape->resize();
        shape->init();
        pair<string, shared_ptr<Shape>> newThing (name, shape);
        loadedThings.insert(newThing);
    }

    things = root["textures"];
    for ( int index = 0; index < things.size(); ++index ) {
        Json::Value rootThing = things[index];

        std::string nameText = rootThing.get("name", "NAME_NONE" ).asString();
        std::string text = rootThing.get("text", "OBJ_NONE" ).asString();
        cout << "loading " << nameText << "\n";
        auto texPtr = make_shared<Texture>();
        texPtr->setFilename(resourceDir + text);
        texPtr->setUnit(0);
        texPtr->setName("tex");
        texPtr->init();
        pair<string, shared_ptr<Texture>> newText (nameText, texPtr);
        loadedText.insert(newText);
    }

}

shared_ptr<Shape> getShape(string name) {
    std::unordered_map<std::string,shared_ptr<Shape>>::const_iterator got = loadedThings.find (name);
    return got->second;
}

shared_ptr<Texture> getTexture(string name) {
    std::unordered_map<std::string,shared_ptr<Texture>>::const_iterator got = loadedText.find (name);
    return got->second;
}

void setMat(int mat, shared_ptr<Program> prog) {
    glUniform3f(prog->getUniform("lightColor"), 1, 1, 1);
    glUniform3f(prog->getUniform("lightPos"), 0, 0, 0);
    switch (mat) {
        case 0: //shiny blue plastic
            glUniform3f(prog->getUniform("ambColor"), 0.02, 0.04, 0.2);
            glUniform3f(prog->getUniform("diffuseColor"), 0.0, 0.16, 0.9);
            glUniform3f(prog->getUniform("specColor"), 0.14, 0.2, 0.8);
            glUniform1f(prog->getUniform("specShine"), 120.0);
            break;
        case 1: // flat grey
            glUniform3f(prog->getUniform("ambColor"), 0.13, 0.13, 0.14);
            glUniform3f(prog->getUniform("diffuseColor"), 0.3, 0.3, 0.4);
            glUniform3f(prog->getUniform("specColor"), 0.3, 0.3, 0.4);
            glUniform1f(prog->getUniform("specShine"), 4.0);
            break;
        case 2: //brass
            glUniform3f(prog->getUniform("ambColor"), 0.3294, 0.2235, 0.02745);
            glUniform3f(prog->getUniform("diffuseColor"), 0.7804, 0.5686, 0.11373);
            glUniform3f(prog->getUniform("specColor"), 0.9922, 0.941176, 0.80784);
            glUniform1f(prog->getUniform("specShine"), 27.9);
            break;
        case 3: //copper
            glUniform3f(prog->getUniform("ambColor"), 0.1913, 0.0735, 0.0225);
            glUniform3f(prog->getUniform("diffuseColor"), 0.7038, 0.27048, 0.0828);
            glUniform3f(prog->getUniform("specColor"), 0.257, 0.1376, 0.08601);
            glUniform1f(prog->getUniform("specShine"), 12.8);
            break;
        case 4: //emerald
            glUniform3f(prog->getUniform("ambColor"), 0.0215, 0.1745, 0.0215);
            glUniform3f(prog->getUniform("diffuseColor"), 0.07568, 0.61424, 0.07568);
            glUniform3f(prog->getUniform("specColor"), 0.633, 0.727811, 0.633);
            glUniform1f(prog->getUniform("specShine"), 76.8);
            break;
    }
}
