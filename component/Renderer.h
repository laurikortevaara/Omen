#pragma once

#include "../system/GraphicsSystem.h"
#include "../system/OpenVRSystem.h"
#include "../Engine.h"
#include "Component.h"

namespace omen {
	class Shader;

	namespace ecs {
		class Renderer : public Component {
		protected:
			virtual void onAttach(ecs::Entity* e) = 0;
			virtual void onDetach(ecs::Entity* e) = 0;
		public:
			virtual void render(omen::Shader* shader = nullptr) const = 0;

			Renderer& setShader(std::unique_ptr<Shader> shader) { m_shader = std::move(shader); return *this; }
			Shader* shader() const { return m_shader.get(); }

			Renderer& setTexture(std::unique_ptr<Texture> texture) { m_texture = std::move(texture); return *this; }
			Texture* texture() const { return m_texture.get(); }
		public:
			void storePolygonMode() const;
			void polygonModeFill() const;
			void polygonModeWireframe() const;
			void restorePolygonMode() const;
		protected:
			Renderer();
		protected:
			GLuint m_vao;
			GLuint m_indexBufferSize;
			GLuint m_indexBuffer;
			GLuint m_vbo;
			GLuint m_vbo_texture;
			GLuint m_vbo_normals;
			GLuint m_vbo_tangents;
			GLuint m_vbo_bitangents;
			GLuint m_vbo_barycentric;
			std::unique_ptr<Shader> m_shader;
			std::unique_ptr<Texture> m_texture;
		private:
		};
	}
}