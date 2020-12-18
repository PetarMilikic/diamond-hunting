#version 330 core
out vec4 FragColor;

//uniform vec4 faktor;
in vec3 Pos;
void main()
{
    FragColor = vec4(abs(sin(Pos.x)),abs(Pos.y)/2+0.3,abs(cos(Pos.z)),1.0);
}



