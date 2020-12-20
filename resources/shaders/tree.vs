#version 330 core

layout (location=0) in vec3 aCords;
layout (location=1) in vec2 aTexCords;
layout (location=2) in vec3 aNormal;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords=aTexCords;
    FragPos=vec3(model*vec4(aCords,1.0));
    gl_Position=projection*view*model*vec4(aCords,1.0);
}