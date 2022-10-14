#version 330

uniform sampler2D sampler_tex;

in vec3 varying_position;
in vec2 varying_coord;
in vec3 varying_normal;

in float varying_gouraud;

out vec4 fragment_colour;

void main(void)
{
	vec3 tex_colour = texture(sampler_tex, varying_coord).rgb;

	tex_colour *= varying_gouraud;

	fragment_colour = vec4(tex_colour, 1.0);
}