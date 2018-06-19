#pragma once

#include "component/Renderer.h"
#include "component/MeshRenderer.h"
#include "Octree.h"
#include "Shader.h"

namespace omen {
	namespace ecs {
		class OctreeRenderer : public Renderer {
		public:
			OctreeRenderer(std::unique_ptr<omen::Octree> octree) : m_octree(std::move(octree)) { initializeRenderer(); };
		protected:
			void initializeRenderer();

			Mesh* mesh;
			std::unique_ptr<omen::Octree> m_octree;
			Shader* m_shader;
			// Inherited via Renderer
			virtual void onAttach(ecs::Entity * e) override;

			virtual void onDetach(ecs::Entity * e) override;

			virtual void render(omen::Shader * shader = nullptr) const override;

			void renderNode(Octree::Node* node, int level);
		private:
		};
	}
}