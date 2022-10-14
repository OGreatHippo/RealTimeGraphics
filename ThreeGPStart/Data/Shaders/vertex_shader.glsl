#version 330

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texcoord;

uniform mat4 combined_xform;
uniform mat4 model_xform;

vec3 light_position;
uniform vec3 light_intensity;

out vec2 varying_coord;
out vec3 varying_normal;
out vec3 varying_position;

out float varying_gouraud;

void main(void)
{
	light_position.y = 500;

	varying_normal = mat3(model_xform) * vertex_normal;
	varying_coord = vertex_texcoord;
	varying_position = mat4x3(model_xform) * vec4(vertex_position, 1.0);

	vec3 P = varying_position;
	vec3 N = vertex_normal;

	vec3 light_direction = light_position - P;
	vec3 L = normalize(light_direction);

	float light_intensity = max(0, dot(L, N));

	varying_gouraud = light_intensity;

	gl_Position = combined_xform * model_xform * vec4(vertex_position, 1.0);
}