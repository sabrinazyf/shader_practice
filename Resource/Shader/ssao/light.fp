#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 position;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Light light;

void main()
{
    vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec4 Diffuse = texture(gAlbedoSpec, TexCoords);

    // Then calculate lighting as usual
    vec3 viewDir  = normalize(viewPos - FragPos); // Viewpos is (0.0.0)
    // Diffuse
    vec3 lightDir = normalize(light.position - FragPos);
//    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.diffuse;
//    // Specular
//    vec3 halfwayDir = normalize(lightDir + viewDir);
//    float spec = pow(max(dot(Normal, halfwayDir), 0.0), texture(gAlbedoSpec, TexCoords).a);
//    vec3 specular = light.diffuse * spec;
//    // Attenuation
//    vec3 lighting = diffuse;
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 0.9 * light.diffuse * (diff * Diffuse.xyz) + 0.1;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Diffuse.a);
    vec3 specular = light.specular * (spec * Diffuse.xyz);
    vec3 result = diffuse + specular;
    FragColor = vec4(norm, 1.0);



    FragColor = vec4(result, 1.0);


//    vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;
//    vec3 Normal = texture(gNormal, TexCoords).rgb;
//    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
//    float Specular = texture(gAlbedoSpec, TexCoords).a;
//    float Depth = texture(gPositionDepth, TexCoords).a;
//
//    // diffuse
//    vec3 norm = normalize(Normal);
//    vec3 lightDir = normalize(light.position - FragPos);
//    float diff = 0.9 * max(dot(norm, lightDir), 0.0)+0.1;
//    vec3 diffuse = light.diffuse * (diff * Diffuse);
//
//    // specular
//    vec3 viewDir = normalize(viewPos - FragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Specular);
//    vec3 specular = light.diffuse * (spec * light.diffuse);
//
//    vec3 result = diffuse + specular;
//
//    if (Depth<1){
//        FragColor = vec4(result, 1.0);
//    } else {
//        FragColor = vec4(Diffuse, 1.0);
//    }
//        FragColor = vec4(result, 1.0);
//    FragColor = vec4(Diffuse, 1.0);

//    FragColor = vec4(Depth);
}