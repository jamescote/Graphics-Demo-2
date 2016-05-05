#include "Camera.h"

/***********\
 * Defines *
\***********/
#define PI					3.14159265f
#define VERT_LOWER_LIMIT	1.f - FLT_EPSILON
#define VERT_UPPER_LIMIT	180.f - VERT_LOWER_LIMIT
#define HORIZONTAL_LIMIT	360.f
#define ZOOM_MIN			0.05f
#define FOV_Y				60.f
#define Z_CLOSE				0.01f
#define Z_FAR				10000.f
#define START_RADIUS		50.f

// Vector Indexing
#define I_THETA				0		// Spherical
#define I_PHI				1
#define I_RADIUS			2
#define I_X					0		// Cartesian
#define I_Y					1
#define I_Z					2

// Shortened Indexing
#define PHI					m_vSPos[I_PHI]
#define THETA				m_vSPos[I_THETA]
#define RADIUS				m_vSPos[I_RADIUS]
#define X_LOOK				m_vSPos[I_X]
#define Y_LOOK				m_vSPos[I_Y]
#define Z_LOOK				m_vSPos[I_Z]

// Constructor
Camera::Camera( int iHeight, int iWidth )
{
	m_vSPos = vec3( 0.f, 90.f, START_RADIUS );	// (Theta, Phi, Radius)
	m_vWorldLookAt = vec3( 0.f, 0.f, 0.f );		// (X, Y, Z)
	updateHxW( iHeight, iWidth );

	m_bUpdated = true;
}

// Copy Constructor
Camera::Camera( Camera& pCopy )
{
	m_vSPos			= pCopy.m_vSPos;
	m_vWorldLookAt	= pCopy.m_vWorldLookAt;
	m_bUpdated		= pCopy.m_bUpdated;
}

// Destructor
Camera::~Camera()
{
	// Nothing to Destruct
}

// Generates toCamera Matrix and updates Uniform in ShaderManager.
mat4 Camera::getToCameraMat()
{
	vec3 vCamCPos = getCartesianPos();
	return lookAt( vCamCPos, m_vWorldLookAt, vec3( 0.f, 1.f, 0.f ) );
}

// Generates toCamera Matrix and updates Uniform in ShaderManager.
mat4 Camera::getPerspectiveMat()
{
	vec3 vCamCPos = getCartesianPos();
	return perspective( FOV_Y * PI/180.f, m_fAspectRatio, Z_CLOSE, Z_FAR );
}

// fetches the World Position of the Camera
vec3 Camera::getCameraWorldPos()
{
	return getCartesianPos();
}

// Returns the Current Camera Position in Cartesian Coordinates
vec3 Camera::getCartesianPos()
{
	float fPhi_Rads = PHI * PI / 180.f;
	float fTheta_Rads = THETA * PI / 180.f;
	vec3 vReturn;
	double fEp = DBL_EPSILON;

	vReturn.z = RADIUS * sin( fPhi_Rads );	// Z = r·sinϕ
	vReturn.x = vReturn.z * sin( fTheta_Rads );		// use Z for X = r·sinϕ·sinθ
	vReturn.x = abs( vReturn.x ) < FLT_EPSILON ? 0.f : vReturn.x;
	vReturn.z *= cos( fTheta_Rads );			// Finish Z: Z = r·sinϕ·cosθ
	vReturn.z = abs( vReturn.z ) < FLT_EPSILON ? 0.f : vReturn.z;
	vReturn.y = RADIUS * cos( fPhi_Rads );	// Y: r·cosϕ
	vReturn.y = abs( vReturn.y ) < FLT_EPSILON ? 0.f : vReturn.y;

	mat4 mLookAtTranslation = translate( mat4( 1.f ), m_vWorldLookAt );
	vec4 mTranslatedPos = mLookAtTranslation * vec4( vReturn, 1.f );
	vReturn = vec3( mTranslatedPos );

	return vReturn;
}

// Handle logic for changing window size.
void Camera::updateHxW( int iHeight, int iWidth ) 
{ 
	m_iHeight = iHeight;
	m_iWidth = iWidth;
	m_fAspectRatio = (float)m_iWidth / (float)m_iHeight;
}

/// Camera Manipulation Functions
// Rotatable 360 degrees around.  Laps if it goes over that limit.
void Camera::orbit( float fDelta_X, float fDelta_Y )
{
	THETA += fDelta_X;
	THETA = THETA > HORIZONTAL_LIMIT ? THETA - HORIZONTAL_LIMIT : THETA;
	THETA = THETA < 0.f ? THETA + HORIZONTAL_LIMIT : THETA;

	PHI += fDelta_Y;
	PHI = PHI < VERT_LOWER_LIMIT ? VERT_LOWER_LIMIT : PHI;
	PHI = PHI > VERT_UPPER_LIMIT ? VERT_UPPER_LIMIT : PHI;

	//cout << "CAMERA ORBIT: {" << THETA << ", " << PHI << "}" << endl;
}

// Zoom in and out by a given Delta
void Camera::zoom( float fDelta )
{
	RADIUS -= fDelta;
	RADIUS = RADIUS < ZOOM_MIN ? ZOOM_MIN : RADIUS;
	RADIUS = RADIUS > ZOOM_MAX ? ZOOM_MAX : RADIUS;

	//cout << "CAMERA ZOOM: {" << RADIUS << "}" << endl;
}

void Camera::pan( float fDelta_X, float fDelta_Z )
{
	m_vWorldLookAt += vec3( ( fDelta_X/ RADIUS), 0.f, -(fDelta_Z / RADIUS) );
}
