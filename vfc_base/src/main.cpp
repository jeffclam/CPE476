// View Frustum Culling base code for CPE 476 VFC workshop
// built off 471 P4 game camera - 2015 revise with glfw and obj and glm - ZJW
// Note data-structure NOT recommended for CPE 476 - 
// object locations in arrays and estimated radii
// use your improved data structures
// note shaders using GLSL 1.2 (just did not bother to update)

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <cmath>
#include <stdio.h>
#include "GLSL.h"
#include "tiny_obj_loader.h"
#include <glm/gtc/matrix_transform.hpp> //perspective, trans etc
#include <glm/gtc/type_ptr.hpp> //value_ptr

using namespace std;
using namespace glm;

string RESOURCE_DIR = ""; // Where the resources are loaded from

GLFWwindow* window;

//main geometry for program
vector<tinyobj::shape_t> nefer;
vector<tinyobj::material_t> materials;
vector<tinyobj::shape_t> sphere;

//global used to control culling or not for sub-window views
int CULL = 1;

int g_width;
int g_height;
glm::vec3 g_light(2, 6, 6);
float updateDir = 0.5;

//camera control - you can ignore - what matters is eye location and view matrix
double g_phi, g_theta;
float g_Camtrans = -2.5;
vec3 view = vec3(0, 0, 1);
vec3 strafe = vec3(1, 0, 0);
vec3 g_eye = vec3(0, 1, 0);
vec3 g_lookAt = vec3(0, 1, -1);

//transforms on objects - ugly as its frankenstein of two example codes
glm::vec3 g_transN[10];
float g_scaleN[10];
float g_rotN[10];
vec3 g_transS[10];
float g_scaleS[10];
float g_rotS[10];
int g_mat_ids[10];
float g_ang[10];

GLuint ShadeProg;
GLuint posBufObjB = 0;
GLuint norBufObjB = 0;
GLuint indBufObjB = 0;

GLuint posBufObjS = 0;
GLuint norBufObjS = 0;
GLuint indBufObjS = 0;

GLuint posBufObjG = 0;
GLuint norBufObjG = 0;

//Handles to the shader data
GLint h_aPosition;
GLint h_aNormal;
GLint h_uModelMatrix;
GLint h_uViewMatrix;
GLint h_uProjMatrix;
GLint h_uLightPos;
GLint h_uMatAmb, h_uMatDif, h_uMatSpec, h_uMatShine;

int printOglError (const char *file, int line) {
   /* Returns 1 if an OpenGL error occurred, 0 otherwise. */
   GLenum glErr;
   int    retCode = 0;
  
   glErr = glGetError ();
   while (glErr != GL_NO_ERROR)
    {
      printf ("glError in file %s @ line %d: %s\n", file, line, gluErrorString (glErr));
      retCode = 1;
      glErr = glGetError ();
    }
   return retCode;
}

inline void safe_glUniformMatrix4fv(const GLint handle, const GLfloat data[]) {
  if (handle >= 0)
    glUniformMatrix4fv(handle, 1, GL_FALSE, data);
}

