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
    float timeSinceSpawn = 0;
    int waveNumber = 0;  //How many waves you've survived
    double mouseX = 0;    //Current position of mouse (X)
    double mouseY = 0;    //Current position of mouse (Y)
    float timeBetweenSpawn = 5;
    int enemyCount = 0;
    int score = 0;
    int grassAlive = 0;
    GLFWwindow *window = NULL;
    //use glfwGetKey (GLFWwindow *window, int key) to check key input
private:

};

#endif /* GameState_h */
