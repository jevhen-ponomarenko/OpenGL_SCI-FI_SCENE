#version 330 core

uniform float id;
smooth in float depth;

out vec4 colorID;

void main()
{
    colorID = vec4(0.8f, 0.7f,0.7f,0.7f);
}
