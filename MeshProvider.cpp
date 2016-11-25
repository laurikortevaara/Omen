#include "MeshProvider.h"
#include "MathUtils.h"

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

std::unique_ptr<Mesh> MeshProvider::createSphere()
{
	float radius = 10.0f;
	unsigned int rings = 10;
	unsigned int sectors = 10;
	{
		std::vector<GLfloat> vertices;
		std::vector<GLfloat> normals;
		std::vector<GLfloat> texcoords;
		std::vector<GLushort> indices;

		float const R = 1. / (float)(rings - 1);
		float const S = 1. / (float)(sectors - 1);
		int r, s;
		
		vertices.resize(rings * sectors * 3);
		normals.resize(rings * sectors * 3);
		texcoords.resize(rings * sectors * 2);
		std::vector<GLfloat>::iterator v = vertices.begin();
		std::vector<GLfloat>::iterator n = normals.begin();
		std::vector<GLfloat>::iterator t = texcoords.begin();
		for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			*t++ = s*S;
			*t++ = r*R;

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;

			*n++ = x;
			*n++ = y;
			*n++ = z;
		}

		indices.resize(rings * sectors * 4);
		std::vector<GLushort>::iterator i = indices.begin();
		for (r = 0; r < rings - 1; r++) for (s = 0; s < sectors - 1; s++) {
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s + 1);
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = (r + 1) * sectors + s;
		}

		
	}
	return nullptr;
}

