#include "Planet.h"
#include "GeometryManager.h"
#include "ShaderManager.h"
#include "ImageReader.h"
#include "Transformation.h"

#define PI 3.14159265f
#define SLICE_SIZE 10.f
#define MAX_THETA_DEGS 360.f
#define MAX_PHI_DEGS 180.f
#define MAX_THETA_CUTS (int)(MAX_THETA_DEGS / SLICE_SIZE)
#define MAX_PHI_CUTS (int)(MAX_PHI_DEGS / SLICE_SIZE)
#define FRAMES_PER_SECOND 40.f
#define FF_SPEED 50.f

#define X 0
#define Y 1
#define Z 2

// Default Constructor.
Planet::Planet( float fRadius, 
				const string& sTextureName, 
				Transformation* pTransform,
				float fAxialTilt, 
				float fSecsForRotation, 
				float fSecsForOrbit,
				bool bLightPlanet )
{
	// Bind Texture to this Planet.
	InitializeTexture( &m_pTexture, sTextureName );

	// Init Position (World Coordinates)
	m_vPos = vec3( 0, 0, 0 );

	// Texture Information for Geometry in shaders
	m_pTextCoords[0][X] = 0;
	m_pTextCoords[0][Y] = 0;
	m_pTextCoords[1][X] = 0;
	m_pTextCoords[1][Y] = m_pTexture.height;
	m_pTextCoords[2][X] = m_pTexture.width;
	m_pTextCoords[2][Y] = m_pTexture.height;
	m_pTextCoords[3][X] = m_pTexture.width;
	m_pTextCoords[3][Y] = 0;

	m_bUpdated = true;
	m_bAnimate = true;
	m_bFastForward = false;
	m_bLightPlanet = bLightPlanet;
	m_fRadius = fRadius;
	m_fCurrRotation = 0.f;

	// degrees devided by seconds per rotation = degrees per second
	m_fOrbitPerFrame = (0 == fSecsForOrbit) ? 0.f :  360.f / fSecsForOrbit;
	m_fRotPerFrame = (0 == fSecsForRotation) ? 0.f : 360.f / fSecsForRotation;
	m_pLastTick = clock();

	m_pTransform = pTransform;

	// Develop UV coordinates for Planet
	constructUVCoords();

	// Develop Axial Tilt Matrix
	m_AxialTilt = rotate( mat4( 1.f ), fAxialTilt, vec3( 0, 0, 1 ) );
	m_pTransform->updateRotation( m_AxialTilt );
}

// Destructor
Planet::~Planet()
{
	m_pTransform = NULL;
}

// Deconstructs the Planet into approximated Triangles and sets up OpenGL to render the Triangles.
void Planet::renderPlanet()
{
	GeometryManager* m_pGmtryMngr = GeometryManager::getInstance();
	ShaderManager* m_pShdrMngr = ShaderManager::getInstance();
	const MyGeometry* pGeometry = m_pGmtryMngr->getGeometry();

	// Update Planet Rotations
	updatePlanet();

	// Bind Planet's Texture Data for Drawing this Planet.
	m_pGmtryMngr->bindTextureData( sizeof( m_pTextCoords ), &m_pTextCoords );
	setLocalTransform( m_pShdrMngr );
	m_pShdrMngr->setWorldMatrix( m_pTransform->getTransformationMatrix( true ) );
	m_pShdrMngr->setLightBool( m_bLightPlanet );

	// bind our shader program and the vertex array object containing our
	// scene geometry, then tell OpenGL to draw our geometry
	glUseProgram( m_pShdrMngr->getProgram( TEXTURE ) );
	glBindVertexArray( pGeometry->vertexArray );

	// Set Vertices for Drawing
	glBindBuffer( GL_ARRAY_BUFFER, pGeometry->vertexBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float )*m_vVertices.size(), &m_vVertices[0] );
	glBindBuffer( GL_ARRAY_BUFFER, pGeometry->colourBuffer );
	glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof( float )*m_vColors.size(), &m_vColors[0] );
	glBindTexture( GL_TEXTURE_2D, m_pTexture.textureName );

	glDrawArrays( GL_TRIANGLES, 0, m_iNumTris );

	glBindVertexArray( 0 );
	glUseProgram( 0 );
}

// Binds the texture to the geometry
void Planet::getTextureData( GLsizeiptr* iPtr, void** data )
{
	*iPtr = sizeof( m_pTextCoords );
	*data = m_pTextCoords;
}

/***************************************************************************************************\
 * Private Functions																			   *
\***************************************************************************************************/

