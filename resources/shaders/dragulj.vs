#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Pos;
void main()
{
    Pos = vec3(aPos.x,aPos.y,aPos.z);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}