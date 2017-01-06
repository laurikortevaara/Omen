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
		static std::unique_ptr<Mesh> loadObject(const std::string& filename);
	};
}

#endif // OMEN_MESH_PROVIDER_H