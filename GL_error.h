//
// Created by Lauri Kortevaara on 08/11/15.
//

#ifndef OMEN_GL_ERROR_H_H
#define OMEN_GL_ERROR_H_H

#include "Omen_OpenGL.h"
#include <iostream>
#include <signal.h>

#ifdef A_DEBUG
inline void check_gl_error(){ \
    GLenum error = glGetError();\
    if(error!=GL_NO_ERROR){\
        std::cout << "Error at: " << __FUNCTION__ << ", ErrorCode: " << error << std::endl;\
        \
    }}
#else
inline void check_gl_error(){}
#endif

#endif //OMEN_GL_ERROR_H_H
