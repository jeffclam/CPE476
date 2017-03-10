#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;

out vec3 fragNor;
out vec4 worldPos;
out vec2 vTexCoord;

void main()
{
	gl_Position = P * V * M * vec4(vertPos, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(M)));
	//fragNor = (M * vec4(vertNor, 1.0)).xyz;
    fragNor = normalMatrix * vertNor;
    worldPos = M * vec4(vertPos, 1.0);
    vTexCoord = vertTex;
}