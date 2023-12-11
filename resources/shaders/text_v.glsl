#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec4 aColor;

uniform mat4 projection;

out vec2 texCoords;
out vec4 color;

void main()
{
    texCoords = aTexCoords;
    color = aColor;
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
}