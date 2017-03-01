#version  330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;

uniform mat4 LP;
uniform mat4 LV;
uniform mat4 M;

void main() {

  /* transform into light space */
  gl_Position = LP * LV * M * vec4(vertPos.xyz, 1.0);

}
