#version 330 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texcoord;

uniform mat4 combined_xform;
uniform mat4 model_xform;

out vec3 varying_position;
out vec3 varying_normal;
out vec2 varying_coord;

void main()
{
    varying_position = vec3(combined_xform * model_xform * vec4(vertex_position, 1.0));
    varying_normal = mat3(transpose(inverse(model_xform))) * vertex_normal;  
    varying_coord = vertex_texcoord;
    
    gl_Position = combined_xform * model_xform * vec4(vertex_position, 1.0);
}