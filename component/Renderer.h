#pragma once

#include "../system/GraphicsSystem.h"
#include "../Engine.h"
#include "Component.h"

namespace omen {
	namespace ecs {
		class Renderer : public Component {
		protected:
			virtual void onAttach(ecs::Entity* e);
			virtual void onDetach(ecs::Entity* e);
		public:
			virtual void render() = 0;

			Renderer& setShader(std::shared_ptr<Shader> shader) { m_shader = shader; return *this; }
			std::shared_ptr<Shader> shader() const { return m_shader; }

			Renderer& setTexture(std::shared_ptr<Texture> texture) { m_texture = texture; return *this; }
			std::shared_ptr<Texture> texture() const { return m_texture; }
		public:
		protected:
			Renderer() : Component() {
				GraphicsSystem *gs = omen::Engine::instance()->findSystem<GraphicsSystem>();
				assert(gs != nullptr);
				gs->add(this);
			};
		protected:
			GLuint m_vao;
			GLuint m_vbo;
			std::shared_ptr<Shader> m_shader;
			std::shared_ptr<Texture> m_texture;
		private:
		};
	}
}