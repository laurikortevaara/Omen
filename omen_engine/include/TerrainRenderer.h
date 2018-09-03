#pragma once

#include "component/MeshRenderer.h"

namespace omen {
	namespace ecs {
		class TerrainRenderer : public MeshRenderer
		{
		public:
			TerrainRenderer(MeshController* mc);
			~TerrainRenderer();

			virtual void render();
		};
	}
}

