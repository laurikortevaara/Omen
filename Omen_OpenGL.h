#ifndef _OMEN_OPENGL_H_
#define _OMEN_OPENGL_H_

#ifdef WIN32
	#include <Windows.h>
	#include <GL/glew.h>	
	#include <GLFW/glfw3.h>
#elif __APPLE__
	#define GL3_PROTOTYPES
	#include <OpenGL/gl3.h>
#else
	#include <GL/gl.h>
#endif

#endif // _OMEN_OPENGL_H_