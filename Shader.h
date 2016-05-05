#pragma once

#include "stdafx.h"

class Shader
{
public:
	Shader();
	~Shader();

	bool initializeShader(string sVSLocation, string sFSLocation);
	bool initializeShader(string sVSLocation, string sFSLocation,
						  string sTCLocation, string sTELocation);

	// Getters, Setters not required
	GLuint getProgram() { return m_uiProgram; }
	GLint fetchVarLocation(const GLchar* sVarName) { return glGetUniformLocation(m_uiProgram, sVarName); }

protected:
	// Shader Variables
	GLuint  m_uiVertex;
	GLuint  m_uiFragment;
	GLuint  m_uiProgram;
	GLuint  m_uiTessCntrl;
	GLuint  m_uiTessEval;
	bool	m_bInitialized;

	// Protected Functions for Setting up Shaders
	string LoadSource(const string &filename);
	GLuint CompileShader(GLenum shaderType, const string &source);
	GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader, GLuint controlShader, GLuint evalShader);
};

