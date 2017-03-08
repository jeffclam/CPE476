#version 330 core

#define VARIATION 5

uniform sampler2D tex;
uniform sampler2D shadowDepth;


in vec2 vTexCoord;
in vec3 fragNor;
in vec4 worldPos;
//in vec4 gl_fragCoord;
out vec4 color;
in vec4 fPosLS;

uniform vec3 ambColor;
uniform vec3 specColor;
uniform float specShine;
uniform vec3 diffuseColor;

#define MAX_LIGHTS 10
uniform int numLights;
uniform struct Light {
   vec4 pos;
   vec3 intensity;
   float ambCoeff;
} lights[MAX_LIGHTS];

float shadowWidth = 2048.0;
//gaus blur
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

void main()
{
    vec4 texColor = texture(tex, vTexCoord);
    vec3 normal = normalize(fragNor);
    vec3 view = normalize(-1 * vec3(worldPos));


	float metallic = dot(-fragNor, view);
	metallic = smoothstep(0.3, .6, metallic);
	metallic = metallic/2 + 0.5;

    vec3 sumColor = vec3(0);

    float inShade = TestShadow(fPosLS);

    for (int i = 0; i < numLights; i++) {
      sumColor += calcLight(lights[i], normal, view);
    }
	//sumColor = ceil(sumColor * VARIATION) / VARIATION;
	//sumColor *= metallic;
	//color = vec4(texColor.xyz * metallic * inShade, texColor.w);
	color = vec4(sumColor.xyz * texColor.xyz * inShade, texColor[3]);
    //color = vec4(color.xyz * metallic, texColor[3]);
	//color = vec4(texColor.xyz * inShade * metallic, texColor[3]);
	//color = vec4(ceil(texColor.xyz * VARIATION) / VARIATION * inShade, texColor[3]);
}

