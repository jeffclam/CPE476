#include "Shape.h"
#include <iostream>

#include "GLSL.h"
#include "Program.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "glm/glm.hpp"

using namespace std;
using namespace glm;

int BTN = 1;

Shape::Shape() :
	eleBufID(0),
	posBufID(0),
	norBufID(0),
	texBufID(0)
{
}

Shape::~Shape()
{
}

void Shape::loadMesh(const string &meshName)
{
	// Load geometry
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> objMaterials;
	string errStr;
	bool rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());
	if(!rc) {
		cerr << errStr << endl;
	} else {
		posBuf = shapes[0].mesh.positions;
		norBuf = shapes[0].mesh.normals;
		texBuf = shapes[0].mesh.texcoords;
		eleBuf = shapes[0].mesh.indices;
	}
}

/* Note this is fairly dorky - */
void Shape::ComputeTex() {
	float u, v;

		for (size_t n = 0; n < norBuf.size()/3; n++) {
			u = norBuf[n*3+0]/2.0 + 0.5;
			v = norBuf[n*3+1]/2.0 + 0.5;
         texBuf[n*3+0] = u;
         texBuf[n*3+1] = v;
      }
}

void Shape::ComputeTanBN() {
		int idx0, idx1, idx2;
		vec3 v0, v1, v2;
		vec2 t0, t1, t2;
		vec3 e0, e1;
		vec2 texE0, texE1;
		float weight;
		vec3 Tan, biTan;

		//bootstrap for every vertex create a tangent and biTangent
		for (size_t n = 0; n < posBuf.size(); n++) {
		   	tanBuf.push_back(0);
				BNBuf.push_back(0);	
		}
		
		for (size_t n = 0; n < eleBuf.size()/3; n++) {
			idx0 = eleBuf[n*3];
			idx1 = eleBuf[n*3 +1];
			idx2 = eleBuf[n*3 +2];
			v0 = vec3(posBuf[idx0*3+0], posBuf[idx0*3+1], posBuf[idx0*3+2]);
			v1 = vec3(posBuf[idx1*3+0], posBuf[idx1*3+1], posBuf[idx1*3+2]);
			v2 = vec3(posBuf[idx2*3+0], posBuf[idx2*3+1], posBuf[idx2*3+2]);
			t0 = vec2(texBuf[idx0*2+0], texBuf[idx0*2+1]); 
			t1 = vec2(texBuf[idx1*2+0], texBuf[idx1*2+1]); 
			t2 = vec2(texBuf[idx2*2+0], texBuf[idx2*2+1]); 
			e0 = v1 - v0;
			e1 = v2 - v0;
			texE0 = t1 - t0;
			texE1 = t2 - t0;
			weight = 1.0f/ (texE0.x*texE1.y - texE0.y*texE1.x);
			Tan = (e0*texE1.y - e1*texE0.y)*weight;
			biTan = (e1*texE0.x - e0*texE1.x)*weight;
			//set the tangent and biTangent for each vertex
		   	tanBuf[idx0*3] = Tan.x;
		   	tanBuf[idx0*3 +1] = Tan.y;
		   	tanBuf[idx0*3 +2] = Tan.z;
		   	BNBuf[idx0*3] = biTan.x;
		   	BNBuf[idx0*3 +1] = biTan.y;
		   	BNBuf[idx0*3 +2] = biTan.z;
		   	tanBuf[idx1*3] = Tan.x;
		   	tanBuf[idx1*3 +1] = Tan.y;
		   	tanBuf[idx1*3 +2] = Tan.z;
		   	BNBuf[idx1*3] = biTan.x;
		   	BNBuf[idx1*3 +1] = biTan.y;
		   	BNBuf[idx1*3 +2] = biTan.z;
		   	tanBuf[idx2*3] = Tan.x;
		   	tanBuf[idx2*3 +1] = Tan.y;
		   	tanBuf[idx2*3 +2] = Tan.z;
		   	BNBuf[idx2*3] = biTan.x;
		   	BNBuf[idx2*3 +1] = biTan.y;
		   	BNBuf[idx2*3 +2] = biTan.z;
		}

}

