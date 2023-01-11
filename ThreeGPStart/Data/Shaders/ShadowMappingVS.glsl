#version 330 core

// Found at https://www.youtube.com/watch?v=vpDer0seP9M&ab_channel=BrianWill

layout (location=0) in vec3 vertex_position;

uniform mat4 ModelMatrix;

void main()
{
    gl_Position = ModelMatrix * vec4(vertex_position, 1.0);
}