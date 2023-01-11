#version 330 core

// Found at https://www.gamedev.net/forums/topic/713242-opengl-4-depth-of-field/

out vec2 varying_coord;

void main(void)
{
	vec4 vertices[4] = vec4[4](vec4(-1.0, -1.0, 0.0, 1.0), vec4(1.0, -1.0, 0.0, 1.0), vec4(-1.0, 1.0, 0.0, 1.0), vec4(1.0, 1.0, 0.0, 1.0));
	vec2 texCoord[4] = vec2[4](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0));

	varying_coord = texCoord[gl_VertexID];

	gl_Position = vertices[gl_VertexID];
}
