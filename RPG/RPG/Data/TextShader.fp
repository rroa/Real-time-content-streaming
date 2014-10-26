#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
smooth in vec4 theColor;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D textureSampler;

void main()
{
	color = texture( textureSampler, UV ) * theColor;
}