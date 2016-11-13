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

	std::vector<glm::vec3> vertices = {};
	std::vector<glm::vec2> uv;
	float max = 1;
	float scale = 10.0/max;
	for (float x = 0; x < max; ++x)
		for (float y = 0; y < max; ++y) {
			float fy  = sin(scale*(x/(scale*max))*(180/3.14))+cos(scale*(y/ (scale*max))*(180/3.14));
			float fy2 = sin(scale*((x+1) / (scale*max))*(180 / 3.14)) + cos(scale*(y / (scale*max))*(180 / 3.14));
			vertices.push_back({ scale*(-0.5f+x), fy2,  scale*(0.5f+y)});
			vertices.push_back({ scale*(-0.5f+x), fy, scale*(-0.5f+y)});
			vertices.push_back({ scale*(0.5f+x),  fy2,  scale*(0.5f+y)});
			uv.push_back({ 0, 1 });
			uv.push_back({ 0, 0 });
			uv.push_back({ 1, 1 });
		
			vertices.push_back({ scale*(-0.5f+x), fy,  scale*(-0.5f+y)});
			vertices.push_back({ scale*(0.5f+x),  fy2,  scale*(-0.5f+y)});
			vertices.push_back({ scale*(0.5f+x),  fy2,   scale*(0.5f+y)});
			uv.push_back({ 0, 0 });
			uv.push_back({ 1, 0 });
			uv.push_back({ 1, 1 });
		}
	std::vector<glm::vec3> normals = { { -1,0,-1 },{ -1,0,1 },{ 1,0,-1 } };
	
	std::vector<GLsizei> indices = { 0,1,2 };
	

	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
	mesh->setVertices(vertices);
	mesh->setNormals(normals);
	mesh->setUVs(uv);
	mesh->setVertexIndices(indices);
	return std::move(mesh);
}