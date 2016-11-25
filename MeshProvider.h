#ifndef OMEN_MESH_PROVIDER_H
#define OMEN_MESH_PROVIDER_H

#include "Material.h"
#include "Mesh.h"

namespace omen {
	class MeshProvider
	{
	public:
		MeshProvider();
		~MeshProvider();

		static std::unique_ptr<Mesh> createPlane();
		static std::unique_ptr<Mesh> createCube();
		static std::unique_ptr<Mesh> createSphere();
	};
}

#endif // OMEN_MESH_PROVIDER_H