/* helper function to change material attributes */
void SetMaterial(int i) {
 
  glUseProgram(ShadeProg);
  switch (i) {
    case 0: //shiny blue plastic
        glUniform3f(h_uMatAmb, 0.02, 0.04, 0.2);
        glUniform3f(h_uMatDif, 0.0, 0.16, 0.9);
        glUniform3f(h_uMatSpec, 0.14, 0.2, 0.8);
        glUniform1f(h_uMatShine, 120.0);
        break;
    case 1: // flat grey
      glUniform3f(h_uMatAmb, 0.13, 0.13, 0.14);
      glUniform3f(h_uMatDif, 0.3, 0.3, 0.4);
      glUniform3f(h_uMatSpec, 0.3, 0.3, 0.4);
      glUniform1f(h_uMatShine, 4.0);
      break;
    case 2: //brass
      glUniform3f(h_uMatAmb, 0.3294, 0.2235, 0.02745);
      glUniform3f(h_uMatDif, 0.7804, 0.5686, 0.11373);
      glUniform3f(h_uMatSpec, 0.9922, 0.941176, 0.80784);
      glUniform1f(h_uMatShine, 27.9);
        break;
    case 3: //copper
      glUniform3f(h_uMatAmb, 0.1913, 0.0735, 0.0225);
      glUniform3f(h_uMatDif, 0.7038, 0.27048, 0.0828);
      glUniform3f(h_uMatSpec, 0.257, 0.1376, 0.08601);
      glUniform1f(h_uMatShine, 12.8);
        break;
    case 4: // flat grey
        glUniform3f(h_uMatAmb, 0.13, 0.13, 0.14);
        glUniform3f(h_uMatDif, 0.3, 0.3, 0.4);
        glUniform3f(h_uMatSpec, 0.3, 0.3, 0.4);
        glUniform1f(h_uMatShine, 4.0);
        break;
    case 5: //shadow
        glUniform3f(h_uMatAmb, 0.12, 0.12, 0.12);
        glUniform3f(h_uMatDif, 0.0, 0.0, 0.0);
        glUniform3f(h_uMatSpec, 0.0, 0.0, 0.0);
        glUniform1f(h_uMatShine, 0);
        break;
    case 6: //gold
        glUniform3f(h_uMatAmb, 0.09, 0.07, 0.08);
        glUniform3f(h_uMatDif, 0.91, 0.2, 0.91);
        glUniform3f(h_uMatSpec, 1.0, 0.7, 1.0);
        glUniform1f(h_uMatShine, 100.0);
        break;
    case 7: //green
        glUniform3f(h_uMatAmb, 0.0, 0.07, 0.0);
        glUniform3f(h_uMatDif, 0.1, 0.91, 0.3);
        glUniform3f(h_uMatSpec, 0, 0, 0);
        glUniform1f(h_uMatShine, 0.0);
        break;
  }
}

/* projection matrix */
mat4 SetProjectionMatrix() {
  	mat4 Projection = perspective(radians(50.0f), (float)g_width/g_height, 0.1f, 100.f);
  	safe_glUniformMatrix4fv(h_uProjMatrix, glm::value_ptr(Projection));
	return Projection;
}

/* top down views using ortho */
mat4 SetOrthoMatrix() {
  	mat4 ortho = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 2.1f, 100.f);
  	safe_glUniformMatrix4fv(h_uProjMatrix, glm::value_ptr(ortho));
	return ortho;	
}

/* camera controls - this is the camera for the top down view */
mat4 SetTopView() {
  	mat4 Cam = lookAt(g_eye + vec3(0, 8, 0), g_eye, g_lookAt-g_eye);
  	safe_glUniformMatrix4fv(h_uViewMatrix, glm::value_ptr(Cam));
	return Cam;
}

/*normal game camera */
mat4 SetView() {
  	mat4 Cam = lookAt(g_eye, g_lookAt, vec3(0, 1, 0));
  	safe_glUniformMatrix4fv(h_uViewMatrix, glm::value_ptr(Cam));
	return Cam;
}

/* model transforms - these are insane because they came from p2B and P4*/
mat4 SetModel(vec3 trans, float rotY, float rotX, vec3 sc) {
  	mat4 Trans = translate( glm::mat4(1.0f), trans);
  	mat4 RotateY = rotate( glm::mat4(1.0f), rotY, glm::vec3(0.0f, 1, 0));
  	mat4 RotateX = rotate( glm::mat4(1.0f), rotX, glm::vec3(1,0 ,0));
  	mat4 Sc = scale( glm::mat4(1.0f), sc);
  	mat4 com = Trans*RotateY*Sc*RotateX;
  	safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(com));
	return com;
}

void SetModel(mat4 m) {
  	safe_glUniformMatrix4fv(h_uModelMatrix, glm::value_ptr(m));
}

