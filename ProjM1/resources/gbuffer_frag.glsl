#version 330 core

#define VARIATION 5

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

uniform sampler2D tex;
uniform sampler2D shadowDepth;


in vec2 vTexCoord;
in vec3 fragNor;
in vec4 worldPos;

in vec4 fPosLS;

uniform float specShine;

float shadowWidth = 2048.0;
//gaus blur
const float M[25]=float[25](0.003f, 0.013f, 0.022f, 0.013f, 0.003f,
                 0.013f, 0.059f, 0.097f, 0.059f, 0.013f,
                 0.022f, 0.097f, 0.159f, 0.097f, 0.022f,
                 0.013f, 0.059f, 0.097f, 0.059f, 0.013f,
                 0.003f, 0.013f, 0.022f, 0.013f, 0.003f);

void main()
{
  vec4 texColor = texture(tex, vTexCoord);
  vec3 normal = normalize(fragNor);
  vec3 view = normalize(-1 * vec3(worldPos));

  // Store the fragment position vector in the first gbuffer texture
  gPosition = worldPos.xyz;
  // Also store the per-fragment normals into the gbuffer
  gNormal = normalize(fragNor);
  // And the diffuse per-fragment color
  gAlbedoSpec.rgb = texColor.xyz;
  // Store specular intensity in gAlbedoSpec's alpha component
  gAlbedoSpec.a = specShine;
}

