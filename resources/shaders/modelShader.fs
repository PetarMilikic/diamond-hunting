#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

struct Material {
//kada koristimo teksture, uvek smo ovako pisati, gde je diffuse tip teksture, a broj je redni broj teksture
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform PointLight pointLight;//treba nam point svetlo koje cemo podesavati iz main-a
uniform vec3 viewPosition; //pozicija kamere
uniform Material material;

//implementiracemo point svetlo(sijalica)
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    //kada god radimo za vektorima koji nam kazuju smer ili pravac, uvek ih normalizujemo,
    //da nas njihova velicina ne bi remetila pri izracunavanju
    vec3 normal = normalize(Normal);//vektor normale za fragment
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 result = CalcPointLight(pointLight, normal, FragPos, viewDir);
    FragColor = vec4(result, 1.0);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);//distanca je rastojanje izmedju izvora svetlosti i fragmenta
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoords));
    //pisemo xxx da bismo svaku komponentu u vec3(texture(texture_specular1, TexCoords) imali podjednako
    //zasto? razlog je naveden u svesci
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoords).xxx);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
