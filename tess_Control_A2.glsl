#version 410

layout (vertices = 4) out; // Tell the system how big a "patch" is

// Pass through the FragShader for Fragment Shader
in vec2 fragTexture[];
out vec2 tcFragTexture[];

void main()
{
	tcFragTexture[gl_InvocationID] = fragTexture[gl_InvocationID];
}