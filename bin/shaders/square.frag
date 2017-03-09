#version 330 core

in vec4 color;
in vec4 texCoord;

uniform sampler2D whiteTex;
uniform sampler2D blackTex;

out vec4 outColor;

void main()
{
	vec2 texCoord2D = vec2((texCoord.x + 1) / 2, (texCoord.y + 1) / 2);
	if (color == vec4(1, 1, 1, 1))
		outColor = texture(whiteTex, texCoord2D);
	else if (color == vec4(0, 0, 0, 1))
		outColor = texture(blackTex, texCoord2D);
	else
		outColor = color;
}
