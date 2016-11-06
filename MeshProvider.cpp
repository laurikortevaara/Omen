#include "MeshProvider.h"


using namespace omen;

MeshProvider::MeshProvider()
{
}


MeshProvider::~MeshProvider()
{
}

std::shared_ptr<Mesh> MeshProvider::createPlane()
{
	/*std::vector<Mesh::Frame> frames;
	frames.push_back({})
	std::shared_ptr<Material> material = std::make_shared<Material>();

	std::vector<glm::vec3> vertices = { { -1,0,-1 },{ -1,0,1 },{ 1,0,-1 } };
	std::vector<int> indices = { 0,1,2 };
	

	material->setDiffuseColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>("shaders/pass_through.glsl", material, );
	*/
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	mesh->setShader(std::make_unique<Shader>("shaders/pass_through.glsl").get());
	return mesh;
}