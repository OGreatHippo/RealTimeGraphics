#version 330 core

// Found at https://www.youtube.com/watch?v=vpDer0seP9M&ab_channel=BrianWill

in vec4 fragment_position;

uniform vec3 lightPos;
uniform float far;

void main() 
{
    float len = length(fragment_position.xyz - lightPos);

    len /= far;

    gl_FragDepth = len;
}