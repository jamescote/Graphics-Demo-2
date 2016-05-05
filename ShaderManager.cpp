#include "ShaderManager.h"

/////////////
// Defines //
/////////////

#define TO_WORLD_LOCATION			m_sA2ShaderVarLocations[TO_WORLD_MAT]
#define TO_CAMERA_LOCATION			m_sA2ShaderVarLocations[TO_CAM_MAT]
#define PERSPECTIVE_M_LOCATION		m_sA2ShaderVarLocations[PERSPECTIVE_MAT]
#define LOCAL_TRANSFORM_LOCATION	m_sA2ShaderVarLocations[LOCAL_TRANSFORM]
#define SPEC_COLOR_LOCATION			m_sA2ShaderVarLocations[SPEC_COLOR]
#define SPEC_EXP_LOCATION			m_sA2ShaderVarLocations[SPEC_EXP]
#define LIGHT_PIXEL_LOCATION		m_sA2ShaderVarLocations[B_LIGHT_PIXEL]
#define CAMERA_LOC_LOCATION			m_sA2ShaderVarLocations[CAM_LOCATION]

// Singleton Variable initialization
ShaderManager* ShaderManager::m_pInstance = NULL;

// Different Vertex Shaders required for each assignment
const string m_sVShaderNames[MAX_SHDRS] = {
	"vertex_A1.glsl",
	"vertex_A2.glsl"
};

// Different Fragment Shaders required for each assignment
const string m_sFShaderNames[MAX_SHDRS] = {
	"fragment_A1.glsl",
	"fragment_A2.glsl"
};

// Different Tessellation Control Shaders required for each assignment.
const string m_sTCShaderNames[MAX_SHDRS] = {
	"tess_Control_A1.glsl",
	"tess_Control_A2.glsl"
};

// Different Tessellation Control Shaders required for each assignment.
const string m_sTEShaderNames[MAX_SHDRS] = {
	"tess_Eval_A1.glsl",
	"tess_Eval_A2.glsl"
};

// Assignment 2 Shader Variable Names.
const GLchar* ShaderManager::c_sA2ShaderVarNames[MAX_A2_VARS] = { "mToWorld",
																  "mToCamera",
																  "mPerspective",
																  "mLocalTransform",
																  "pSpecColor",
																  "fSpecExp",
																  "bLightPixel",
																  "mCameraLocation" };

// Public - Not a singleton
// Designed mainly to manage different shaders between assignments.  
// Could make Assignment Classes that each have their own shader managers: TODO.
ShaderManager::ShaderManager()
{
	m_bInitialized = false;
}

// Get the Singleton ShaderManager Object.  Initialize it if NULL.
ShaderManager* ShaderManager::getInstance()
{
	if (NULL == m_pInstance)
		m_pInstance = new ShaderManager();

	return m_pInstance;
}

// Destructor - Kill any shaders that we've been using.
ShaderManager::~ShaderManager()
{
	// unbind any shader programs
	glUseProgram(0);
}

/*******************************************************************\
 * Set up Shaders												   *
\*******************************************************************/

// Inializes shaders. 
bool ShaderManager::initializeShaders()
{
	m_bInitialized = m_pShader[REGULAR].initializeShader(m_sVShaderNames[REGULAR], m_sFShaderNames[REGULAR]);
	m_bInitialized &= m_pShader[TEXTURE].initializeShader(m_sVShaderNames[TEXTURE], m_sFShaderNames[TEXTURE]);

	if ( m_bInitialized )
		for ( unsigned int i = 0; i < MAX_A2_VARS; i++ )
		{
			m_sA2ShaderVarLocations[i] = m_pShader[TEXTURE].fetchVarLocation( c_sA2ShaderVarNames[i] );
			if ( ERR_CODE == m_sA2ShaderVarLocations[i] )
				cout << "Error: Couldn't get location for \"" << c_sA2ShaderVarNames[i] << "\"." << endl;
		}

	// return False if not all Shaders Initialized Properly
	return m_bInitialized;
}

/*********************************************************************************************************************************\
 * Uniform Manipulation                                                                                                          *
\*********************************************************************************************************************************/

// Sets the To World Matrix, dependent on the Scene that is going to World.
void ShaderManager::setWorldMatrix( const mat4 &mToWorld )
{
	if ( ERR_CODE != TO_WORLD_LOCATION )
	{
		glUseProgram( m_pShader[TEXTURE].getProgram() );
		glUniformMatrix4fv( TO_WORLD_LOCATION, 1, GL_FALSE, value_ptr( mToWorld ) );
		glUseProgram(0);
	}
}

// Sets the To Camera Matrix, dependent on the Camera Position.
void ShaderManager::setCameraMatrix( const mat4 &mToCamera )
{
	if ( ERR_CODE != TO_CAMERA_LOCATION )
	{
		glUseProgram( m_pShader[TEXTURE].getProgram() );
		glUniformMatrix4fv( TO_CAMERA_LOCATION, 1, GL_FALSE, value_ptr( mToCamera ) );
		glUseProgram( 0 );
	}
}

// Sets the Perspective Matrix to convert from Camera Space to View Space.
void ShaderManager::setPerspective( const mat4 &mPerspective )
{
	if ( ERR_CODE != PERSPECTIVE_M_LOCATION )
	{
		glUseProgram( m_pShader[TEXTURE].getProgram() );
		glUniformMatrix4fv( PERSPECTIVE_M_LOCATION, 1, GL_FALSE, value_ptr( mPerspective ) );
		glUseProgram( 0 );
	}
}

// Sets the local transformation Matrix for Rotation and own Axis
void ShaderManager::setLocalTransform( const mat4 &mTransform )
{
	if ( ERR_CODE != LOCAL_TRANSFORM_LOCATION )
	{
		glUseProgram( m_pShader[TEXTURE].getProgram() );
		glUniformMatrix4fv( LOCAL_TRANSFORM_LOCATION, 1, GL_FALSE, value_ptr( mTransform ) );
		glUseProgram( 0 );
	}
}

// Set the Specular Color for an object
void ShaderManager::setSpecularColor( const vec3 &mSpecColor )
{
	if ( ERR_CODE != SPEC_COLOR_LOCATION )
	{
		glUseProgram( m_pShader[TEXTURE].getProgram() );
		glUniform3f( SPEC_COLOR_LOCATION, mSpecColor.r, mSpecColor.g, mSpecColor.b );
		glUseProgram( 0 );
	}
}

// Set the Specular Exponent for the Object
void ShaderManager::setSpecularExp( float fSpecExp )
{
	if ( ERR_CODE != SPEC_EXP_LOCATION )
	{
		glUseProgram( m_pShader[TEXTURE].getProgram() );
		glUniform1f( SPEC_EXP_LOCATION, fSpecExp );
		glUseProgram( 0 );
	}
}

// Set the Lighting Boolean to Light the Object
void ShaderManager::setLightBool( bool bLight )
{
	if ( ERR_CODE != LIGHT_PIXEL_LOCATION )
	{
		glUseProgram( m_pShader[TEXTURE].getProgram() );
		glUniform1i( LIGHT_PIXEL_LOCATION, bLight );
		glUseProgram( 0 );
	}
}

// Set the Location of the Camera in World Coordinates.
void ShaderManager::setCameraLocation( const vec3 &mCamPos )
{
	if ( ERR_CODE != CAMERA_LOC_LOCATION )
	{
		glUseProgram( m_pShader[TEXTURE].getProgram() );
		glUniform3f( CAMERA_LOC_LOCATION, mCamPos.x, mCamPos.y, mCamPos.z );
		glUseProgram( 0 );
	}
}
