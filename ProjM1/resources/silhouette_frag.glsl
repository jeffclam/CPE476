#version 330 core

uniform vec3 silhouette_color;

out vec4 color;

void main(void) {
	color = vec4(silhouette_color, 1.0);
}