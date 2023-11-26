#version 330 core
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D spriteTexture;
uniform vec3 color;

void main()
{
    fragColor = texture(spriteTexture, texCoords);
}