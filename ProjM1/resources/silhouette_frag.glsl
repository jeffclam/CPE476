#version 330 core

uniform vec3 silhouette_color;

layout(location = 0) out vec4 FragColor;

void main(void) {
	FragColor = vec4(silhouette_color, 1.0);
}