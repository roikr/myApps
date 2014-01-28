#version 150

in vec4 position;
in vec2 texcoord;

uniform mat4 modelViewProjectionMatrix;


uniform float timeValX;
uniform float timeValY;
uniform vec2 mouse;


void main(){
	gl_Position = modelViewProjectionMatrix * position;
}