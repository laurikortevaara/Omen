//
// Created by Lauri Kortevaara(Intopalo) on 12/01/16.
//

#ifndef OMEN_RENDERABLE_H
#define OMEN_RENDERABLE_H

#include <vector>
#include "../Shader.h"
#include "Component.h"

namespace omen {
    namespace ecs {
        class Renderable : public ecs::Component {
        protected:
            virtual void onAttach(ecs::Entity* e);
            virtual void onDetach(ecs::Entity* e);
        public:
            virtual void render() = 0;

        protected:
            Renderable(glm::vec3 pos, float width, float height, float depth = 0.0f);

            virtual ~Renderable();

            void setShader(Shader* shader) {m_shader = shader;}
            void setTexture(Texture* texture) {m_texture = texture;}

            Shader* shader() {return m_shader;}
            Texture* texture() {return m_texture;}

            virtual void initializeShader() = 0;
            virtual void initializeTexture() = 0;

            std::vector<GLshort> m_indices;
            GLuint m_vao;
            GLuint m_vbo;
            GLuint m_vbo_texcoord;
            GLuint m_ibo;

            float m_width;
            float m_height;
            glm::vec2 m_pos;
            glm::vec2 m_pivot;
        private:
            Shader *m_shader;
            Texture *m_texture;
        };
    }
}


#endif //OMEN_RENDERABLE_H
