#include "MeshProvider.h"


using namespace omen;

MeshProvider::MeshProvider()
{
}


MeshProvider::~MeshProvider()
{
}

std::unique_ptr<Mesh> MeshProvider::createPlane()
{
	std::unique_ptr<Material> material = std::make_unique<Material>();
	material->setDiffuseColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	std::vector<glm::vec3> vertices = { { -1,0,-1 },{ -1,0,1 },{ 1,0,-1 } };
	std::vector<glm::vec3> normals = { { -1,0,-1 },{ -1,0,1 },{ 1,0,-1 } };
	std::vector<glm::vec2> uv = { { -1,0},{ -1,0 },{ 1,0 } };
	std::vector<GLsizei> indices = { 0,1,2 };
	

	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
	return std::move(mesh);
}