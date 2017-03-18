#ifndef _H_PARTICLESYSTEM_
#define _H_PARTICLESYSTEM_

#include "Particle.h"
#include "Program.h"
#include "Texture.h"
#include "stuff.h"
#include "MatrixStack.h"

using namespace std;

class ParticleSystem {
    public:
        ParticleSystem(bool dies, vec4 startColor, vec3 startPos);
        ~ParticleSystem(){};
        void update(float deltaTime);
        void render(shared_ptr<Program> prog);
        const bool isEmpty() const { return alivePart == 0 && !isInfinite; };
        void initGeom();
        void updateGeom();
    private:
        vec3 origin;
        vec4 baseColor;
        bool isInfinite;
        vector<Particle> particles;
        int partNum = 100;
        int alivePart = 0;
        GLfloat points[300];
        GLfloat pointColors[400];
        GLuint pointsbuffer;
        GLuint colorbuffer;
        GLuint VertexArrayID;
};

class ParticleManager {
    public:
        ParticleManager();
        ~ParticleManager(){};
        void update(float deltaTime);
        void addParticleSystem(bool dies, vec4 startColor, vec3 startPos);
        void init(string vert, string frag);
        void render(tmat4x4<float> MV, tmat4x4<float> P);
    private:
        vector<ParticleSystem> systems;
        shared_ptr<Program> partProg;
        shared_ptr<Texture> tex;
};

#endif