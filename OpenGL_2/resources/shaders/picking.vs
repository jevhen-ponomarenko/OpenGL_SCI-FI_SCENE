#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 projection;

smooth out float depth;

void main()
{
    vec4 pos4 = model * vec4(position, 1.0f);
    gl_Position = projection * pos4;
    depth = pos4.z;
}
