#version 410

layout (vertices = 4) out; // Tell the system how big a "patch" is

in vec3 vs_Colour[];
out vec3 te_Colour[];

void main()
{
	// Setting Tesselation Level for Outer (also a level for inner).
	gl_TessLevelOuter[0] = 1;	// x levels along the curve.
	gl_TessLevelOuter[1] = 32; 	// 32 levels along the curve <- Linux != Windows 
					// 	implementation.  Numbers are swapped.
	
	// Just pass in the positions
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	
	te_Colour[gl_InvocationID] = vs_Colour[gl_InvocationID];
	
}
