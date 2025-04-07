#version 330 core
in vec3 Color;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
out vec4 fragColor;
uniform sampler2D texture1;
uniform vec3 viewPos;
void main(void)
{
	vec3 result = Color;
   fragColor = texture(texture1, TexCoord) * vec4(result, 1.0f);
}