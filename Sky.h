//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#ifndef OMEN_SKY_H
#define OMEN_SKY_H

#include <glfw/glfw3.h>
#include <OpenGL/OpenGL.h>
#include <vector>
#include "Shader.h"

namespace Omen {
    class Sky {
    public:
        Sky();
        virtual ~Sky();

        void render();

    private:
        std::vector<GLshort> m_indices;
        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_ibo;
        Shader *m_shader;
    };
} // namespace Omen

#endif //OMEN_SKY_H