void Planet::constructUVCoords()
{
	// Float values for cartesean coordinates
	float fX, fY, fZ;
	float fPhi_Rads, fTheta_Rads;
	vec3 vTop, vBottom;
	vector<vec3> vMiddleVerts;
	vector<vec3> vSphereVerts;

	if ( m_bUpdated )
	{
		m_vVertices.clear();
		vTop = vec3( 0.f, m_fRadius, 0.f );
		vBottom = vec3( 0.f, -m_fRadius, 0.f );

		/*	X = r·sinϕ·cosθ
			Y = r·sinϕ·sinθ
			Z = r·cosϕ
			*/
		// Calculates Cartesean Coordinates for Triangles in relative coordinates to the Planet.
		for ( float fPhi = SLICE_SIZE; fPhi < MAX_PHI_DEGS; fPhi += SLICE_SIZE )
		{
			fPhi_Rads = fPhi * PI / 180.f;

			for ( float fTheta = 0.f; fTheta < MAX_THETA_DEGS; fTheta += SLICE_SIZE )
			{
				fTheta_Rads = fTheta * PI / 180.f;
				fZ = m_fRadius * sin( fPhi_Rads );	// Z = r·sinϕ
				fX = fZ * sin( fTheta_Rads );		// use Z for X = r·sinϕ·sinθ
				fX = abs( fX ) < FLT_EPSILON ? 0.f : fX;
				fZ *= cos( fTheta_Rads );			// Finish Z: Z = r·sinϕ·cosθ
				fZ = abs( fZ ) < FLT_EPSILON ? 0.f : fZ;
				fY = (m_fRadius * cos( fPhi_Rads ));	// Y: r·cosϕ
				fY = abs( fY ) < FLT_EPSILON ? 0.f : fY;
				
				// Add the Coordinate.
				vMiddleVerts.push_back( vec3( fX, fY, fZ ) );
				vSphereVerts.push_back( vec3( fPhi, fTheta, m_fRadius ) );
			}
		}

		// Gathered all Points on Sphere.
		for ( unsigned int i = 0; i < (unsigned int)MAX_THETA_CUTS; ++i )
		{
			int i_Nxt = (i + 1) % MAX_THETA_CUTS;
			add3DVertex( vMiddleVerts[i][X], vMiddleVerts[i][Y], vMiddleVerts[i][Z] );
			addColor( vSphereVerts[i][X], vSphereVerts[i][Y], vSphereVerts[i][Z] );
			add3DVertex( vTop[X], vTop[Y], vTop[Z] );
			if ( MAX_THETA_CUTS - 1 == i )
				addColor( 0.f, vSphereVerts[i_Nxt][Y] + 360.f, vSphereVerts[i_Nxt][Z] );
			else
				addColor( 0.f, vSphereVerts[i_Nxt][Y], vSphereVerts[i_Nxt][Z] );
			add3DVertex( vMiddleVerts[i_Nxt][X], vMiddleVerts[i_Nxt][Y], vMiddleVerts[i_Nxt][Z] );
			if ( MAX_THETA_CUTS - 1 == i )
				addColor( vSphereVerts[i_Nxt][X], vSphereVerts[i][Y] + SLICE_SIZE, vSphereVerts[i_Nxt][Z] );
			else
				addColor( vSphereVerts[i_Nxt][X], vSphereVerts[i_Nxt][Y], vSphereVerts[i_Nxt][Z] );
		}
		for ( unsigned int i = 0; i < vMiddleVerts.size() - MAX_THETA_CUTS; i += MAX_THETA_CUTS )
			for ( unsigned int p = 0; p < MAX_THETA_CUTS; ++p )
			{
				// Forward Looking Variables
				int p_Next = (p + 1) % MAX_THETA_CUTS;
				int i_Next = i + MAX_THETA_CUTS;
		
				// Add Square Cut
				// Triangle 1
				/*
					P1
					|\
					| \
					P3_P2
				*/
				add3DVertex( vMiddleVerts[i + p][X], vMiddleVerts[i + p][Y], vMiddleVerts[i + p][Z] );									// P1
				addColor( vSphereVerts[i + p][X], vSphereVerts[i + p][Y], vSphereVerts[i + p][Z] );
				add3DVertex( vMiddleVerts[i_Next + p_Next][X], vMiddleVerts[i_Next + p_Next][Y], vMiddleVerts[i_Next + p_Next][Z] );	// P2
				if ( 0 == p_Next )
					addColor( vSphereVerts[i_Next + p_Next][X], vSphereVerts[i_Next + p][Y] + SLICE_SIZE, vSphereVerts[i_Next + p_Next][Z] );
				else
					addColor( vSphereVerts[i_Next + p_Next][X], vSphereVerts[i_Next + p_Next][Y], vSphereVerts[i_Next + p_Next][Z] );
				add3DVertex( vMiddleVerts[i_Next + p][X], vMiddleVerts[i_Next + p][Y], vMiddleVerts[i_Next + p][Z] );					// P3
				addColor( vSphereVerts[i_Next + p][X], vSphereVerts[i_Next + p][Y], vSphereVerts[i_Next + p][Z] );
				// Triangle 2
				/*
					P1_P2
					 \ |
					  \|
					  P3
				*/
				add3DVertex( vMiddleVerts[i + p][X], vMiddleVerts[i + p][Y], vMiddleVerts[i + p][Z] );									// P1
				addColor( vSphereVerts[i + p][X], vSphereVerts[i + p][Y], vSphereVerts[i + p][Z] );
				add3DVertex( vMiddleVerts[i + p_Next][X], vMiddleVerts[i + p_Next][Y], vMiddleVerts[i + p_Next][Z] );					// P2
				if ( 0 == p_Next )
					addColor( vSphereVerts[i + p_Next][X], vSphereVerts[i + p][Y] + SLICE_SIZE, vSphereVerts[i + p_Next][Z] );
				else
					addColor( vSphereVerts[i + p_Next][X], vSphereVerts[i + p_Next][Y], vSphereVerts[i + p_Next][Z] );
				add3DVertex( vMiddleVerts[i_Next + p_Next][X], vMiddleVerts[i_Next + p_Next][Y], vMiddleVerts[i_Next + p_Next][Z] );	// P3
				if ( 0 == p_Next )
					addColor( vSphereVerts[i_Next + p_Next][X], vSphereVerts[i_Next + p][Y] + SLICE_SIZE, vSphereVerts[i_Next + p_Next][Z] );
				else
					addColor( vSphereVerts[i_Next + p_Next][X], vSphereVerts[i_Next + p_Next][Y], vSphereVerts[i_Next + p_Next][Z] );
			}
		
		// Evaluate Bottom Cap
		// Gathered all Points on Sphere.
		for ( unsigned int i = vMiddleVerts.size() - MAX_THETA_CUTS; i < vMiddleVerts.size(); ++i )
		{
			unsigned int i_Nxt = (i + 1);
			i_Nxt = i_Nxt >= vMiddleVerts.size() ? vMiddleVerts.size() - MAX_THETA_CUTS : i_Nxt;
			add3DVertex( vMiddleVerts[i][X], vMiddleVerts[i][Y], vMiddleVerts[i][Z] );
			addColor( vSphereVerts[i][X], vSphereVerts[i][Y], vSphereVerts[i][Z] );
			add3DVertex( vMiddleVerts[i_Nxt][X], vMiddleVerts[i_Nxt][Y], vMiddleVerts[i_Nxt][Z] );
			if ( vMiddleVerts.size() - MAX_THETA_CUTS == i_Nxt )
				addColor( vSphereVerts[i_Nxt][X], vSphereVerts[i_Nxt][Y] + 360.0f, vSphereVerts[i_Nxt][Z] );
			else
				addColor( vSphereVerts[i_Nxt][X], vSphereVerts[i_Nxt][Y], vSphereVerts[i_Nxt][Z] );
			add3DVertex( vBottom[X], vBottom[Y], vBottom[Z] );
			if ( vMiddleVerts.size() - MAX_THETA_CUTS == i_Nxt )
				addColor( 180.f, vSphereVerts[i_Nxt][Y] + 360.0f, vSphereVerts[i_Nxt][Z] );
			else
				addColor( 180.f, vSphereVerts[i_Nxt][Y], vSphereVerts[i_Nxt][Z] );
		}

		m_iNumTris = m_vVertices.size() / 3;

		m_bUpdated = false;
	}
}

