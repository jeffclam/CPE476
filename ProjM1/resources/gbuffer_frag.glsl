#version 330 core

#define VARIATION 5

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

uniform sampler2D tex;

in vec2 vTexCoord;
in vec3 fragNor;
in vec4 worldPos;

uniform float specShine;

void main()
{
  vec4 texColor = texture(tex, vTexCoord);
  vec3 normal = normalize(fragNor);

  // Store the fragment position vector in the first gbuffer texture
  gPosition = worldPos.xyz;
  // Also store the per-fragment normals into the gbuffer
  gNormal = normalize(fragNor);
  // And the diffuse per-fragment color
  gAlbedoSpec.rgb = texColor.rgb;
  // Store specular intensity in gAlbedoSpec's alpha component
  gAlbedoSpec.a = specShine;
}