//
//  BunnyGameObj.h
//  Lab01
//
//  Created by Andrew Adriance on 1/20/17.
//
//

#ifndef BunnyGameObj_h
#define BunnyGameObj_h

#include "GameObj.h"

class BunnyGameObj : public GameObj {
public:
    BunnyGameObj(shared_ptr<Shape> s) : GameObj(s) {};
    void update(double time);
    void setRandomVel();
private:
};

#endif /* BunnyGameObj_h */
