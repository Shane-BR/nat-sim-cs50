#version 330 core
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D textTexture;
uniform vec4 color;

void main()
{
    vec4 tex = texture(textTexture, texCoords);
    float colorMix = color.a;

    fragColor = mix(tex, vec4(color.rgb, 1.0), colorMix);
}