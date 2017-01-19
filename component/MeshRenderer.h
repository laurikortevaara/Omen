#pragma once

#include "Renderer.h"

#define VERTEX_ATTRIB_POS 0
#define VERTEX_ATTRIB_TCOORD 1
#define VERTEX_ATTRIB_NORMAL 2
#define VERTEX_ATTRIB_TANGENT 3
#define VERTEX_ATTRIB_BITANGENT 4

namespace omen {
	class Material;
	class Texture;
	class Sprite;
	namespace ui { class Slider; }
	
	namespace ecs {
		class MeshController;

		class MeshRenderer : public Renderer {
		protected:
		protected:
			virtual void onAttach(ecs::Entity* e);
			virtual void onDetach(ecs::Entity* e);
		public:
			MeshRenderer(MeshController* meshController);
			
			MeshRenderer& setMaterial(const Material& material) {
				m_material = std::make_unique<Material>(material);
				return *this;
			}

			virtual void render();
			
			void setShininess(float value) { m_shininess = value; }

			omen::ecs::MeshController* meshController() const { return m_meshController; }
		protected:
			void connectSlider(ui::Slider* slider);

			std::unique_ptr<omen::Shader> m_shader;

		private:
			std::unique_ptr<Material> m_material;
			float m_shininess;
			float m_specularCoeff;
			Texture* m_texture;
			Texture* m_textureNormal;
			Sprite * m_sprite;
			omen::ecs::MeshController* m_meshController;
			glm::vec3 m_lightDir;
			bool	m_renderNormals;
		};

	}
}