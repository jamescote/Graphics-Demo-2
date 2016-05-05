#include "GeometryManager.h"

// these vertex attribute indices correspond to those specified for the
// input variables in the vertex shader
const GLuint VERTEX_INDEX = 0;
const GLuint COLOUR_INDEX = 1;
const GLuint TEXTURE_INDEX = 2;

// Assignment 3 - Patch Point Count
#define NUM_CONTROL_POINTS 4

// Singleton static setup
GeometryManager* GeometryManager::m_pInstance = NULL;

// Constructor
GeometryManager::GeometryManager()
{
	m_bInitialized = false;
}

// Singleton getter.
GeometryManager* GeometryManager::getInstance()
{
	if (NULL == m_pInstance)
		m_pInstance = new GeometryManager();

	return m_pInstance;
}

// Destructor
GeometryManager::~GeometryManager()
{
	glBindVertexArray(0);

	// unbind and destroy our vertex array object and associated buffers
	glDeleteVertexArrays(1, &m_pGeometry.vertexArray);
	glDeleteBuffers(1, &m_pGeometry.vertexBuffer);
	glDeleteBuffers(1, &m_pGeometry.colourBuffer);
	glDeleteBuffers(1, &m_pGeometry.textureBuffer);
}

// Initializes Geometry, currently from boilerplate code
// Will need to revisit and improve upon.
bool GeometryManager::initializeGeometry()
{
	if (!m_bInitialized)
	{
		// create an array buffer object for storing our vertices
		glGenBuffers(1, &m_pGeometry.vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_pGeometry.vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, MAX_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

		// create another one for storing our colours
		glGenBuffers(1, &m_pGeometry.colourBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_pGeometry.colourBuffer);
		glBufferData(GL_ARRAY_BUFFER, MAX_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

		// create a vertex array object encapsulating all our vertex attributes
		glGenVertexArrays(1, &m_pGeometry.vertexArray);
		glBindVertexArray(m_pGeometry.vertexArray);

		// associate the position array with the vertex array object
		glBindBuffer(GL_ARRAY_BUFFER, m_pGeometry.vertexBuffer);
		glVertexAttribPointer(VERTEX_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(VERTEX_INDEX);

		// assocaite the colour array with the vertex array object
		glBindBuffer(GL_ARRAY_BUFFER, m_pGeometry.colourBuffer);
		glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_INDEX);

		//-----------------
		// Set up vertex attribute info for textures
		//-----------------
		glBindVertexArray(m_pGeometry.vertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, m_pGeometry.textureBuffer);
		glVertexAttribPointer(TEXTURE_INDEX, 2, GL_UNSIGNED_INT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_INDEX);

		// unbind our buffers, resetting to default state
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Set to Initialized
		m_bInitialized = !CheckGLErrors();
	}
	// check for OpenGL errors and return false if error occurred
	return !m_bInitialized;
}

// Bind a texture to the texture buffer of a given Assignment.
// Size of the data and the data are passed in.
void GeometryManager::bindTextureData(GLsizeiptr iPtr, void* data)
{
	glGenBuffers(1, &m_pGeometry.textureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_pGeometry.textureBuffer);
	glBufferData(GL_ARRAY_BUFFER, iPtr, data, GL_DYNAMIC_DRAW);
}
