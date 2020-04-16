#version 330

in vec3 VertColor;
in vec2 TextCoord;

uniform sampler2D tex;

out vec4 color;


void main()
{
	//color = texture(tex, TextCoord) * vec4(VertColor, 1.0f);
	//color = texture(tex, vec2(TextCoord.s, 1.0 - TextCoord.t)) * vec4(VertColor, 1.0f); //翻转y轴
	color = texture(tex, vec2(TextCoord.s, 1.0 - TextCoord.t)); //翻转y轴,去除颜色混合
}