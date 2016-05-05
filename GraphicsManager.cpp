#include "GraphicsManager.h"
#include "ShaderManager.h"
#include "GeometryManager.h"
#include "SceneGraph.h"

// Planet Indices
#define SUN 0
#define EARTH 1
#define MOON 2
#define STARS 3

#define SUN_EARTH_DIST	log(149597890.f) / LOG_BASE
#define EARTH_MOON_DIST log(384399.f)	 / LOG_BASE

// Singleton Variable initialization
GraphicsManager* GraphicsManager::m_pInstance = NULL;

// Constructor - Private, only accessable within the Graphics Manager
GraphicsManager::GraphicsManager(GLFWwindow* rWindow)
{
	// Initialize and Get Shader and Geometry Managers
	m_pShaderMngr	= ShaderManager::getInstance();
	m_pGeometryMngr = GeometryManager::getInstance();

	m_pWindow = rWindow;
	int iHeight, iWidth;
	glfwGetWindowSize(m_pWindow, &iWidth, &iHeight);

	// Initialize SceneGraph and Build Graph
	float fSunRadius = 8;
	float fScaledEarthRadius = log( 6378.1f ) / LOG_BASE;
	float fScaledMoonRadius = log( 1737.1f ) / LOG_BASE;
	float fScaledSunRadius = log( 696000.f ) / LOG_BASE;
	float fSunEarthRatio = fScaledSunRadius / fScaledEarthRadius;
	float fEarthMoonRatio = fScaledEarthRadius / fScaledMoonRadius;

	float fEarthRadius = fScaledSunRadius * fSunEarthRatio;
	float fMoonRadius = fScaledEarthRadius * fEarthMoonRatio;

	float fSun_EarthDist = fSunRadius + (fSunRadius * log( 149597890 ) / LOG_BASE);
	float fEarth_MoonDist = fEarthRadius + (fEarthRadius * log( 384399 ) / LOG_BASE);
	float fMoonYOffset = sin( log( 23.5f ) / LOG_BASE ) * fEarth_MoonDist;
	m_pSceneGraph = new SceneGraph();
	m_pTransformations[SUN]		= new Transformation( vec3(0,0,0) );
	m_pTransformations[EARTH]	= new Transformation( vec3( fSun_EarthDist, 0.f, 0.f ) );
	m_pTransformations[MOON]	= new Transformation( vec3( fEarth_MoonDist, fMoonYOffset, 0.f ) );
	m_pSceneGraph->addTransformation( m_pTransformations[SUN], NULL );
	m_pSceneGraph->addTransformation( m_pTransformations[EARTH], m_pTransformations[SUN] );
	m_pSceneGraph->addTransformation( m_pTransformations[MOON], m_pTransformations[EARTH] );
	
	// Initialize Planets
	m_pPlanets[SUN]		= new Planet( fSunRadius, "texture_sun.jpg", m_pTransformations[SUN], log(7.25f) / LOG_BASE, 25.38f, 0.f, false );
	m_pPlanets[STARS]	= new Planet( ZOOM_MAX, "texture_stars.jpg", m_pTransformations[SUN], log(180.f) / LOG_BASE, 0.f, 0.f, false );
	m_pPlanets[EARTH] = new Planet( fEarthRadius, "earth_surface.jpg", m_pTransformations[EARTH], log( 23.4f ) / LOG_BASE, 0.9972698, 365.f, true );
	m_pPlanets[MOON] = new Planet( fMoonRadius, "texture_moon.jpg", m_pTransformations[MOON], log( 6.68f ) / LOG_BASE, 27.321582f, 27.321582f, true );
	m_pCamera = new Camera( iHeight, iWidth );
}

// Singleton Implementations
// Requires Window to initialize 
GraphicsManager* GraphicsManager::getInstance(GLFWwindow *rWindow)
{
	if (NULL == m_pInstance)
		m_pInstance = new GraphicsManager(rWindow);

	return m_pInstance;
}

