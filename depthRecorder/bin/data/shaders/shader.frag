#version 120

#extension GL_ARB_texture_rectangle : enable

uniform sampler2DRect tex0;
uniform float minEdge;
uniform float maxEdge;

void main (void){
	vec2 pos = gl_TexCoord[0].st;
	float color = texture2DRect(tex0, pos).r;
	float c = (color-minEdge)/(maxEdge-minEdge);
	float mask = step(minEdge,color)-step(maxEdge,color);
	gl_FragColor = vec4( vec3(1.0-c) , mask);
}