#version 330 core
out vec4 fragColor;

in vec2 texCoords;
in vec4 color;

uniform sampler2D textTexture;

void main()
{
    vec4 tex = texture(textTexture, texCoords);
    float colorMix = color.a;

    if (tex.a <= 0)
        discard;

    fragColor = mix(tex, vec4(color.rgb, tex.a), colorMix);
}