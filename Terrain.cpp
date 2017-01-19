#include "Terrain.h"
#include "component/MeshController.h"
#include "component/MeshRenderer.h"
#include "TerrainRenderer.h"
#include "MeshProvider.h"

using namespace omen;

Terrain::Terrain() :
	GameObject("Terrain")
{
	std::unique_ptr<ecs::MeshController> mc = std::make_unique<ecs::MeshController>();
	std::unique_ptr<ecs::TerrainRenderer> mr = std::make_unique<ecs::TerrainRenderer>(mc.get());

	std::unique_ptr<Mesh> mesh = MeshProvider::createPlane(1000, 1);

	mc->setMesh(std::move(mesh));
	addComponent(std::move(mr));
	addComponent(std::move(mc));
}


Terrain::~Terrain()
{
}
