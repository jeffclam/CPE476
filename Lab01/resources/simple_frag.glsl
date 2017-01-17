#version 330 core
in vec3 fragNor;
in vec4 worldPos;
out vec4 color;

uniform vec3 lightPos;
uniform vec3 lightColor;

uniform vec3 ambColor;

uniform vec3 specColor;
uniform float specShine;

uniform vec3 diffuseColor;

void main()
{
    vec3 normal = normalize(fragNor);
    // diffuse
    vec3 l = normalize(lightPos - vec3(worldPos));
    vec3 diffuse = max(dot(normal, l), 0) * diffuseColor * lightColor;
    //specular
    vec3 r = normalize(-l + 2 * dot(normal, l) * normal);
    vec3 v = normalize(-1 * vec3(worldPos));
    vec3 specular = pow(max(0, dot(v,r)),specShine) * specColor * lightColor;
    //ambient
    vec3 ambient = ambColor * lightColor;
    
    vec3 vertColor = diffuse + specular + ambient;
    
    color = vec4(vertColor, 1.0);
}

