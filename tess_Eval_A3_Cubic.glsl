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
	vec4 p3  = gl_in[3].gl_Position;
	float fU = gl_TessCoord.x;
	float fUinv = (1.0-fU);
	
	float fP0Mod = pow(fUinv,3.0);
	float fP1Mod = pow(fUinv,2.0)*fU*3.0;
	float fP2Mod = pow(fU,2.0)*fUinv*3.0;
	float fP3Mod = pow(fU,3.0);
	
	// Curve Point.
	gl_Position = (fP0Mod*p0) + (fP1Mod*p1) + (fP2Mod*p2) + (fP3Mod*p3);
	
	// Send Colour
	Colour = te_Colour[0];
}