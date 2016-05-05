#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "Shader.h"

// Enum for Shaders
enum eShaderType
{
	REGULAR = 0,
	TEXTURE,
	MAX_SHDRS
};

// Class: Shader Manager
// Purpose: Manages all the Shaders used by every Assignment.  Manages Uniform variable
//			manipulation and properly initializes and destroys created Shaders.
// Current Issues: Uniform variables hard-coded.
// TODO: (Low Priority) Scan through all shader code for uniform variables and set up
//						management of those variables dynamically.  Too much work...
// Written by: James Coté
class ShaderManager
{
public:
	static ShaderManager* getInstance();
	~ShaderManager();

	// Initialize Shaders
	bool initializeShaders();

	// Get the specified program for using shaders for rendering
	GLuint getProgram(eShaderType eType) { return m_pShader[eType].getProgram(); }
	
	// For resetting all shaders to default values
	void resetAllShaders();

	// Setting Uniforms
	void setWorldMatrix(  const mat4 &mToWorld );
	void setCameraMatrix( const mat4 &mToCamera );
	void setPerspective( const mat4 &mPerspective );
	void setLocalTransform( const mat4 &mTransform );
	void setSpecularColor( const vec3 &mSpecColor );
	void setSpecularExp( float fSpecExp );
	void setLightBool( bool bLight );
	void setCameraLocation( const vec3 &mCamPos );

private:
	// Singleton Implementation
	ShaderManager();
	static ShaderManager* m_pInstance;

	// Should only be initialized once.
	bool m_bInitialized;

	// Shader Variables
	Shader m_pShader[MAX_SHDRS];

	// Enum for different Uniform Shader Variables.  Hard-coded for now
	// TODO: load from file. Low Priority
	enum eShaderVars
	{
		TO_WORLD_MAT = 0,
		TO_CAM_MAT,
		PERSPECTIVE_MAT,
		LOCAL_TRANSFORM,
		// Lighting Variables
		SPEC_COLOR,
		SPEC_EXP,
		B_LIGHT_PIXEL,
		CAM_LOCATION,
		MAX_A2_VARS 
	};

	// Store Uniform Shader Variable Locations for ASSG_2
	GLint m_sA2ShaderVarLocations[MAX_A2_VARS];
	static const GLchar* c_sA2ShaderVarNames[MAX_A2_VARS];
};