// Destruct Shaders, Buffers, Arrays and other GL stuff.
GraphicsManager::~GraphicsManager()
{
	// Destruct Assg 1
	for ( int i = 0; i < NUM_PLANETS; ++i )
	{
		if ( NULL != m_pPlanets[i] )
			delete m_pPlanets[i];
		
		if ( NULL != m_pTransformations[i] )
			delete m_pTransformations[i];
	}	

	// Let go of Window Handle
	m_pWindow = NULL;

	// Gather up and clean up Managers
	if ( NULL != m_pShaderMngr )
		delete m_pShaderMngr;

	if ( NULL != m_pGeometryMngr )
		delete m_pGeometryMngr;

	if ( NULL != m_pCamera )
		delete m_pCamera;

	if ( NULL != m_pSceneGraph )
		delete m_pSceneGraph;
}

// Intended to be called every cycle, or when the graphics need to be updated
bool GraphicsManager::renderGraphics()
{
	// call function to draw our scene
	RenderScene();

	// scene is rendered to the back buffer, so swap to front for display
	glfwSwapBuffers(m_pWindow);

	// check for Window events
	glfwPollEvents();

	return !glfwWindowShouldClose(m_pWindow);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer
// Copied from Boilercode Program
// Will be replaced with functions in Graphic objects.
void GraphicsManager::RenderScene()
{
	// clear screen to a dark grey colour
	glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	glClear( GL_DEPTH_BUFFER_BIT );			// Clear Depth Buffer for awesomeness!

	m_pShaderMngr->setCameraMatrix( m_pCamera->getToCameraMat() );
	m_pShaderMngr->setPerspective( m_pCamera->getPerspectiveMat() );
	m_pShaderMngr->setCameraLocation( m_pCamera->getCameraWorldPos() );
	m_pShaderMngr->setSpecularColor( vec3( 1.0f, 1.0f, 1.0f ) );
	m_pShaderMngr->setSpecularExp( 65.f );

	for ( int i = 0; i < NUM_PLANETS; ++i )
		m_pPlanets[i]->renderPlanet();
}

// Function initializes shaders and geometry.
// contains any initializion requirements in order to start drawing.
bool GraphicsManager::initializeGraphics()
{
	// Locals
	bool bError = false;

	// Shaders
	if (!m_pShaderMngr->initializeShaders())
	{
		cout 
			<< "Couldn't initialize shaders." << endl;
		bError = true;
	}

	// Geometry
	if (initializeGeometry())
	{
		cout << "Couldn't initialize Geometry." << endl;
		bError = true;
	}

	// OpenGL's Z-buffer algorithm for hidden surface removal.
	glEnable( GL_DEPTH_TEST );
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	return bError; 
}

// Initializes Geometry, currently from boilerplate code
// Will need to revisit and improve upon.
bool GraphicsManager::initializeGeometry()
{
	GLsizeiptr iPtr;
	void* data;
	
	m_pPlanets[0]->getTextureData(&iPtr, &data);
	m_pGeometryMngr->bindTextureData(iPtr, data);

	return m_pGeometryMngr->initializeGeometry();
}

/*************************************************************\
 * Camera Manipulation                                       *
\*************************************************************/

// Translate the Camera's View location
void GraphicsManager::panCamera( float fX, float fY, vec2& vPrevPos )
{
	vec2 vDelta = vec2( fX - vPrevPos.x, vPrevPos.y - fY);
	m_pCamera->pan( vDelta[0], vDelta[1] );
}

// Orbit the Camera around it's view location
void GraphicsManager::orbitCamera( float fX, float fY, vec2& vPrevPos )
{
	vec2 vDelta = vec2( fX, fY ) - vPrevPos;
	m_pCamera->orbit( vDelta[0], vDelta[1] );
}

// Adjust Camera's Spherical Radius from view position.
void GraphicsManager::zoomCamera( float fZoomVal )
{
	m_pCamera->zoom( fZoomVal );
}

// Animation Controls
void GraphicsManager::toggleAnimation()
{
	for ( int i = 0; i < NUM_PLANETS; ++i )
		m_pPlanets[i]->toggleAnimation();
}

// Animation Controls
void GraphicsManager::toggleFastForward()
{
	for ( int i = 0; i < NUM_PLANETS; ++i )
		m_pPlanets[i]->toggleFastForward();
}


