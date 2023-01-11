#version 330 core

// Found at https://www.youtube.com/watch?v=vpDer0seP9M&ab_channel=BrianWill

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 fragment_position;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face;

        for(int i = 0; i < 3; ++i)
        {
            fragment_position = gl_in[i].gl_Position;

            gl_Position = shadowMatrices[face] * fragment_position;

            EmitVertex();
        }
        EndPrimitive();
    }
}