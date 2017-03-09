#version 330 core

in vec4 point;
in vec4 position;
in vec4 color_in;

out vec4 color;
out vec4 texCoord;

void main()
{
	color = color_in;
	texCoord = point + position;
	gl_Position = point + position;
}
