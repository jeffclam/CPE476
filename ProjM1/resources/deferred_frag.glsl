#version 330 core
out vec4 color;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D shadowDepth;
uniform mat4 LS;

vec3 ambColor = vec3(0.1, 0.1, 0.1);
vec3 specColor = vec3(0.5, 0.5, 0.5);

#define MAX_LIGHTS 35
uniform int numLights;
uniform struct Light {
   vec4 pos;
   vec3 intensity;
   float ambCoeff;
} lights[MAX_LIGHTS];

vec3 worldPos;
vec3 diffuseColor;
float specShine;
float shadowWidth = 2048.0;
const float M[25]=float[25](0.003f, 0.013f, 0.022f, 0.013f, 0.003f,
                 0.013f, 0.059f, 0.097f, 0.059f, 0.013f,
                 0.022f, 0.097f, 0.159f, 0.097f, 0.022f,
                 0.013f, 0.059f, 0.097f, 0.059f, 0.013f,
                 0.003f, 0.013f, 0.022f, 0.013f, 0.003f);

vec3 calcLight(Light light, vec3 normal, vec3 view) {
   vec3 surfaceToLight;
   float attenuation = 1.0;
   if (light.pos.w == 0.0) {
      // directional light
      surfaceToLight = normalize(light.pos.xyz);
      // no attenuation for light to simulate sun
      attenuation = 1.0;
   } else {
      surfaceToLight = normalize(light.pos.xyz - worldPos.xyz);
      float distToLight = length(light.pos.xyz - worldPos.xyz);
      //attenuation = 1.0 / (1.0 + pow(distToLight, 2);
      attenuation = 0.5;
   }
   
   // Ambient = Amb * intensity
   vec3 amb = light.ambCoeff * ambColor.rgb * light.intensity;
   
   // Diffuse = Diff * dot(N, L) * intensity
   float diffuseCoeff = max(0.0, dot(normal, surfaceToLight));
   vec3 diff = diffuseCoeff * diffuseColor.rgb * light.intensity;
   
   // Specular = Spec * pow(dot(vec3(view), R), shine)) * intensity
   float specularCoeff = 0.0;
   if (diffuseCoeff > 0.0) {
      specularCoeff = pow(max(0.0, dot(view, reflect(-surfaceToLight, normal))), specShine);
   }
   vec3 spec = specularCoeff * specColor * light.intensity;
   
   return amb + attenuation * diff;//(diff + spec);
}

/* returns 1 if shadowed */
/* called with the point projected into the light's coordinate space */
float TestShadow(vec4 LSfPos) {
  float contribution = 1.2;
  float offset = 1.0/shadowWidth;
	//1: shift the coordinates from -1, 1 to 0 ,1
  vec3 shift = (LSfPos.xyz + vec3(1.0))/2.0;
	//2: read off the stored depth (.) from the ShadowDepth, using the shifted.xy 
  float curD = shift.z;
  for(int i = -2; i <= 2; i++) {
    for(int j = -2; j <= 2; j++) {
      //3: compare to the current depth (.z) of the projected depth
      vec2 sTexCord = vec2(shift.x + i * offset, shift.y + j * offset);
      float lightD = texture(shadowDepth, sTexCord).r;
      //4: subtract shadow contribution
      if(curD >= lightD + 0.01) {
        contribution -= M[(i + 2) * 5 + (j + 2)];
      }
    }
  }
	return contribution;
}

void main() {
    // Retrieve data from gbuffer
    worldPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    diffuseColor = texture(gAlbedoSpec, TexCoords).rgb;
    specShine = texture(gAlbedoSpec, TexCoords).a;
    vec3 view = normalize(-1 * vec3(worldPos));

    vec3 sumColor = vec3(0);

    float inShade = TestShadow(LS * vec4(worldPos, 1.0));

    for (int i = 0; i < numLights; i++) {
      sumColor += calcLight(lights[i], normal, view);
    }

    //color = vec4(diffuseColor, 1.0);
    color = vec4(sumColor.xyz * diffuseColor.xyz * inShade, 1.0);
    if(diffuseColor.xyz == vec3(0))
      discard;

}