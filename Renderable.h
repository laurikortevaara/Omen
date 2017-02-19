//
// Created by Lauri Kortevaara(Intopalo) on 12/01/16.
//

#ifndef OMEN_RENDERABLE_H
#define OMEN_RENDERABLE_H

#include <vector>
#include "Shader.h"
#include "Object.h"

namespace omen {
	namespace ecs { class Renderer; }
    class Renderable : public omen::Object {
    public:
        virtual void render() = 0;
		virtual ~Renderable();

		void setPos(const glm::vec2& pos) { m_pos = pos; }
		glm::vec2 pos() const { return m_pos; }

		float width() const { return m_width; }
		float height() const { return m_height; }
		glm::vec2 size() const { return glm::vec2(m_width, m_height); }

		glm::vec2 pivot() const { return m_pivot; }
		void setPivot(const glm::vec2& pivot) { m_pivot = pivot; }

		Shader* shader() { return m_shader; }
		Texture* texture() { return m_texture; }

		void setRenderer(omen::ecs::Renderer* renderer) { m_renderer = renderer; }
		omen::ecs::Renderer* renderer() const { return m_renderer; }

    protected:
        Renderable(glm::vec3 pos, float width, float height, float depth = 0.0f);
		
        void setShader(Shader* shader) {m_shader = shader;}
        void setTexture(Texture* texture) {m_texture = texture;}


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
        Shader* m_shader;
        Texture* m_texture;
		omen::ecs::Renderer* m_renderer;
    };
}



#endif //OMEN_RENDERABLE_H
