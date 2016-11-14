//
// Created by Lauri Kortevaara(Intopalo) on 12/01/16.
//

#ifndef OMEN_RENDERABLE_H
#define OMEN_RENDERABLE_H

#include <vector>
#include "Shader.h"
#include "Object.h"

namespace omen {
    class Renderable : public omen::Object {
    public:
        virtual void render() = 0;
		virtual ~Renderable();

		void setPos(const glm::vec2& pos) { m_pos = pos; }
		glm::vec2 pos() const { return m_pos; }
    protected:
        Renderable(glm::vec3 pos, float width, float height, float depth = 0.0f);
		
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



#endif //OMEN_RENDERABLE_H
