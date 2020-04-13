#version 330

out vec4 color;
uniform vec4 uniformColor;

void main()
{
	color = uniformColor;
}