//Given a vector of shapes which has already been read from an obj file
// resize all vertices to the range [-1, 1]
void resize_obj(std::vector<tinyobj::shape_t> &shapes){
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    float scaleX, scaleY, scaleZ;
    float shiftX, shiftY, shiftZ;
    float epsilon = 0.001;

    minX = minY = minZ = 1.1754E+38F;
    maxX = maxY = maxZ = -1.1754E+38F;

    //Go through all vertices to determine min and max of each dimension
    for (size_t i = 0; i < shapes.size(); i++) {
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            if(shapes[i].mesh.positions[3*v+0] < minX) minX = shapes[i].mesh.positions[3*v+0];
            if(shapes[i].mesh.positions[3*v+0] > maxX) maxX = shapes[i].mesh.positions[3*v+0];

            if(shapes[i].mesh.positions[3*v+1] < minY) minY = shapes[i].mesh.positions[3*v+1];
            if(shapes[i].mesh.positions[3*v+1] > maxY) maxY = shapes[i].mesh.positions[3*v+1];

            if(shapes[i].mesh.positions[3*v+2] < minZ) minZ = shapes[i].mesh.positions[3*v+2];
            if(shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = shapes[i].mesh.positions[3*v+2];
        }
    }
   //From min and max compute necessary scale and shift for each dimension
   float maxExtent, xExtent, yExtent, zExtent;
   xExtent = maxX-minX;
   yExtent = maxY-minY;
   zExtent = maxZ-minZ;
   if (xExtent >= yExtent && xExtent >= zExtent) {
     maxExtent = xExtent;
   }
   if (yExtent >= xExtent && yExtent >= zExtent) {
     maxExtent = yExtent;
   }
   if (zExtent >= xExtent && zExtent >= yExtent) {
     maxExtent = zExtent;
   }
    scaleX = 2.0 /maxExtent;
    shiftX = minX + (xExtent/ 2.0);
    scaleY = 2.0 / maxExtent;
    shiftY = minY + (yExtent / 2.0);
    scaleZ = 2.0/ maxExtent;
    shiftZ = minZ + (zExtent)/2.0;

    //Go through all verticies shift and scale them
    for (size_t i = 0; i < shapes.size(); i++) {
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            shapes[i].mesh.positions[3*v+0] = (shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
            assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
            shapes[i].mesh.positions[3*v+1] = (shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
            assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
            shapes[i].mesh.positions[3*v+2] = (shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
            assert(shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
        }
    }
}

/* draw a snowman */
void drawSnowman(mat4 moveModel, int i) {

   // Enable and bind position array for drawing
   GLSL::enableVertexAttribArray(h_aPosition);
   glBindBuffer(GL_ARRAY_BUFFER, posBufObjS);
   glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // Enable and bind normal array for drawing
   GLSL::enableVertexAttribArray(h_aNormal);
   glBindBuffer(GL_ARRAY_BUFFER, norBufObjS);
   glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

   // Bind index array for drawing
   int nIndices = (int)sphere[0].mesh.indices.size();
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufObjS);

   SetMaterial(5);
	//shadow
   mat4 t = translate(mat4(1.0), vec3(0.2, -1.4, 0.2));
   mat4 s = scale(mat4(1.0), vec3(1, 0.01, 1));
   SetModel(moveModel*t*s);
   glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
   
	if (i%2 ==0)
      SetMaterial(0);
   else
      SetMaterial(1);
	//body?
   t = translate(mat4(1.0), vec3(0, -0.5, 0));
   SetModel(moveModel*t);
   glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
  
   t = translate(mat4(1.0), vec3(0., 0.72, 0));
   s = scale(mat4(1.0), vec3(.75, .75, .75));
   mat4 com = t*s;
   SetModel(moveModel*com);
   glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

   t = translate(mat4(1.0), vec3(0, 1.75, 0));
   s = scale(mat4(1.0), vec3(0.55, 0.55, 0.55));
   com = t*s;
   SetModel(moveModel*com);
   glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

   //switch the shading to greyscale
   SetMaterial(4);
   //the right arm
   t = translate(mat4(1.0), vec3( .37, 0.75, .5));
   mat4 r = rotate(mat4(1.0), radians(g_ang[i]), vec3(0, 0, 1));
   mat4 t1 = translate(mat4(1.0), vec3( .37, 0.0, .0));
   s = scale(mat4(1.0), vec3(0.75, 0.05, 0.05));
   com =t*r*t1*s; 
   SetModel(moveModel*com);
   glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

   //update animation on arm
   if (g_ang[i] > 18)
      updateDir = -0.5;
   else if (g_ang[i] < -20)
      updateDir = 0.5;
   g_ang[i] += updateDir;

   //the left arm
   t = translate(mat4(1.0), vec3( -.75, 0.75, .5));
   s = scale(mat4(1.0), vec3( 0.75, 0.05, 0.05));
   com = t*s;
   SetModel(moveModel*com);
   glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

   //eyes
   t = translate(mat4(1.0), vec3( -.35, 1.75, .38));
   s = scale(mat4(1.0), vec3(  0.05, 0.05, 0.05));
   com = t*s;
   SetModel(moveModel*com);
   glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

   t = translate(mat4(1.0), vec3( .35, 1.75, .38));
   s = scale(mat4(1.0), vec3( 0.05, 0.05, 0.05));
   com = t*s;
   SetModel(moveModel*com);
   glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

   GLSL::disableVertexAttribArray(h_aPosition);
   GLSL::disableVertexAttribArray(h_aNormal);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


void loadShapes(const string &objFile, std::vector<tinyobj::shape_t>& shapes)
{
	string err = tinyobj::LoadObj(shapes, materials, objFile.c_str());
	if(!err.empty()) {
		cerr << err << endl;
	}
   resize_obj(shapes);
}


/*code to set up Nefertiti mesh */
void initNefer(std::vector<tinyobj::shape_t>& shape) {

	// Send the position array to the GPU
	const vector<float> &posBuf = shape[0].mesh.positions;
	glGenBuffers(1, &posBufObjB);
	glBindBuffer(GL_ARRAY_BUFFER, posBufObjB);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	
	// Send the normal array to the GPU
	vector<float> norBuf;
   glm::vec3 v1, v2, v3;
   glm::vec3 edge1, edge2, norm;
   int idx1, idx2, idx3;
   //for every vertex initialize the vertex normal to 0
   for (int j = 0; j < shape[0].mesh.positions.size()/3; j++) {
      norBuf.push_back(0);
      norBuf.push_back(0);
      norBuf.push_back(0);
   }
   //process the mesh and compute the normals - for every face
   //add its normal to its associated vertex
   for (int i = 0; i < shape[0].mesh.indices.size()/3; i++) {
      idx1 = shape[0].mesh.indices[3*i+0];
      idx2 = shape[0].mesh.indices[3*i+1];
      idx3 = shape[0].mesh.indices[3*i+2];
      v1 = glm::vec3(shape[0].mesh.positions[3*idx1 +0],shape[0].mesh.positions[3*idx1 +1], shape[0].mesh.positions[3*idx1 +2]);
      v2 = glm::vec3(shape[0].mesh.positions[3*idx2 +0],shape[0].mesh.positions[3*idx2 +1], shape[0].mesh.positions[3*idx2 +2]);
      v3 = glm::vec3(shape[0].mesh.positions[3*idx3 +0],shape[0].mesh.positions[3*idx3 +1], shape[0].mesh.positions[3*idx3 +2]);
      if (0) {
         std:: cout << "v1 " << v1.x << " " << v1.y << " " << v1.z << std::endl;
         std:: cout << "v2 " << v1.x << " " << v2.y << " " << v2.z << std::endl;
         std:: cout << "v3 " << v3.x << " " << v3.y << " " << v3.z << std::endl;
      }
      edge1 = v2 - v1;
      edge2 = v3 - v1;
      norm = glm::cross(edge1, edge2);
  norBuf[3*idx1+0] += (norm.x);
      norBuf[3*idx1+1] += (norm.y);
      norBuf[3*idx1+2] += (norm.z);
      norBuf[3*idx2+0] += (norm.x);
      norBuf[3*idx2+1] += (norm.y);
      norBuf[3*idx2+2] += (norm.z);
      norBuf[3*idx3+0] += (norm.x);
      norBuf[3*idx3+1] += (norm.y);
      norBuf[3*idx3+2] += (norm.z);
   }
	glGenBuffers(1, &norBufObjB);
	glBindBuffer(GL_ARRAY_BUFFER, norBufObjB);
	glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	
	// Send the index array to the GPU
	const vector<unsigned int> &indBuf = shape[0].mesh.indices;
	glGenBuffers(1, &indBufObjB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufObjB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size()*sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);

	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLSL::checkVersion();
	assert(glGetError() == GL_NO_ERROR);
}

/*send snowman data to GPU */
void initSnow(std::vector<tinyobj::shape_t>& shape) {

	// Send the position array to the GPU
	const vector<float> &posBuf = shape[0].mesh.positions;
	glGenBuffers(1, &posBufObjS);
	glBindBuffer(GL_ARRAY_BUFFER, posBufObjS);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	
	// Send the normal array to the GPU
	vector<float> norBuf = shape[0].mesh.normals;;
	glGenBuffers(1, &norBufObjS);
	glBindBuffer(GL_ARRAY_BUFFER, norBufObjS);
	glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	
	// Send the index array to the GPU
	const vector<unsigned int> &indBuf = shape[0].mesh.indices;
	glGenBuffers(1, &indBufObjS);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufObjS);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size()*sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);

	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLSL::checkVersion();
	assert(glGetError() == GL_NO_ERROR);
}

/* ground plane data to GPU */
void initGround() {

   float G_edge = 20;
  GLfloat g_backgnd_data[] = {
      -G_edge, -1.0f, -G_edge,
        -G_edge,  -1.0f, G_edge,
        G_edge, -1.0f, -G_edge,
        -G_edge,  -1.0f, G_edge,
        G_edge, -1.0f, -G_edge,
        G_edge, -1.0f, G_edge,
  };


  GLfloat nor_Buf_G[] = { 
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
  };

   glGenBuffers(1, &posBufObjG);
   glBindBuffer(GL_ARRAY_BUFFER, posBufObjG);
   glBufferData(GL_ARRAY_BUFFER, sizeof(g_backgnd_data), g_backgnd_data, GL_STATIC_DRAW);
	
   glGenBuffers(1, &norBufObjG);
	glBindBuffer(GL_ARRAY_BUFFER, norBufObjG);
	glBufferData(GL_ARRAY_BUFFER, sizeof(nor_Buf_G), nor_Buf_G, GL_STATIC_DRAW);

}

void initGL()
{
	// Set the background color
	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
	// Enable Z-buffer test
	glEnable(GL_DEPTH_TEST);
   glPointSize(18);

   float tx, tz, s, r;	
   float Wscale = 18.0;
	srand(time(NULL));
  //allocate the transforms for the different models
  for (int i=0; i < 10; i++) {
      tx = 0.2 + Wscale*(static_cast <float> (rand()) / static_cast <float> (RAND_MAX))-Wscale/1.0;
      tz = 0.2 + Wscale*(static_cast <float> (rand()) / static_cast <float> (RAND_MAX))-Wscale/1.0;
      s = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
      r = 360*(static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
      g_transN[i] = vec3(tx, 0, tz);
      g_scaleN[i] = 1.0;
      g_rotN[i] = r;
      tx = 0.1 + Wscale*(static_cast <float> (rand()) / static_cast <float> (RAND_MAX))-Wscale/2.0;
      tz = 0.1 + Wscale*(static_cast <float> (rand()) / static_cast <float> (RAND_MAX))-Wscale/2.0;
      s = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
      r = 360*(static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
      g_transS[i] = vec3(tx, 0, tz);
      g_scaleS[i] = 1.0;
      g_rotS[i] = r;
      g_mat_ids[i] = i%4;
      g_ang[i] = 0;
  }

   g_phi = 0;
   g_theta = -90;
   initNefer(nefer);
   initSnow(sphere);
   initGround();
}

bool installShaders(const string &vShaderName, const string &fShaderName)
{
	GLint rc;
	
	// Create shader handles
	GLuint VS = glCreateShader(GL_VERTEX_SHADER);
	GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);
	
	// Read shader sources
	const char *vshader = GLSL::textFileRead(vShaderName.c_str());
	const char *fshader = GLSL::textFileRead(fShaderName.c_str());
	glShaderSource(VS, 1, &vshader, NULL);
	glShaderSource(FS, 1, &fshader, NULL);
	
	// Compile vertex shader
	glCompileShader(VS);
   std::cout << "just compiled the v shader" << std::endl;
   printOglError(__FILE__, __LINE__);
	GLSL::printError();
	glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
	GLSL::printShaderInfoLog(VS);
	if(!rc) {
		printf("Error compiling vertex shader %s\n", vShaderName.c_str());
		return false;
	}
	
	// Compile fragment shader
	glCompileShader(FS);
   std::cout << "just compiled the f shader" << std::endl;
	GLSL::printError();
	glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
	GLSL::printShaderInfoLog(FS);
	if(!rc) {
		printf("Error compiling fragment shader %s\n", fShaderName.c_str());
		return false;
	}
	
	// Create the program and link
	   ShadeProg = glCreateProgram();
	   glAttachShader(ShadeProg, VS);
	   glAttachShader(ShadeProg, FS);
	   glLinkProgram(ShadeProg);
      std::cout << "just linked the shaders" << std::endl;
   
	   GLSL::printError();
	   glGetProgramiv(ShadeProg, GL_LINK_STATUS, &rc);
	   GLSL::printProgramInfoLog(ShadeProg);
	   if(!rc) {
		   printf("Error linking shaders %s and %s\n", vShaderName.c_str(), fShaderName.c_str());
		   return false;
	   }

   /* get handles to attribute data */
    h_aPosition = GLSL::getAttribLocation(ShadeProg, "aPosition");
    h_aNormal = GLSL::getAttribLocation(ShadeProg, "aNormal");
    h_uProjMatrix = GLSL::getUniformLocation(ShadeProg, "uProjMatrix");
    h_uViewMatrix = GLSL::getUniformLocation(ShadeProg, "uViewMatrix");
    h_uModelMatrix = GLSL::getUniformLocation(ShadeProg, "uModelMatrix");
    h_uLightPos = GLSL::getUniformLocation(ShadeProg, "uLightPos");
    h_uMatAmb = GLSL::getUniformLocation(ShadeProg, "UaColor");
    h_uMatDif = GLSL::getUniformLocation(ShadeProg, "UdColor");
    h_uMatSpec = GLSL::getUniformLocation(ShadeProg, "UsColor");
    h_uMatShine = GLSL::getUniformLocation(ShadeProg, "Ushine");
	
	assert(glGetError() == GL_NO_ERROR);
	return true;
}


/* VFC code starts here TODO - start here and fill in these functions!!!*/
vec4 Left, Right, Bottom, Top, Near, Far;
vec4 planes[6];

void ExtractVFPlanes(mat4 P, mat4 V) {

	/* composite matrix */
    //mat 4 is [col][row] like other GPU stuff
	mat4 comp = P*V;
	vec3 n; //use to pull out normal
	float l; //length of normal for plane normalization

	Left.x = comp[0][3] + comp[0][0]; // see handout to fill in with values from comp
	Left.y = comp[1][3] + comp[1][0]; // see handout to fill in with values from comp
	Left.z = comp[2][3] + comp[2][0]; // see handout to fill in with values from comp
	Left.w = comp[3][3] + comp[3][0]; // see handout to fill in with values from comp
    n = vec3(Left.x, Left.y, Left.z);
    l = length(n);
	planes[0] = Left = Left/l;
	cout << "Left' " << Left.x << " " << Left.y << " " <<Left.z << " " << Left.w << endl;
	
	Right.x = comp[0][3] - comp[0][0]; // see handout to fill in with values from comp
	Right.y = comp[1][3] - comp[1][0]; // see handout to fill in with values from comp
	Right.z = comp[2][3] - comp[2][0]; // see handout to fill in with values from comp
	Right.w = comp[3][3] - comp[3][0]; // see handout to fill in with values from comp
    n = vec3(Right.x, Right.y, Right.z);
    l = length(n);
	planes[1] = Right = Right/l;
	cout << "Right " << Right.x << " " << Right.y << " " <<Right.z << " " << Right.w << endl;

	Bottom.x = comp[0][3] + comp[0][1]; // see handout to fill in with values from comp
	Bottom.y = comp[1][3] + comp[1][1]; // see handout to fill in with values from comp
	Bottom.z = comp[2][3] + comp[2][1]; // see handout to fill in with values from comp
	Bottom.w = comp[3][3] + comp[3][1]; // see handout to fill in with values from comp
    n = vec3(Bottom.x, Bottom.y, Bottom.z);
    l = length(n);
	planes[2] = Bottom = Bottom/l;
	cout << "Bottom " << Bottom.x << " " << Bottom.y << " " <<Bottom.z << " " << Bottom.w << endl;
	
	Top.x = comp[0][3] - comp[0][1]; // see handout to fill in with values from comp
	Top.y = comp[1][3] - comp[1][1]; // see handout to fill in with values from comp
	Top.z = comp[2][3] - comp[2][1]; // see handout to fill in with values from comp
	Top.w = comp[3][3] - comp[3][1]; // see handout to fill in with values from comp
    n = vec3(Top.x, Top.y, Top.z);
    l = length(n);
	planes[3] = Top  = Top/l;
	cout << "Top " << Top.x << " " << Top.y << " " <<Top.z << " " << Top.w << endl;

	Near.x = comp[0][3] + comp[0][2]; // see handout to fill in with values from comp
	Near.y = comp[1][3] + comp[1][2]; // see handout to fill in with values from comp
	Near.z = comp[2][3] + comp[2][2]; // see handout to fill in with values from comp
	Near.w = comp[3][3] + comp[3][2]; // see handout to fill in with values from comp
    n = vec3(Near.x, Near.y, Near.z);
    l = length(n);
	planes[4] = Near = Near/l;
	cout << "Near " << Near.x << " " << Near.y << " " <<Near.z << " " << Near.w << endl;

	Far.x = comp[0][3] - comp[0][2]; // see handout to fill in with values from comp
	Far.y = comp[1][3] - comp[1][2]; // see handout to fill in with values from comp
	Far.z = comp[2][3] - comp[2][2]; // see handout to fill in with values from comp
	Far.w = comp[3][3] - comp[3][2]; // see handout to fill in with values from comp
    n = vec3(Far.x, Far.y, Far.z);
    l = length(n);
	planes[5] = Far = Far/l;
	cout << "Far " << Far.x << " " << Far.y << " " <<Far.z << " " << Far.w << endl;
}


/* helper function to compute distance to the plane */
/* TODO: fill in */
float DistToPlane(float A, float B, float C, float D, vec3 point) {
	return (A*point.x + B*point.y + C*point.z + D);
}

/* Actual cull on planes */
//returns 1 to CULL
int ViewFrustCull(vec3 center, float radius) {

	float dist;

	if (CULL) {
		cout << "testing against all planes" << endl;
		for (int i=0; i < 6; i++) {
			dist = DistToPlane(planes[i].x, planes[i].y, planes[i].z, planes[i].w, center);
			if(dist < radius) {
                return 1;
            }


		}
		return 0;	
	} else {
		return 0;
	}
}


/* code to draw the scene */
void drawScene(float PmatID) {

	int nIndices;

   for (int i=0; i < 10; i++) {

		if( !ViewFrustCull(g_transN[i], -1.25)) {
      	//draw the mesh
	   	// Enable and bind position array for drawing
	   	GLSL::enableVertexAttribArray(h_aPosition);
	   	glBindBuffer(GL_ARRAY_BUFFER, posBufObjB);
	   	glVertexAttribPointer(h_aPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	   	// Enable and bind normal array for drawing
	   	GLSL::enableVertexAttribArray(h_aNormal);
	   	glBindBuffer(GL_ARRAY_BUFFER, norBufObjB);
	   	glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
	   	// Bind index array for drawing
	   	nIndices = (int)nefer[0].mesh.indices.size();
	   	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBufObjB);
    		//set the color 
			if (i%2 ==0) { 
		  		SetMaterial(2);
			} else {
		  		SetMaterial(3);
			}
      	SetModel(g_transN[i], radians(g_rotN[i]), radians(-90.0f), vec3(1));
		
      	//draw the mesh	
	   	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);
			//draw the shadow
      	SetMaterial(5);
      	SetModel(vec3(g_transN[i].x+0.2, g_transN[i].y-1, g_transN[i].z+0.2), radians(g_rotN[i]), radians(-90.0f), vec3(1, .01, 1));
	   	glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

      	GLSL::disableVertexAttribArray(h_aPosition);
	   	GLSL::disableVertexAttribArray(h_aNormal);
	   	glBindBuffer(GL_ARRAY_BUFFER, 0);
	   	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		//now test the snowmen
		if( !ViewFrustCull(g_transS[i], -1.5)) {
      	/*now draw the snowmen */
      	mat4 Trans = translate( mat4(1.0f), vec3(g_transS[i].x, g_transS[i].y+0.4, g_transS[i].z));
      	mat4 RotateY = rotate( mat4(1.0f), radians(g_rotS[i]), glm::vec3(0.0f, 1, 0));
      	mat4 Sc = scale( glm::mat4(1.0f), vec3(g_scaleS[i]));
      	mat4 com = Trans*RotateY*Sc;
      	drawSnowman(com, i);
		}
   }	
  
   //always draw the ground
   SetMaterial(PmatID);
   SetModel(vec3(0), radians(0.0f), radians(0.0f), vec3(1));

   glEnableVertexAttribArray(h_aPosition);
   glBindBuffer(GL_ARRAY_BUFFER, posBufObjG);
   glVertexAttribPointer( h_aPosition, 3,  GL_FLOAT, GL_FALSE, 0, (void*)0);
	GLSL::enableVertexAttribArray(h_aNormal);
	glBindBuffer(GL_ARRAY_BUFFER, norBufObjG);
	glVertexAttribPointer(h_aNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

   glDrawArrays(GL_TRIANGLES, 0, 6);

   GLSL::disableVertexAttribArray(h_aPosition);
	GLSL::disableVertexAttribArray(h_aNormal);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}	

void drawGL() {
	
  // Get current frame buffer size.
   int width, height;
   glfwGetFramebufferSize(window, &width, &height);
   glViewport(0, 0, width, height);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Use our GLSL program
	glUseProgram(ShadeProg);
   glUniform3f(h_uLightPos, g_light.x, g_light.y, g_light.z);

	//draw the scene from the game camera with culling enabled
   mat4 P = SetProjectionMatrix();
   mat4 V = SetView();
	ExtractVFPlanes(P, V); 
	CULL = 1;
	drawScene(0);

	/* draw the complete scene from a top down camera */
	glClear( GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, 300, 300);
   SetOrthoMatrix();
   SetTopView();
	CULL = 0;
	drawScene(7);
	
	/* draw the culled scene from a top down camera */
	glClear( GL_DEPTH_BUFFER_BIT);
	glViewport(0, g_height-300, 300, 300);
   SetOrthoMatrix();
   SetTopView();
	CULL = 1;
	drawScene(7);
	glUseProgram(0);
	
	assert(glGetError() == GL_NO_ERROR);

}


void window_size_callback(GLFWwindow* window, int w, int h){
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	g_width = w;
	g_height = h;
}

void mouse_callback(GLFWwindow* window, int but, int action, int mods) {

	cout << "use two finger mouse scroll" << endl;
}

/* much of the camera is here */
void scroll_callback(GLFWwindow* window, double deltaX, double deltaY) {
  vec3 diff, newV;
   //cout << "xDel + yDel " << deltaX << " " << deltaY << endl;
   g_theta += deltaX;
   g_phi += deltaY;
   newV.x = cosf(g_phi*(3.14/180.0))*cosf(g_theta*(3.14/180.0));
   newV.y = -1.0*sinf(g_phi*(3.14/180.0));
   newV.z = 1.0*cosf(g_phi*(3.14/180.0))*cosf((90.0-g_theta)*(3.14/180.0));
   diff.x = (g_lookAt.x - g_eye.x) - newV.x;
   diff.y = (g_lookAt.y - g_eye.y) - newV.y;
   diff.z = (g_lookAt.z - g_eye.z) - newV.z;
   g_lookAt.x = g_lookAt.x - diff.x;
   g_lookAt.y = g_lookAt.y - diff.y;
   g_lookAt.z = g_lookAt.z - diff.z;
   view = g_eye - g_lookAt;
   strafe = cross(vec3(0, 1,0), view);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
   float speed = 0.2;

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
      g_eye -= speed*strafe;
      g_lookAt -= speed*strafe;
      }
    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
      g_eye += speed*strafe;
      g_lookAt += speed*strafe;
      }
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
      g_eye -= speed*view;
      g_lookAt -= speed*view;
      }
    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
      g_eye += speed*view;
      g_lookAt += speed*view;
      }
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
     g_light.x += 0.25; 
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
     g_light.x -= 0.25; 
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
     g_Camtrans += 0.25; 
    if (key == GLFW_KEY_N && action == GLFW_PRESS)
     g_Camtrans -= 0.25; 
}

int main(int argc, char **argv)
{
   if(argc < 2) {
      cout << "Please specify the resource directory: e.g. prog4 ../resources" << endl;
      return 0;
   }
   RESOURCE_DIR = argv[1] + string("/");
	cout << "Resources " << RESOURCE_DIR << endl;

// Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
   glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Open a window and create its OpenGL context
   g_width = 1024;
   g_height = 768;
   window = glfwCreateWindow( g_width, g_height, "vfc with game camera", NULL, NULL);
   if( window == NULL ){
      fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
// Initialize GLEW
   if (glewInit() != GLEW_OK) {
      fprintf(stderr, "Failed to initialize GLEW\n");
      return -1;
   }

   // Ensure we can capture the escape key being pressed below
   glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

   glEnable (GL_BLEND);
   glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	loadShapes(RESOURCE_DIR + "Nefertiti-100k.obj", nefer);
	loadShapes(RESOURCE_DIR + "sphere.obj", sphere);
	initGL();
	installShaders(RESOURCE_DIR + "vert.glsl", RESOURCE_DIR + "frag.glsl");
	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);


    do{
      drawGL();

        // Swap buffers
      glfwSwapBuffers(window);
      glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
   while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0 );

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

	return 0;
}