void Shape::resize() {
  float minX, minY, minZ;
   float maxX, maxY, maxZ;
   float scaleX, scaleY, scaleZ;
   float shiftX, shiftY, shiftZ;
   float epsilon = 0.001;

   minX = minY = minZ = 1.1754E+38F;
   maxX = maxY = maxZ = -1.1754E+38F;

   //Go through all vertices to determine min and max of each dimension
   for (size_t v = 0; v < posBuf.size() / 3; v++) {
      if(posBuf[3*v+0] < minX) minX = posBuf[3*v+0];
      if(posBuf[3*v+0] > maxX) maxX = posBuf[3*v+0];

      if(posBuf[3*v+1] < minY) minY = posBuf[3*v+1];
      if(posBuf[3*v+1] > maxY) maxY = posBuf[3*v+1];

      if(posBuf[3*v+2] < minZ) minZ = posBuf[3*v+2];
      if(posBuf[3*v+2] > maxZ) maxZ = posBuf[3*v+2];
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
   for (size_t v = 0; v < posBuf.size() / 3; v++) {
      posBuf[3*v+0] = (posBuf[3*v+0] - shiftX) * scaleX;
      assert(posBuf[3*v+0] >= -1.0 - epsilon);
      assert(posBuf[3*v+0] <= 1.0 + epsilon);
      posBuf[3*v+1] = (posBuf[3*v+1] - shiftY) * scaleY;
      assert(posBuf[3*v+1] >= -1.0 - epsilon);
      assert(posBuf[3*v+1] <= 1.0 + epsilon);
      posBuf[3*v+2] = (posBuf[3*v+2] - shiftZ) * scaleZ;
      assert(posBuf[3*v+2] >= -1.0 - epsilon);
      assert(posBuf[3*v+2] <= 1.0 + epsilon);
   }
}
void Shape::init()
{
	// Send the position array to the GPU
	glGenBuffers(1, &posBufID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	
	// Send the normal array to the GPU
	if(norBuf.empty()) {
		norBufID = 0;
	} else {
		glGenBuffers(1, &norBufID);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	}
	
	// Send the texture array to the GPU
	if(texBuf.empty()) {
		//texBufID = 0;
		//send in spherical constructed
		for (size_t v = 0; v < posBuf.size(); v++) {
         texBuf.push_back(0);
      }
      ComputeTex();

		glGenBuffers(1, &texBufID);
		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
		glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW);
	} else {
		glGenBuffers(1, &texBufID);
		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
		glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW);
	}
	
	if (BTN) {
		ComputeTanBN();
		glGenBuffers(1, &tanBufID);
		glBindBuffer(GL_ARRAY_BUFFER, tanBufID);
		glBufferData(GL_ARRAY_BUFFER, tanBuf.size()*sizeof(float), &tanBuf[0], GL_STATIC_DRAW);
		glGenBuffers(1, &BNBufID);
		glBindBuffer(GL_ARRAY_BUFFER, BNBufID);
		glBufferData(GL_ARRAY_BUFFER, BNBuf.size()*sizeof(float), &BNBuf[0], GL_STATIC_DRAW);
	}
	
	// Send the element array to the GPU
	glGenBuffers(1, &eleBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW);
	
	// Unbind the arrays
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	assert(glGetError() == GL_NO_ERROR);
}

void Shape::draw(const shared_ptr<Program> prog) const
{
	// Bind position buffer
	int h_pos = prog->getAttribute("vertPos");
	GLSL::enableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	
	// Bind normal buffer
	int h_nor = prog->getAttribute("vertNor");
	if(h_nor != -1 && norBufID != 0) {
		GLSL::enableVertexAttribArray(h_nor);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	}
	
	// Bind texcoords buffer
	int h_tex = prog->getAttribute("vertTex");
	if(h_tex != -1 && texBufID != 0) {
		GLSL::enableVertexAttribArray(h_tex);
		glBindBuffer(GL_ARRAY_BUFFER, texBufID);
		glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	}
	int h_tan = -1, h_bn = -1;

	if (BTN) {
	  h_tan = prog->getAttribute("vertTan");
	  if(h_tan != -1 && tanBufID != 0) {
		  GLSL::enableVertexAttribArray(h_tan);
		  glBindBuffer(GL_ARRAY_BUFFER, tanBufID);
		  glVertexAttribPointer(h_tan, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	  }
	  h_bn = prog->getAttribute("vertBN");
	  if(h_bn != -1 && BNBufID != 0) {
		  GLSL::enableVertexAttribArray(h_bn);
		  glBindBuffer(GL_ARRAY_BUFFER, BNBufID);
		  glVertexAttribPointer(h_bn, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	  }
	}
	
	// Bind element buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID);
	
	// Draw
	glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0);
	
	// Disable and unbind
	if(h_tex != -1) {
		GLSL::disableVertexAttribArray(h_tex);
	}
	if(h_nor != -1) {
		GLSL::disableVertexAttribArray(h_nor);
	}
	if(h_tan != -1) {
		GLSL::disableVertexAttribArray(h_tan);
		GLSL::disableVertexAttribArray(h_bn);
	}
	GLSL::disableVertexAttribArray(h_pos);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
