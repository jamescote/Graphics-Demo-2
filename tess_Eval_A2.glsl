#version 410

layout(isolines, equal_spacing) in;

// Pass through the FragShader for Fragment Shader
in vec2 tcFragTexture[];
out vec2 patchFragTexture;

void main()
{
	//patchFragTexture = tcFragTexture[gl_InvocationID];
}