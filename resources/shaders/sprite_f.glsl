#version 330 core
out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D spriteTexture;
uniform vec4 color;

void main()
{
    vec4 tex = texture(spriteTexture, texCoords);
    float colorMix = color.a;

    if (tex.a <= 0)
        discard;

    // Exclude pure black from color mixing
    if (tex.rgb == vec3(0, 0, 0))
        colorMix = 0.0;

    fragColor = mix(tex, vec4(color.rgb, 1.0), colorMix);
}