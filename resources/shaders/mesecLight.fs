#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
//kada koristimo teksture, uvek smo ovako pisati, gde je diffuse tip teksture, a broj je redni broj teksture
uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
}
