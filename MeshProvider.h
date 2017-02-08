#ifndef OMEN_MESH_PROVIDER_H
#define OMEN_MESH_PROVIDER_H

#include "Material.h"
#include "Mesh.h"
#include <list>

namespace omen {
	namespace ecs { class GameObject; }

	class MeshProvider
	{
	public:
		MeshProvider();
		~MeshProvider();

		static std::unique_ptr<Mesh> createPlane(float size = 10.0, int subdiv = 1);
		static std::unique_ptr<Mesh> createCube();
		static std::unique_ptr<Mesh> createSphere();
		static std::list< std::unique_ptr<omen::ecs::GameObject> > loadObject(const std::string& filename);
	};
}

#endif // OMEN_MESH_PROVIDER_H