#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
// uniform sampler2D texture1;
// uniform sampler2D texture2;
uniform vec4 customColor;

void main()
{
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2) * customColor;
    FragColor = customColor;
}