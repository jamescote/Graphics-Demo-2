//////////////
// Includes //
//////////////
#include "stdafx.h"
#include "ShaderManager.h"
#include "Mouse_Handler.h"

#ifdef USING_LINUX
#include <Magick++.h>
#endif

/* DEFINES */
#define START_HEIGHT	1024
#define START_WIDTH		1024

// Function Prototypes
void ErrorCallback( int error, const char* description );
void KeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
void WindowResizeCallback( GLFWwindow* window, int iWidth, int iHeight );
void mouseMovecallback( GLFWwindow* window, double x, double y );
void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
void mouseScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
bool initializeWindow( GLFWwindow** rWindow, int iHeight, int iWidth, const char* cTitle );

//
// Entry for Program
int main()
{
	int iRunning = glfwInit();
	GLFWwindow*		m_Window = 0;
	ShaderManager*	m_ShdrMngr = 0;
	Mouse_Handler*	m_MseHndlr = 0;
	GraphicsManager* m_GpxMngr = 0;
	clock_t mStartTime, mTotalTime = 0;
	vector<clock_t> mAvgTime;

	if ( 0 == iRunning )
		cout << "Error: GLFW failed to initialize, ending program." << endl;
	else	// Run Program
	{
#ifdef USING_LINUX
		// Initialze Magick - LINUX only
		Magick::InitializeMagick( "" );
#endif

		// Set Error Callback
		glfwSetErrorCallback( ErrorCallback );
		iRunning = initializeWindow( &m_Window, START_HEIGHT, START_WIDTH, "CPSC 453 - Graphics" );

#ifdef USING_WINDOWS
		// Initialize GLAD Windows
		gladLoadGL();
#endif

		// Bind window to graphics Manager
		if ( 1 == iRunning )
			m_GpxMngr = GraphicsManager::getInstance( m_Window );

		// Initialize the Mouse Handler.
		m_MseHndlr = Mouse_Handler::getInstance( m_Window );

		// Initialize Graphics
		iRunning = !m_GpxMngr->initializeGraphics();

		// Main loop
		while ( iRunning )
		{
			mStartTime = clock();
			// do Graphics Loop
			iRunning = m_GpxMngr->renderGraphics();
			mAvgTime.push_back( clock() - mStartTime );
		}
		
		for ( unsigned int i = 0; i < mAvgTime.size(); ++i )
			mTotalTime += mAvgTime[i];

		mAvgTime.clear();

		if ( mAvgTime.size() > 0 )
			mTotalTime /= mAvgTime.size();

		cout << "Average Render Time = " << ((float)mTotalTime) / CLOCKS_PER_SEC << "secs." << endl;

		// Clean up!
		if ( m_GpxMngr != NULL )
		{
			mStartTime = clock();
			delete m_GpxMngr;
			mTotalTime = clock() - mStartTime;
			cout << "Deleted Graphics Manager in " << ((float)mTotalTime) / CLOCKS_PER_SEC << "secs." << endl;
		}

		if ( m_MseHndlr != NULL )
		{
			mStartTime = clock();
			delete m_MseHndlr;
			mTotalTime = clock() - mStartTime;
			cout << "Deleted Mouse Handler in " << ((float)mTotalTime) / CLOCKS_PER_SEC << "secs." << endl;
		}

		mStartTime = clock();
		glfwDestroyWindow( m_Window );
		mTotalTime = clock() - mStartTime;
		cout << "Destroyed Window in " << ((float)mTotalTime) / CLOCKS_PER_SEC << "secs." << endl;
	}

	glfwTerminate();

	cout << "Finished Program, au revoir!" << endl;

	return 0;

}


// For reporting GLFW errors
void ErrorCallback( int error, const char* description )
{
	cout << "GLFW Error : " << error << ":" << endl;
	cout << description << endl;
}

bool initializeWindow( GLFWwindow** rWindow, int iHeight, int iWidth, const char* cTitle )
{
	bool bSuccess = true;

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
	(*rWindow) = glfwCreateWindow( iWidth, iHeight, cTitle, 0, 0 );

	if ( !*rWindow )
	{
		cout << "Program failed to create GLFW window" << endl;
		bSuccess = false;
	}
	else
	{
		glfwSetKeyCallback( *rWindow, KeyCallback );
		glfwSetWindowSizeCallback( *rWindow, WindowResizeCallback );
		glfwSetMouseButtonCallback( *rWindow, mouseButtonCallback );
		glfwSetCursorPosCallback( *rWindow, mouseMovecallback );
		glfwSetScrollCallback( *rWindow, mouseScrollCallback );
		glfwWindowHint( GLFW_SAMPLES, 4 );
		glfwMakeContextCurrent( *rWindow );
	}

	return bSuccess;
}

// handles keyboard input events
void KeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	ShaderManager* pShdrMngr = ShaderManager::getInstance();
	GraphicsManager* pGrphxMngr = GraphicsManager::getInstance(window);

	if ( GLFW_PRESS == action )
	{
		switch ( key )
		{
			case (GLFW_KEY_ESCAPE) :											// Exit
				glfwSetWindowShouldClose( window, GL_TRUE );
				break;
			case (GLFW_KEY_SPACE) :
				pGrphxMngr->toggleAnimation();
				break;
			case (GLFW_KEY_F) :
				pGrphxMngr->toggleFastForward();
				break;
		}
	}
	else if ( GLFW_RELEASE == action )
	{
		if ( GLFW_KEY_F == key )
			pGrphxMngr->toggleFastForward();
	}
}

// handles Window Resize events
void WindowResizeCallback( GLFWwindow* window, int iWidth, int iHeight )
{
	glViewport( 0, 0, iWidth, iHeight );
	GraphicsManager::getInstance( window )->resizedWindow( iHeight, iWidth );
}

// Mouse Button Callback
void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
{
	// TODO - From Assgs 1-4, need update.
	Mouse_Handler* mMouseHndlr = Mouse_Handler::getInstance( window );
	double fX, fY;

	if ( GLFW_MOUSE_BUTTON_1 == button )
	{
		glfwGetCursorPos( window, &fX, &fY );
		if ( GLFW_PRESS == action )
			mMouseHndlr->mouseTStart();
		else if ( GLFW_RELEASE == action )
			mMouseHndlr->mouseTEnd();
	}
	if ( GLFW_MOUSE_BUTTON_2 == button )
	{
		glfwGetCursorPos( window, &fX, &fY );
		if ( GLFW_PRESS == action )
			mMouseHndlr->mouseRStart();
		else if ( GLFW_RELEASE == action )
			mMouseHndlr->mouseREnd();
	}
}

// Handles input from Mouse Moves.
void mouseMovecallback( GLFWwindow* window, double x, double y )
{
	Mouse_Handler* mMouseHndlr = Mouse_Handler::getInstance( window );

	mMouseHndlr->updateMouse( (float)x, (float)y );
}

// Handle scroll wheel callbacks
void mouseScrollCallback( GLFWwindow* window, double xoffset, double yoffset )
{
	GraphicsManager* pGPXMngr = GraphicsManager::getInstance( window );

	pGPXMngr->zoomCamera( (float)yoffset );
}
