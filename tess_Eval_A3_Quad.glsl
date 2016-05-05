#version 410

layout(isolines, equal_spacing) in;

in vec3 te_Colour[];
out vec3 Colour;

void main()
{
	// Set up variables.
	vec4 p0  = gl_in[0].gl_Position;
	vec4 p1  = gl_in[1].gl_Position;
	vec4 p2  = gl_in[2].gl_Position;
	float fU = gl_TessCoord.x;
	float fUinv = (1.0-fU);
	
	float fP0Mod = fUinv*fUinv;
	float fP1Mod = fUinv*fU*2.0;
	float fP2Mod = fU*fU;
	
	// Curve Point.
	gl_Position = (fP0Mod*p0) + (fP1Mod*p1) + (fP2Mod*p2);
	
	// Send Colour
	Colour = te_Colour[0];
}