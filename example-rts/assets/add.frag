#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

layout(set=0, binding=0) uniform sampler2D u_texture0;
layout(set=0, binding=1) uniform sampler2D u_texture1;

layout(location = 0) in vec2 texcoord;

layout(location = 0) out vec4 f_color;

void main() 
{
	f_color = texture(u_texture0, texcoord) + texture(u_texture1, texcoord);
}