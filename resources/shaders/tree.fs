#version 330 core

out vec4 FragColor;
//ODAVDE svetlo:

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

//direkciono svetlo
struct Light {
    //vec3 position;
    //poziciju menjamo sa smerom padanja svetlosti iz tog izvora
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//point svetlo
struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//spot svetlo
struct SpotLight {
    vec3 position;//pozicija svetla
    vec3 direction;//smer padanja svetla
    float cutOff;//unutrasnji cut ugao
    float outerCutOff;//spoljasnji cut ugao

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


in vec2 TexCoords;
// floor:

//out vec4 FragColor;
//in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform PointLight pointLight;
uniform SpotLight spotLight;

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

//DOVDE svetlo :))


void main()
{

     vec4 TextureColorDiff= texture(material.diffuse,TexCoords);
     vec4 TextureColorSpec= texture(material.specular,TexCoords);
     if(TextureColorDiff.a<0.5) // texture1.a=texture1.alpha
             discard;           // odbacujemo ako je alfa<0.5
     if(TextureColorSpec.a<0.5) // texture1.a=texture1.alpha
            discard;           // odbacujemo ako je alfa<0.5

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);//vektor smera gledanja kamere
    vec3 result = CalcDirLight(light, norm, viewDir);
    result += CalcPointLight(pointLight, norm, FragPos, viewDir);
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    FragColor = vec4(result, 1.0);
}

//Pomocne funkcije:
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // difuzna komponenta
    float diff = max(dot(normal, lightDir), 0.0);
    // spekularna komponenta
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);//distanca je rastojanje izmedju izvora svetlosti i fragmenta
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);//length je duzina vektora
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}




