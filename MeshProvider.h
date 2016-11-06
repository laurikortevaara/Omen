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

		static std::shared_ptr<Mesh> createPlane();
	};
}

#endif // OMEN_MESH_PROVIDER_H