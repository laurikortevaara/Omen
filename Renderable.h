//
// Created by Lauri Kortevaara(Intopalo) on 12/01/16.
//

#ifndef OMEN_RENDERABLE_H
#define OMEN_RENDERABLE_H

#include <vector>
#include "Shader.h"

namespace Omen {
    class Renderable {
    public:
        Shader* m_shader;
        virtual void render() = 0;
    protected:
        std::vector<GLuint> m_indices;
        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_ibo;
    };
}


#endif //OMEN_RENDERABLE_H
