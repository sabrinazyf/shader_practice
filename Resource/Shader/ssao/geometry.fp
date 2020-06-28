#version 330 core
layout (location = 0) out vec4 gPositionDepth;
//layout (location = 0) out vec3 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

struct Material {
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

//uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;
uniform Material material;

const float NEAR = 0.1; // Projection matrix's near plane distance
const float FAR = 50.0f; // Projection matrix's far plane distance

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPositionDepth.xyz = FragPos;
//    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z);
    gPositionDepth.a = gl_FragCoord.z;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
//    gNormal = Normal;
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = material.diffuse.rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = material.shininess;
}