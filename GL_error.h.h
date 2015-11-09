//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#ifndef OMEN_GL_ERROR_H_H
#define OMEN_GL_ERROR_H_H

#include <OpenGL/gl3.h>
#include <iostream>
#include <signal.h>

inline void check_gl_error(){ \
    GLenum error = glGetError();\
    if(error!=GL_NO_ERROR){\
        std::cout << "Error at: " << __PRETTY_FUNCTION__ << ", ErrorCode: " << error << std::endl;\
        raise(SIGTRAP);\
    }}

#endif //OMEN_GL_ERROR_H_H
