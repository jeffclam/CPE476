//
//  BunnyGameObj.cpp
//  Lab01
//
//  Created by Andrew Adriance on 1/20/17.
//
//

#include <stdio.h>
#include "BunnyGameObj.h"

using namespace std;

void BunnyGameObj::update(double time) {
    pos += vel * (float)time;
    printf("HELLO, IS IT ME - bun\n");
}

void BunnyGameObj::setRandomVel() {
    srand (time(NULL));
    vel[0] = rand() % 2;//40 - 20;
    vel[1] = rand() % 2;//40 - 20;
    vel[2] = rand() % 2;//40 - 20;
}
