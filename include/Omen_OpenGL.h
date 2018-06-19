#ifndef _OMEN_OPENGL_H_
#define _OMEN_OPENGL_H_

#define GL_GLEXT_PROTOTYPES

#include "GL_error.h"

#ifdef WIN32
	#include <Windows.h>
	#include <GL/glew.h>	
	#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

#elif __APPLE__
	#define GL3_PROTOTYPES
	#include <OpenGL/gl3.h>
#else
	#include <GL/gl.h>
#endif

#endif // _OMEN_OPENGL_H_