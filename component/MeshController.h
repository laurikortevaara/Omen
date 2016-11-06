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
			MeshController() : ecs::Component(), m_mesh(nullptr) {};
			MeshController& setMesh(std::shared_ptr<Mesh> mesh) {
				m_mesh = mesh;
				return *this;
			}

			const std::shared_ptr<Mesh>& mesh() { return m_mesh; }
		protected:
		private:
			std::shared_ptr<Mesh> m_mesh;

		};
	}
}