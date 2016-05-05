// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// uniform = constant variable over everytime shader runs.
uniform sampler2D image;		

// Lighting Uniforms
uniform vec3 vSpecColor;
uniform float fSpecExp;
uniform bool bLightPixel;

// Color of Light
const vec3 vLightColor = vec3( 1.0, 1.0, 1.0 );				  

// interpolated colour received from vertex stage
in vec2 fragTexture;

// Input Variables for Lighting
in vec4 vNormal;
in vec3 vToLight;
in vec3 vToEye;
in vec3 vWorldPos;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

vec3 calculateDiffuse( )
{
	vec3 vDiffuseReturn;
	
	float fLightIntensity = dot(vec3(vNormal),vToLight);		// Intensity of the Light
	fLightIntensity = max( 0.0, fLightIntensity );	// Ignore negative Intensities
	
	vDiffuseReturn = vLightColor * fLightIntensity;
	
	return vDiffuseReturn;
}

vec3 calculateSpecular( )
{
	vec3 vSpecularReturn;
	float fCosSigma;
	vec3 vHVect = normalize( vToLight + vToEye );
	
	fCosSigma = dot( vHVect, vec3( vNormal ) );
	fCosSigma = max( 0.0, fCosSigma );
	fCosSigma = pow( fCosSigma, fSpecExp );
	vSpecularReturn = vLightColor * fCosSigma;
	vSpecularReturn *= vSpecColor;
	
	return vSpecularReturn;
}

void main(void)
{
	vec3 vDiffuseColor;
	vec3 vSpecColor;
	vec4 texelColor = texture(image, fragTexture);
	
	if( bLightPixel )
	{
		vDiffuseColor = calculateDiffuse();
		vSpecColor = calculateSpecular();
		
		texelColor *= vDiffuseColor;
		texelColor += vSpecColor;
	}
	
	FragmentColour = texelColor;
}
