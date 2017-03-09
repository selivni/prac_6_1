#version 330 core

in vec4 point;
in vec4 position;
in vec4 color_in;

uniform vec2 scaleVector;

out vec4 color;
out vec4 texCoord;

void main()
{
	mat4 scaleMatrix = mat4(1.0);
	texCoord = point;
	scaleMatrix[0][0] = scaleVector.x / 2;
	scaleMatrix[1][1] = scaleVector.y / 2;
	color = color_in;
	gl_Position = scaleMatrix * point + position;
}
