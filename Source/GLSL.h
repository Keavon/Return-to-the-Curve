#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <cassert>
#include <glad/glad.h>
#include <string>


namespace GLSL
{
	using namespace std;
	
	void printOpenGLErrors(char const * const Function, char const * const File, int const Line);
	void checkError(const char *str = 0);
	void printProgramInfoLog(GLuint program);
	void printShaderInfoLog(GLuint shader);
	void checkVersion();
	GLint getAttribLocation(const GLuint program, const char varname[], bool verbose = true);
	GLint getUniformLocation(const GLuint program, const char varname[], bool verbose = true);
	void enableVertexAttribArray(const GLint handle);
	void disableVertexAttribArray(const GLint handle);
	void vertexAttribPointer(const GLint handle, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
}

#ifndef DISABLE_OPENGL_ERROR_CHECKS
#define CHECKED_GL_CALL(x) do { GLSL::printOpenGLErrors("{{BEFORE}} "#x, __FILE__, __LINE__); (x); GLSL::printOpenGLErrors(#x, __FILE__, __LINE__); } while (0)
#else
#define CHECKED_GL_CALL(x) (x)
#endif
