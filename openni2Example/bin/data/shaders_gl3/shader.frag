#version 150

uniform sampler2D src_tex_unit0;
uniform vec4 globalColor;

in vec2 texCoordVarying;

out vec4 fragColor;

void main(){
	//float color = texture(src_tex_unit0,gl_FragCoord.xy).x;
	fragColor = vec4(texture(src_tex_unit0,gl_FragCoord.xy).rgb,1.0);
}
