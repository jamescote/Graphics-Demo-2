#pragma once

#include "stdafx.h"

// Geometry Structure
struct MyGeometry
{
	// OpenGL names for array buffer objects, vertex array object
	GLuint  vertexBuffer;
	GLuint  colourBuffer;
	GLuint	textureBuffer;
	GLuint  vertexArray;


	// initialize object names to zero (OpenGL reserved value)
	MyGeometry() : vertexBuffer( 0 ), colourBuffer( 0 ), vertexArray( 0 )
	{
	}
};

// Definitions
#define MAX_BUFFER_SIZE		100000

// Class: GeometryManager
// Purpose: Manages a geometry structure for each Assignment.  Ensures proper setup of
//			each VAO for requirements of the Assignment.  
// TODO: Store textures in GeometryManager and be able to hand out binding numbers to associate
//			Textures to desired classes.
// Written By: James Coté
class GeometryManager
{
public:
	static GeometryManager* getInstance();
	~GeometryManager();

	bool initializeGeometry();
	void bindTextureData(GLsizeiptr iPtr, void* data);

	// To get a Geometry Pointer, unable to be modified
	MyGeometry const* getGeometry() { return &m_pGeometry; }
private:
	// Singleton Implementation
	GeometryManager();
	static GeometryManager* m_pInstance;

	bool m_bInitialized;
	MyGeometry m_pGeometry;	
};

