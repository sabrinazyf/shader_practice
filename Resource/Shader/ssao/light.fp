#version 400 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
//uniform sampler2D gShadowDepth;
uniform mat4 lightSpaceMatrix;
uniform mat4 cameraView;

struct Light {
    vec3 position;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 viewPos;
uniform Light light;
const float NEAR = 0.1;// Projection matrix's near plane distance
const float FAR = 50.0f;// Projection matrix's far plane distance

//float ShadowCalculation(vec4 fragPosLightSpace, vec3 FragPos, vec3 Normal)
//{
//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    // transform to [0,1] range
////    projCoords = projCoords * 0.5 + 0.5;
//    //    float closestDepth = textureProj(gShadowDepth, fragPosLightSpace);
////    float closestDepth = texture(gShadowDepth, projCoords.xy).r;
////    float currentDepth = projCoords.z;
//    // calculate bias (based on depth map resolution and slope)
//
//    vec3 normal = normalize(Normal);
//    vec3 lightDir = normalize(light.position - FragPos);
//    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
//    // check whether current frag pos is in shadow
//    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
//    // PCF
//    float shadow = 0.0;
//    vec2 texelSize = 1.0 / textureSize(gShadowDepth, 0);
//    for (int x = -1; x <= 1; ++x)
//    {
//        for (int y = -1; y <= 1; ++y)
//        {
//            float pcfDepth = texture(gShadowDepth, projCoords.xy + vec2(x, y) * texelSize).r;
//            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
//        }
//    }
//    shadow /= 9.0;
//
//    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
//    if (projCoords.z > 1.0)
//    shadow = 0.0;
//    if (closestDepth<currentDepth)
//    shadow = 1.0;
//    return shadow;
//}

//float readShadowMap(vec3 eyeDir)
//{
//    mat4 cameraViewToWorldMatrix = inverse(cameraView);
//    mat4 cameraViewToProjectedLightSpace = lightSpaceMatrix * cameraViewToWorldMatrix;
//    vec4 projectedEyeDir = cameraViewToProjectedLightSpace * vec4(eyeDir, 1);
//    projectedEyeDir = projectedEyeDir/projectedEyeDir.w;
//
//    vec2 textureCoordinates = projectedEyeDir.xy * vec2(0.5, 0.5) + vec2(0.5, 0.5);
//
//    const float bias = 0.0001;
//    float depthValue = texture2D(gShadowDepth, textureCoordinates).x - bias;
//    return depthValue;
////    return projectedEyeDir.z * 0.5 + 0.5 < depthValue ? 0:1.0;
//}

void main()
{
    //     perform perspective divide
    vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec4 Diffuse = texture(gAlbedoSpec, TexCoords);
    float Depth = texture(gPositionDepth, TexCoords).a;

//    vec4 fragPosLightSpace = lightSpaceMatrix * inverse(cameraView) * vec4(FragPos, 1.0);
    //    vec4 fragPosLightSpace = lightSpaceMatrix * cameraView * vec4(FragPos, 1.0);

    // Then calculate lighting as usual
    vec3 viewDir  = normalize(viewPos - FragPos);// Viewpos is (0.0.0)
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 norm = normalize(Normal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Diffuse.a);
    vec3 specular = light.specular * spec;

    vec3 eyeDir = FragPos.xyz - viewPos.xyz;

//    float shadow = ShadowCalculation(fragPosLightSpace, FragPos, Normal);
    float shadow =  texture(gNormal, TexCoords).a;

    vec3 lighting = ((max(shadow,0.5)) * (diffuse + specular)) * Diffuse.xyz;

//        vec3 result = (diffuse + specular)* Diffuse.xyz;

//    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//    // transform to [0,1] range
//    projCoords = projCoords * 0.5 + 0.5;
//    float closestDepth = textureProj(gShadowDepth, projCoords).r;
//    //    float closestDepth = texture(gShadowDepth, projCoords.xy).r;
//    float currentDepth = projCoords.z;

//    FragColor = vec4(lighting, 1.0);

    //
        if (Depth<1){
            FragColor = vec4(lighting, 1.0);
        } else {
            FragColor = vec4(Diffuse.xyz,1.0);
        }

    //    FragColor = vec4(Depth);
}