void Planet::setLocalTransform( ShaderManager* pShdrMngr )
{
	mat4 pRotation = rotate( mat4( 1.f ), m_fCurrRotation, vec3( 0, 1, 0 ) );
	mat4 pTransform = m_AxialTilt * pRotation;
	pShdrMngr->setLocalTransform( pRotation );
}

// Adds a 3D vertex for the cartesian uv-coordinates 
void Planet::add3DVertex( float fX, float fY, float fZ )
{
	m_vVertices.push_back( fX );
	m_vVertices.push_back( fY );
	m_vVertices.push_back( fZ );
}

// Adds a 3D vertex for the cartesian uv-coordinates 
void Planet::addColor( float fX, float fY, float fZ )
{
	m_vColors.push_back( fX );
	m_vColors.push_back( fY );
	m_vColors.push_back( fZ );
}

// rotates Planet's Axis as well as orbit around its parent body.
void Planet::updatePlanet()
{
	// Evaluate New Rotations based on Speed.
	clock_t mTimeNow = clock();

	if ( m_bAnimate )
	{
		float fAdjustedOrbit = m_bFastForward ? m_fOrbitPerFrame * FF_SPEED : m_fOrbitPerFrame;
		float fAdjustedRotation = m_bFastForward ? m_fRotPerFrame * FF_SPEED : m_fRotPerFrame;
		float fElapsedTime = (float)(mTimeNow - m_pLastTick) / CLOCKS_PER_SEC;
		float fPercentofSecond = fElapsedTime / FRAMES_PER_SECOND;
		mat4 mOrbitDelta = rotate( mat4( 1.f ), fAdjustedOrbit * fPercentofSecond, vec3( 0, 1, 0 ) );

		// Update Current Axis Rotation
		m_fCurrRotation += fAdjustedRotation * fPercentofSecond;
		m_fCurrRotation = m_fCurrRotation >= 360.f ? m_fCurrRotation - 360.f : m_fCurrRotation;
	
		// Update Orbit Rotation
		m_pTransform->updateRotation( mOrbitDelta );
	}

	m_pLastTick = clock();
}