std::unique_ptr<Mesh> MeshProvider::createCube()
{
	glm::vec3 vertexData[] = {
		//bottom face
		{ 1.0f, -1.0f, 1.0f },
		{1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},
		{ -1.0f, -1.0f, 1.0f },
		{1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f},


		//top face
		{ -1.0f, 1.0f, -1.0f },
		{1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, 1.0f},
		{ -1.0f, 1.0f, 1.0f },
		{1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, 1.0f},

		//right face
		{ 1.0f, 1.0f, -1.0f },
		{1.0f, -1.0f, -1.0f},
		{1.0f, 1.0f, 1.0f},
		{ 1.0f, 1.0f, 1.0f },
		{1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, 1.0f},

		//front face
		{ 1.0f, 1.0f, 1.0f },
		{1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{ -1.0f, 1.0f, 1.0f },
		{1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},

		//left face
		{ -1.0f, 1.0f, 1.0f },
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, -1.0f},
		{ -1.0f, 1.0f, -1.0f },
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f},
		
		//back face
		{ 1.0f, -1.0f, -1.0f },
		{1.0f, 1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f},
		{ -1.0f, -1.0f, -1.0f },
		{1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f}
	};

	std::unique_ptr<Material> material = std::make_unique<Material>();
	material->setDiffuseColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();
	std::vector<Mesh::Vertex> vertices = {};
	std::vector<glm::vec3> normals = {};
	std::vector<glm::vec3> tangents = {};
	std::vector<glm::vec2> uv = {};
	for (int i = 0; i < size(vertexData); ++i) {
		vertices.push_back(vertexData[i]);
	}

	glm::vec3 max_pos;
	glm::vec3 min_pos;

	for (int i = 0; i < vertices.size(); i += 3) {
		glm::vec3& v1 = vertices[i];
		glm::vec3& v2 = vertices[(i + 1) % (vertices.size())];
		glm::vec3& v3 = vertices[(i + 2) % (vertices.size())];
		max_pos.x = max(v1.x, max_pos.x); max_pos.y = max(v1.y, max_pos.y); max_pos.z = max(v1.z, max_pos.z);
		min_pos.x = min(v1.x, min_pos.x); min_pos.y = min(v1.y, min_pos.y); min_pos.z = min(v1.z, min_pos.z);

		max_pos.x = max(v2.x, max_pos.x); max_pos.y = max(v2.y, max_pos.y); max_pos.z = max(v2.z, max_pos.z);
		min_pos.x = min(v2.x, min_pos.x); min_pos.y = min(v2.y, min_pos.y); min_pos.z = min(v2.z, min_pos.z);

		max_pos.x = max(v3.x, max_pos.x); max_pos.y = max(v3.y, max_pos.y); max_pos.z = max(v3.z, max_pos.z);
		min_pos.x = min(v3.x, min_pos.x); min_pos.y = min(v3.y, min_pos.y); min_pos.z = min(v3.z, min_pos.z);
	}

	for (int i = 0; i < vertices.size(); i+=3 ) {
		glm::vec3& v1 = vertices[i];
		glm::vec3& v2 = vertices[(i+1) % (vertices.size())];
		glm::vec3& v3 = vertices[(i+2) % (vertices.size())];
		glm::vec3 n = glm::normalize(glm::cross((v2 - v1),(v3 - v1)));
		normals.push_back(n);
		normals.push_back(n);
		normals.push_back(n);

		// Create cube mapped UV coordinates
		glm::vec2 uv1, uv2, uv3;

		float max_value = max(fabs(n.x), max(fabs(n.y), fabs(n.z)));
		if (fabs(n.x) == max_value) {
			if (n.z < 0) {
				uv1.x = fabs((v1.z - min_pos.z) / (max_pos.z - min_pos.z));
				uv1.y = fabs((v1.y - min_pos.y) / (max_pos.y - min_pos.y));

				uv2.x = fabs((v2.z - min_pos.z) / (max_pos.z - min_pos.z));
				uv2.y = fabs((v2.y - min_pos.y) / (max_pos.y - min_pos.y));

				uv3.x = fabs((v3.z - min_pos.z) / (max_pos.z - min_pos.z));
				uv3.y = fabs((v3.y - min_pos.y) / (max_pos.y - min_pos.y));
			}
			else {
				uv1.x = fabs((v1.z - max_pos.z) / (max_pos.z - min_pos.z));
				uv1.y = fabs((v1.y - max_pos.y) / (max_pos.y - min_pos.y));

				uv2.x = fabs((v2.z - max_pos.z) / (max_pos.z - min_pos.z));
				uv2.y = fabs((v2.y - max_pos.y) / (max_pos.y - min_pos.y));

				uv3.x = fabs((v3.z - max_pos.z) / (max_pos.z - min_pos.z));
				uv3.y = fabs((v3.y - max_pos.y) / (max_pos.y - min_pos.y));
			}
		}
		else 
		if (fabs(n.y) == max_value) {
			if (n.y < 0) {
				uv1.x = fabs((v1.x - min_pos.x) / (max_pos.x - min_pos.x));
				uv1.y = fabs((v1.z - min_pos.z) / (max_pos.z - min_pos.z));

				uv2.x = fabs((v2.x - min_pos.x) / (max_pos.x - min_pos.x));
				uv2.y = fabs((v2.z - min_pos.z) / (max_pos.z - min_pos.z));

				uv3.x = fabs((v3.x - min_pos.x) / (max_pos.x - min_pos.x));
				uv3.y = fabs((v3.z - min_pos.z) / (max_pos.z - min_pos.z));
			}
			else {
				uv1.x = fabs((v1.x - max_pos.x) / (max_pos.x - min_pos.x));
				uv1.y = fabs((v1.z - max_pos.z) / (max_pos.z - min_pos.z));

				uv2.x = fabs((v2.x - max_pos.x) / (max_pos.x - min_pos.x));
				uv2.y = fabs((v2.z - max_pos.z) / (max_pos.z - min_pos.z));

				uv3.x = fabs((v3.x - max_pos.x) / (max_pos.x - min_pos.x));
				uv3.y = fabs((v3.z - max_pos.z) / (max_pos.z - min_pos.z));
			}
		}
		else
		if (fabs(n.z) == max_value) {
			if (n.z < 0) {
				uv1.x = fabs((v1.x - min_pos.x) / (max_pos.x - min_pos.x));
				uv1.y = fabs((v1.y - min_pos.y) / (max_pos.y - min_pos.y));

				uv2.x = fabs((v2.x - min_pos.x) / (max_pos.x - min_pos.x));
				uv2.y = fabs((v2.y - min_pos.y) / (max_pos.y - min_pos.y));

				uv3.x = fabs((v3.x - min_pos.x) / (max_pos.x - min_pos.x));
				uv3.y = fabs((v3.y - min_pos.y) / (max_pos.y - min_pos.y));
			}
			else {
				uv1.x = fabs((v1.x - max_pos.x) / (max_pos.x - min_pos.x));
				uv1.y = fabs((v1.y - max_pos.y) / (max_pos.y - min_pos.y));

				uv2.x = fabs((v2.x - max_pos.x) / (max_pos.x - min_pos.x));
				uv2.y = fabs((v2.y - max_pos.y) / (max_pos.y - min_pos.y));

				uv3.x = fabs((v3.x - max_pos.x) / (max_pos.x - min_pos.x));
				uv3.y = fabs((v3.y - max_pos.y) / (max_pos.y - min_pos.y));
			}
		}
		
		uv.push_back(uv1);
		uv.push_back(uv2);
		uv.push_back(uv3);
	}

	int i = 0; 
	for (auto& v : vertices) {
		v.uv = uv[i];
		v.normal = normals[i];
		++i;
	}

	for (unsigned int i = 0; i < vertices.size()-3; i += 3) {
		glm::vec3& v0 = vertices[i];
		glm::vec3& v1 = vertices[i + 1];
		glm::vec3& v2 = vertices[i + 2];
		glm::vec2& uv0 = uv[i];
		glm::vec2& uv1 = uv[i + 1];
		glm::vec2& uv2 = uv[i + 2];

		glm::vec3 Edge1 = v1 - v0;
		glm::vec3 Edge2 = v2 - v0;

		float DeltaU1 = uv1.x - uv0.x;
		float DeltaV1 = uv1.y - uv0.y;
		float DeltaU2 = uv2.x - uv0.x;
		float DeltaV2 = uv2.y - uv0.y;

		float f = 1.0f / (DeltaU1 * DeltaV2 - DeltaU2 * DeltaV1);

		glm::vec3 Tangent, Bitangent;

		Tangent.x = f * (DeltaV2 * Edge1.x - DeltaV1 * Edge2.x);
		Tangent.y = f * (DeltaV2 * Edge1.y - DeltaV1 * Edge2.y);
		Tangent.z = f * (DeltaV2 * Edge1.z - DeltaV1 * Edge2.z);

		Bitangent.x = f * (-DeltaU2 * Edge1.x - DeltaU1 * Edge2.x);
		Bitangent.y = f * (-DeltaU2 * Edge1.y - DeltaU1 * Edge2.y);
		Bitangent.z = f * (-DeltaU2 * Edge1.z - DeltaU1 * Edge2.z);

		tangents.push_back(Tangent);

		vertices[i].tangent += Tangent;
		vertices[i+1].tangent += Tangent;
		vertices[i+2].tangent += Tangent;
	}

	tangents.clear();
	for (auto& v : vertices) {
		v.tangent = glm::normalize(v.tangent);
		tangents.push_back(v.tangent);
	}

	for (auto& t : tangents) {
		t = glm::normalize(t);
	}
		
	mesh->setVertices(vertices);
	mesh->setNormals(normals);
	mesh->setTangents(tangents);
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