#version 330 core

layout (location=0) in vec3 vertex_position;
layout (location=1) in vec3 vertex_normal;
layout (location=2) in vec2 vertex_texcoord;

out vec2 ftexcoord;
out vec3 Position;
out vec3 Normal;
out vec4 ShadowCoord;
out vec3 Untransformed_Position;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform mat3 NormalMatrix;
uniform mat4 ShadowMatrix;


void main()
{
    mat4 mv = ViewMatrix * ModelMatrix;
    mat4 mvp = ProjectionMatrix*mv;

    Position = (mv * vec4(vertex_position,1.0)).xyz;
    Normal = normalize( NormalMatrix * vertex_normal );
    ShadowCoord = ShadowMatrix * vec4(vertex_position, 1.0);
    Untransformed_Position = vertex_position;

    gl_Position = mvp * vec4(vertex_position, 1.0);
    ftexcoord = vertex_texcoord;
}