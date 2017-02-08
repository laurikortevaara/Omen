#pragma once

#include "Component.h"
#include "../Mesh.h"

namespace omen {
	namespace ecs {
		class MeshController : public ecs::Component {
		protected:
			virtual void onAttach(ecs::Entity* entity);
			virtual void onDetach(ecs::Entity* entity);
		public:
			MeshController() : ecs::Component(), m_mesh(nullptr), m_castShadow(true) {};
			MeshController& setMesh(std::unique_ptr<Mesh> mesh) {
				m_mesh = std::move(mesh);
				return *this;
			}

			void setCastShadow(bool castShadow) { m_castShadow = castShadow; }
			bool castShadow() const { return m_castShadow; }

			const std::unique_ptr<Mesh>& mesh() const { return m_mesh; }
		protected:
		private:
			std::unique_ptr<Mesh> m_mesh;
			bool m_castShadow;

		};
	}
}