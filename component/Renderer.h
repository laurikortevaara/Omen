#pragma once

#include "../system/GraphicsSystem.h"
#include "../Engine.h"
#include "Component.h"

namespace omen {
	namespace ecs {
		class Renderer : public Component {
		protected:
			virtual void onAttach(ecs::Entity* e) = 0;
			virtual void onDetach(ecs::Entity* e) = 0;
		public:
			virtual void render() = 0;

			Renderer& setShader(std::unique_ptr<Shader> shader) { m_shader = std::move(shader); return *this; }
			Shader* shader() const { return m_shader.get(); }

			Renderer& setTexture(std::unique_ptr<Texture> texture) { m_texture = std::move(texture); return *this; }
			Texture* texture() const { return m_texture.get(); }
		public:
		protected:
			Renderer() : m_vao(0),
				m_vbo(0),
				Component() {
				GraphicsSystem *gs = omen::Engine::instance()->findSystem<GraphicsSystem>();
				assert(gs != nullptr);
				gs->add(this);
				int a = 1;
			};
		protected:
			GLuint m_vao;
			GLuint m_vbo;
			GLuint m_vbo_texture;
			GLuint m_vbo_normals;
			std::unique_ptr<Shader> m_shader;
			std::unique_ptr<Texture> m_texture;
		private:
		};
	}
}