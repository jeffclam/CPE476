#version 330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;

uniform mat4 P;
uniform mat4 M;
uniform mat4 V;

uniform float silhouette_offset;

void main(void) {
	vec4 tPos = vec4(vertPos + vertNor * silhouette_offset, 1.0);
	gl_Position = P * V * M * tPos;
}