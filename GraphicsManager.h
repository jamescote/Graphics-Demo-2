#pragma once

/* INCLUDES */
#include "stdafx.h"
#include "Camera.h"
#include "Planet.h"

/* DEFINES */
#define NUM_PLANETS 4

// Forward Declarations
class ShaderManager;
class GeometryManager;
class SceneGraph;

// Class: Graphics Manager
// Purpose: Acts as the Sinew between all moving parts that are required for drawing
//			with openGL.
// Written by: James Coté
class GraphicsManager
{
public:
	static GraphicsManager* getInstance(GLFWwindow *rWindow);
	~GraphicsManager();

	// Graphics Application
	bool initializeGraphics();
	bool renderGraphics();
	void toggleAnimation();
	void toggleFastForward();

	/// HxW Settings
	void resizedWindow( int iHeight, int iWidth ) { m_pCamera->updateHxW( iHeight, iWidth ); };

	/// Camera Manipulation
	void panCamera( float fX, float fY, vec2& vPrevPos );
	void orbitCamera( float fX, float fY, vec2& vPrevPos );
	void zoomCamera( float fZoomVal );

private:
	// For Singleton Implementation
	GraphicsManager(GLFWwindow* rWindow); 
	static GraphicsManager* m_pInstance;

	// Window Reference
	GLFWwindow* m_pWindow;
	
	// List of Planets for management.
	Planet* m_pPlanets[NUM_PLANETS];
	SceneGraph* m_pSceneGraph;
	Transformation* m_pTransformations[NUM_PLANETS];

	// Camera Object
	Camera* m_pCamera;

	// Render Functions
	void RenderScene();

	// Manages Shaders for all assignments
	ShaderManager* m_pShaderMngr;
	GeometryManager* m_pGeometryMngr;

	bool initializeGeometry(); // Bind Textures
};

