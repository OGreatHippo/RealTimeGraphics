#version 330 core

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_texcoord; // might need to be 2

out vec2 varying_coord;

void main()
{
    varying_coord = vertex_texcoord;
    gl_Position = vec4(vertex_position, 1.0);
}