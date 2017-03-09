#version 330 core

in vec4 color;
in vec4 texCoord;

uniform sampler2D whiteTex;
uniform sampler2D blackTex;
uniform sampler2D whiteKTex;
uniform sampler2D blackKTex;

out vec4 outColor;

void main()
{
	vec2 texCoord2D = vec2((texCoord.x * 0.91 + 1) / 2, (texCoord.y * 0.91 + 1) / 2);
	if (color == vec4(0.2, 0.2, 0.2, 1))
		outColor = texture(blackTex, texCoord2D);
	else if (color == vec4(0.8, 0.8, 0.8, 1))
		outColor = texture(whiteTex, texCoord2D);
	else if (color == vec4(0.5, 0.2, 0.2, 1))
		outColor = texture(blackKTex, texCoord2D);
	else if (color == vec4(0.7, 0.7, 1, 1))
		outColor = texture(whiteKTex, texCoord2D);
	else
		outColor = color;
}
