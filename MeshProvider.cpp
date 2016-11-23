#include "MeshProvider.h"


using namespace omen;

MeshProvider::MeshProvider()
{
}


MeshProvider::~MeshProvider()
{
}

template<typename T, int sz>
int size(T(&)[sz])
{
	return sz;
}

std::unique_ptr<Mesh> MeshProvider::createCube()
{
	const GLfloat vertexData[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};

	std::unique_ptr<Material> material = std::make_unique<Material>();
	material->setDiffuseColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
	std::vector<glm::vec3> vertices = {};
	std::vector<glm::vec3> normals = {};
	std::vector<glm::vec2> uv = {};
	for (int i = 0; i < size(vertexData); i+=3) {
		vertices.push_back({ vertexData[i], vertexData[i + 1], vertexData[i + 2] });
	}

	for (int i = 0; i < vertices.size()-3; ++i ) {
		glm::vec3& v1 = vertices[i];
		glm::vec3& v2 = vertices[(i+2) % (vertices.size()-1)];
		glm::vec3& v3 = vertices[(i + 1) % (vertices.size() - 1)];
		glm::vec3 n = glm::normalize(glm::cross((v2 - v1),(v3 - v1)));
		normals.push_back(n);
	}


	for (int i = 0; i < vertices.size() - 3; i += 3) {
		uv.push_back({ 0,0 });
		uv.push_back({ 0,1 });
		uv.push_back({ 1,0 });
	}
	mesh->setVertices(vertices);
	mesh->setNormals(normals);
	mesh->setUVs(uv);
	//mesh->setVertexIndices(indices);
	return std::move(mesh);
}

std::unique_ptr<Mesh> MeshProvider::createPlane()
{
	std::unique_ptr<Material> material = std::make_unique<Material>();
	material->setDiffuseColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	float size = 100.0f;
	std::vector<glm::vec3> vertices = { {-size,0,-size },{ size,0,size },{ -size,0,size },{ -size,0,-size },{ size,0,-size }, { size,0,size } };

	std::vector<glm::vec2> uv = { {0,0},{ 1,1 },{ 0,1 },{ 0,0 },{ 1,0 },{ 1,1 } };
	std::vector<glm::vec3> normals = { { 0,1,0 },{ 0,1,0 },{ 0,1,0 },{ 0,1,0 },{ 0,1,0 },{ 0,1,0 } };
	/*float max = 100;
	float scale = 10.0/max;
	for (float x = 0; x < max; ++x)
		for (float y = 0; y < max; ++y) {
			float fy  = sin(scale*(scale*x/(max))*(180/3.14))+cos(scale*(scale*y/ (max))*(180/3.14));
			float fy2 = fy;
			vertices.push_back({ scale*(-0.5f+x), fy2,  scale*(0.5f+y)});
			vertices.push_back({ scale*(-0.5f+x), fy, scale*(-0.5f+y)});
			vertices.push_back({ scale*(0.5f+x),  fy2,  scale*(0.5f+y)});
			uv.push_back({ 0, 1 });
			uv.push_back({ 0, 0 });
			uv.push_back({ 1, 1 });
			normals.push_back({ 0,-1,0 });
			normals.push_back({ 0,-1,0 });
			normals.push_back({ 0,-1,0 });
		
			vertices.push_back({ scale*(-0.5f+x), fy,  scale*(-0.5f+y)});
			vertices.push_back({ scale*(0.5f+x),  fy2,  scale*(-0.5f+y)});
			vertices.push_back({ scale*(0.5f+x),  fy2,   scale*(0.5f+y)});
			uv.push_back({ 0, 0 });
			uv.push_back({ 1, 0 });
			uv.push_back({ 1, 1 });
			normals.push_back({ 0,-1,0 });
			normals.push_back({ 0,-1,0 });
			normals.push_back({ 0,-1,0 });
		}*/
		
	std::vector<GLsizei> indices = { 0,1,2 };
	
	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
	mesh->setVertices(vertices);
	mesh->setNormals(normals);
	mesh->setUVs(uv);
	mesh->setVertexIndices(indices);
	return std::move(mesh);
}