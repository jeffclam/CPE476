#version 330 core
uniform sampler2D colorTex;
uniform sampler2D normalTex;
uniform sampler2D shadowDepth;

uniform vec3 lightDir;

out vec4 Outcolor;
in mat3 TBN;

in OUT_struct {
   vec3 fPos;
   vec3 fragNor;
   vec2 vTexCoord;
   vec4 fPosLS;
   vec3 vColor;
} in_struct;

/* returns 1 if shadowed */
/* called with the point projected into the light's coordinate space */
float TestShadow(vec4 LSfPos) {

	float bias = 0.01;
	//1: shift the coordinates from -1, 1 to 0 ,1
	vec3 shift = (LSfPos.xyz + vec3(1.0))/2.0;	
	//2: read off the stored depth (.) from the ShadowDepth, using the shifted.xy 
	float curD = shift.z;
	float lightD = texture(shadowDepth, shift.xy).r;
	//3: compare to the current depth (.z) of the projected depth
	//4: return 1 if the point is shadowed
	if (curD - bias > lightD)
		return 1.0;
	else
		return 0.0;
}

void main() {

  float Shade;
  float amb = 0.3;
  float shine = 10.0;

  vec4 BaseColor = vec4(in_struct.vColor, 1);
  vec4 tColor = texture(colorTex, in_struct.vTexCoord);
  vec4 tNormal = texture(normalTex, in_struct.vTexCoord);

  Shade = TestShadow(in_struct.fPosLS);

  vec3 V = -1 * normalize(in_struct.fPos);
  vec3 H = (normalize(lightDir) + V)/2.0;
  vec3 N = 2.0*(tNormal.xyz) - 1.0;

  float sCoeff = pow( max(0, dot(N,H)), shine);
  float dCoeff = (max(0, dot((2.0 * tNormal.xyz) - 1.0, normalize(lightDir))));
  Outcolor = amb*tColor + (1.0-Shade)*vec4(dCoeff) * tColor + sCoeff * tColor;
  //Outcolor = sCoeff * tColor;
}

