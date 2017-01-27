//
//  GameState.h
//  ProjM1
//
//  Created by Andrew Adriance on 1/26/17.
//
//

#ifndef GameState_h
#define GameState_h

#include <GLFW/glfw3.h>

class GameState {
public:
    GameState();
    float worldTime = 0; //Current time in the world total
    float deltaTime = 0; //Time since last update
    int waveNumber = 0;  //How many waves you've survived
    float mouseX = 0;    //Current position of mouse (X)
    float mouseY = 0;    //Current position of mouse (Y)
    GLFWwindow *window = NULL;
    //use glfwGetKey (GLFWwindow *window, int key) to check key input
};

#endif /* GameState_h */
