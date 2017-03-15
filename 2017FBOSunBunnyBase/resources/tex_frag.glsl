#version 330 core

in vec2 texCoord;
out vec4 color;
uniform sampler2D texBuf;
int center = 512;

/* just pass through the texture color but add a little blue to anything on left side */
void main(){
   color = vec4(texture( texBuf, vec2(texCoord) ).rgb, 1);
   vec4 centerColor = vec4(texture( texBuf, vec2(0.5) ).rgb, 1);

   float dist = distance(vec2(center), vec2(gl_FragCoord.x, gl_FragCoord.y));
   dist = max(dist, 0.01);
	color.xyz += vec3(centerColor * (1.0-dist/128));

}
