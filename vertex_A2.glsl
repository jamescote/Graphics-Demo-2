// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexColour;
layout(location = 2) in vec2 VertexTexture;

// output to be interpolated between vertices and passed to the fragment stage
out vec2 fragTexture;

// Output Variables for Lighting
out vec4 vNormal;
out vec3 vToLight;
out vec3 vToEye;
out vec3 vWorldPos;

// Uniform Projection Matrices
uniform mat4 mToWorld;
uniform mat4 mToCamera;
uniform mat4 mPerspective;
uniform mat4 mLocalTransform;
uniform vec3 mCameraLocation;

// Constants
const float fZeroBound = 1e-7;
const float PI = 3.14159265;

// Light Position (In World Coordinates)
const vec3 vLightPos = vec3( 0.0, 0.0, 0.0 );

void main()
{
    // assign vertex position without modification
    gl_Position = vec4(VertexPosition, 1.0);
	
	// truncate texture coords to values between [0,1]
	fragTexture.x = VertexColour.y / 360.0;
	fragTexture.y = VertexColour.x / -180.0;
	
	// Apply Tranformations
	gl_Position = mLocalTransform * gl_Position;		// Local Rotations	
	
	// Get Normal in World Space.
	vNormal = normalize( vec4( vec3( gl_Position ), 0.0 ) );
	vNormal = mToWorld * vNormal;
	
	gl_Position = mToWorld * gl_Position;				// Translate->Rotate to world position
	
	// Calculate Lighting Vectors now that Vertex is in World Coords
	vWorldPos 	= vec3( gl_Position );
	vToEye 		= normalize( mCameraLocation - vWorldPos );
	vToLight 	= normalize( vLightPos - vWorldPos );
	
	// Translate Vertex to View Space, we have everything we need to shade it.
	gl_Position = mToCamera * gl_Position;				// transform to Camera Space
	gl_Position = mPerspective * gl_Position;			// transform to View Space
}
