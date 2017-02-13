#version 330 core
uniform sampler2D tex;


in vec2 vTexCoord;
in vec3 fragNor;
in vec4 worldPos;
//in vec4 gl_fragCoord;
out vec4 color;

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
   
   return amb + attenuation * (diff + spec);
}

void main()
{
    vec4 texColor = texture(tex, vTexCoord);
    vec3 normal = normalize(fragNor);
    vec3 view = normalize(-1 * vec3(worldPos));

    vec3 sumColor = vec3(0);

    for (int i = 0; i < numLights; i++) {
      sumColor += calcLight(lights[i], normal, view);
   }

    color = vec4(sumColor[0] * texColor[0], 
                 sumColor[1] * texColor[1], 
                 sumColor[2] * texColor[2], 
                                        texColor[3]);
}

