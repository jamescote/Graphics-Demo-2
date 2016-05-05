#pragma once

// Includes
#include "stdafx.h"
#include "ImageReader.h"

// Spherical Coordinates indicies
#define THETA 0
#define PHI 1
#define RADIUS 2
#define NUM_TEXTURE_COORDS 4
#define BASE 1e7f
#define LOG_BASE log(BASE)

// Forward Declarations
class Transformation;
class ShaderManager;

class Planet
{
public:
	Planet( float fRadius, 
			const string& sTextureName, 
			Transformation* pTransform, 
			float fAxisTilt, 
			float fSecsForRotation, 
			float fSecsForOrbit,
			bool bLightPlanet );
	~Planet();

	// Render Functions
	void renderPlanet();
	void toggleAnimation()	{ m_bAnimate = !m_bAnimate; }
	void toggleFastForward() { m_bFastForward = !m_bFastForward; }

	// Binds the TextureData on the Planet.
	void getTextureData( GLsizeiptr* iPtr, void** data );

private:
	// Planet Variables
	vec3 m_vPos; // Spherical Positions stored in Spherical Coordinates.
	GLuint m_pTextCoords[NUM_TEXTURE_COORDS][2];
	float m_fRadius;
	int m_iNumTris;
	MyTexture m_pTexture;
	Transformation* m_pTransform;
	bool m_bUpdated, m_bAnimate, m_bFastForward, m_bLightPlanet;
	vector<GLfloat> m_vVertices;	// Vertices for Drawing Triangles.
	vector<GLfloat> m_vColors;		// Color Values for Triangles

	// Localized Rotation
	mat4 m_AxialTilt;
	float m_fRotPerFrame, m_fCurrRotation;
	float m_fOrbitPerFrame;
	clock_t m_pLastTick;

	// Private Functions
	void add3DVertex( float fX, float fY, float fZ );
	void addColor( float fX, float fY, float fZ );
	void constructUVCoords();
	void setLocalTransform( ShaderManager* pShdrMngr );
	void updatePlanet();
};

