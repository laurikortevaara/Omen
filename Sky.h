//
// Created by Lauri Kortevaara(Intopalo) on 11/01/16.
//

#ifndef OMEN_SKY_H
#define OMEN_SKY_H

#include <glfw/glfw3.h>
#include <OpenGL/OpenGL.h>
#include <vector>
#include "Shader.h"
#include "component/Renderable.h"

namespace omen {
    class Sky : public ecs::Renderable {
    public:
        Sky();
        virtual ~Sky();

        virtual void render();

    private:
        std::vector<GLshort> m_indices;
        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_ibo;
        Shader *m_shader;
    };
} // namespace omen

#endif //OMEN_SKY_H
