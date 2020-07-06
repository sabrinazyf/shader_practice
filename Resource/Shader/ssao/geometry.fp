#version 400 core
layout (location = 0) out vec4 gPositionDepth;
//layout (location = 0) out vec3 gPositionDepth;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
uniform sampler2D gShadowDepth;
uniform vec3 lightPosition;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
in vec4 fragPosLightSpace;

//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;
uniform Material material;

const float NEAR = 0.1;// Projection matrix's near plane distance
const float FAR = 50.0f;// Projection matrix's far plane distance

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;// Back to NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 FragPos, vec3 Normal)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(gShadowDepth, projCoords.xy).r;
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(gShadowDepth, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(gShadowDepth, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0) shadow = 0.0;
    return 1-shadow;
    //    return closestDepth;
}

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPositionDepth.xyz = FragPos;
    //    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z);
    gPositionDepth.a = gl_FragCoord.z;
    // also store the per-fragment normals into the gbuffer
    gNormal.xyz = normalize(Normal).xyz;
    //    gNormal = Normal;
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = material.diffuse.rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = material.shininess;
    gNormal.a = ShadowCalculation(fragPosLightSpace, FragPos, gNormal.xyz);
}