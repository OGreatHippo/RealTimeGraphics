#version 330 core

//layout(location = 0) in vec3 vertex_position;
//layout(location = 1) in vec2 vertex_texcoord; 
//
//out vec2 varying_coord;
//
//void main()
//{
//    varying_coord = vertex_texcoord;
//    gl_Position = vec4(vertex_position, 1.0);
//}

out vec2 varying_coord;

void main(void)
{
	vec4 vertices[4] = vec4[4](vec4(-1.0, -1.0, 0.0, 1.0), vec4(1.0, -1.0, 0.0, 1.0), vec4(-1.0, 1.0, 0.0, 1.0), vec4(1.0, 1.0, 0.0, 1.0));
	vec2 texCoord[4] = vec2[4](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0));

	varying_coord = texCoord[gl_VertexID];

	gl_Position = vertices[gl_VertexID];
}
