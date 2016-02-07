//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_GL_ERROR_H_H
#define OMEN_GL_ERROR_H_H

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
#include <iostream>
#include <signal.h>

#ifdef __DEBUG
inline void check_gl_error(){ \
    GLenum error = glGetError();\
    if(error!=GL_NO_ERROR){\
        std::cout << "Error at: " << __PRETTY_FUNCTION__ << ", ErrorCode: " << error << std::endl;\
        \
    }}
#else
inline void check_gl_error(){}
#endif

#endif //OMEN_GL_ERROR_H_H
