#version 330 core

uniform sampler2D sampler_tex;

in vec3 varying_position;
in vec2 varying_coord;
in vec3 varying_normal;

out vec4 fragment_colour;

void main()
{    
    vec3 tex_colour = texture(sampler_tex, varying_coord).rgb;
    fragment_colour = vec4(tex_colour * 0.1, 1.0);
} 