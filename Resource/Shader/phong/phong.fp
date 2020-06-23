#version 330 core

in vec3 p;
in vec3 n;

out vec4 color;

//uniform sampler2D texture_diffuse1;

void main()
{
    //    color = vec4(texture(texture_diffuse1, TexCoords));
    color = vec4(p, 1.0);
}