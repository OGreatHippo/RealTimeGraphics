#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout(location = 2) in vec2 vertex_texcoord;

uniform mat4 combined_xform;
uniform mat4 model_xform;

out vec3 FragPos;
out vec3 Normal;
out vec2 varying_coord;

void main()
{
    FragPos = vec3(model_xform * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model_xform))) * aNormal;  
    varying_coord = vertex_texcoord;
    
    gl_Position = combined_xform * model_xform * vec4(FragPos, 1.0);
}