#version 330 core
out vec4 color;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D shadowDepth;
uniform mat4 LS;
uniform int width;
uniform int height;

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
mat3 sx = mat3( 
    1.0, 2.0, 1.0, 
    0.0, 0.0, 0.0, 
   -1.0, -2.0, -1.0 
);
mat3 sy = mat3( 
    1.0, 0.0, -1.0, 
    2.0, 0.0, -2.0, 
    1.0, 0.0, -1.0 
);
const float M[25]=float[25](0.003f, 0.013f, 0.022f, 0.013f, 0.003f,
                 0.013f, 0.059f, 0.097f, 0.059f, 0.013f,
                 0.022f, 0.097f, 0.159f, 0.097f, 0.022f,
                 0.013f, 0.059f, 0.097f, 0.059f, 0.013f,
                 0.003f, 0.013f, 0.022f, 0.013f, 0.003f);

float stepmix(float edge0, float edge1, float E, float x) {
    float T = clamp(0.5 * (x - edge0 + E) / E, 0.0, 1.0);
    return mix(edge0, edge1, T);
}

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
      attenuation = 1.0 / (1.0 + pow(distToLight, 2));
   }
   
   // Ambient = Amb * intensity
   vec3 amb = light.ambCoeff * light.intensity;
   
   	float A = 0.1;
	float B = 0.3;
	float C = 0.6;
	float D = 1.0;

   // Diffuse = Diff * dot(N, L) * intensity
   float diffCoeff = max(0.0, dot(normal, surfaceToLight));
   
   float E = fwidth(diffCoeff);
   /*
   if (diffCoeff > A - E && diffCoeff < A + E)
       diffCoeff = stepmix(A, B, E, diffCoeff);
   else if (diffCoeff > B - E && diffCoeff < B + E)
       diffCoeff = stepmix(B, C, E, diffCoeff);
   else if (diffCoeff > C - E && diffCoeff < C + E)
       diffCoeff = stepmix(C, D, E, diffCoeff);
   else */if (diffCoeff < A)
		diffCoeff = 0.0;
	else if (diffCoeff < B)
		diffCoeff = B;
	else if (diffCoeff < C)
		diffCoeff = C;
	else
		diffCoeff = D; 
   
   vec3 diff = diffCoeff * light.intensity;
   
   // Specular = Spec * pow(dot(vec3(view), R), shine)) * intensity
   float specCoeff = 0.0;
   if (diffCoeff > 0) {
	  specCoeff = pow(max(0.0, dot(view, reflect(-surfaceToLight, normal))), specShine);
	  E = fwidth(specCoeff); 
	  if (specCoeff > 0.3 - E && specCoeff < 0.3 + E)
		specCoeff = clamp(0.3 * (specCoeff - 0.3 + E) / E, 0.0, 1.0);
	  else 
		specCoeff = step(0.3, specCoeff); 
   }
   vec3 spec = specCoeff * light.intensity;
   
   return amb + attenuation * (diff + spec);
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

float outline() {
    mat3 I;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            float offsetX = float(i-1)/float(width);
            float offsetY = float(j-1)/float(height);
            vec3 sample  = texture(gAlbedoSpec, vec2(TexCoords) + vec2(offsetX, offsetY)).rgb;
            I[i][j] = length(sample); 
        }
    }
    float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]); 
    float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);
    float g = sqrt(pow(gx, 2.0)+pow(gy, 2.0));
    g = smoothstep(0.4, 0.6, g);
	return g;
}

void main() {
    // Retrieve data from gbuffer
    worldPos = texture(gPosition, TexCoords).rgb;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    diffuseColor = texture(gAlbedoSpec, TexCoords).rgb * .8;
    specShine = texture(gAlbedoSpec, TexCoords).a;
    vec3 view = normalize(-1 * vec3(worldPos));

	float inShade = TestShadow(LS * vec4(worldPos, 1.0));
	float g = outline();

    vec3 intensity = vec3(0);
    for (int i = 0; i < numLights; i++) {
      intensity += calcLight(lights[i], normal, view);
    }

    vec3 baseColor = diffuseColor.rgb;

	//color = vec4((diffuseColor.xyz * inShade) - vec3(g), 1.0);
	//color = vec4(intensity.xyz * inShade - vec3(g), 1.0);
	color = vec4((intensity.xyz * baseColor.xyz * inShade) - vec3(g), 1.0);

    if(diffuseColor.xyz == vec3(0))
      discard;

}