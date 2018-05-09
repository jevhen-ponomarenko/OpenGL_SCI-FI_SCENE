#version 330 core

uniform float id;
smooth in float depth;

out vec4 colorID;

void main()
{
    colorID = vec4(id, 1, -depth/1000, 0);
}
