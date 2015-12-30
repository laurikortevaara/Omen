//
// Created by Lauri Kortevaara(Intopalo) on 29/12/15.
//

#include "WavefrontLoader.h"
#include "Model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>


std::vector<glm::vec3> WavefrontLoader::m_vertices;
std::vector<glm::vec3> WavefrontLoader::m_parameter_coords;
std::vector<glm::vec3> WavefrontLoader::m_normals;
std::vector<glm::vec2> WavefrontLoader::m_texcoords;
std::vector<WavefrontLoader::_material*> WavefrontLoader::materials;

bool WavefrontLoader::loadObj(const std::string &filename) {
    std::ifstream fin(filename, std::ifstream::in);
    if (fin.is_open()) {
        std::string currentFolder = "";
        if (filename.find("/") != std::string::npos) {
            currentFolder = filename.substr(0, filename.rfind("/") + 1);
        }

        std::ostringstream sout;

        std::copy(std::istreambuf_iterator<char>(fin),
                  std::istreambuf_iterator<char>(),
                  std::ostreambuf_iterator<char>(sout));
        fin.close();

        std::string str = sout.str();
        std::cout << str << std::endl;

        std::istringstream f(str);
        std::string s;
        mesh* current_mesh = nullptr;
        while (getline(f, s, '\n')) {
            if (*s.begin() == '#')
                continue;
            // Material Library file
            if (s.find("mtllib") == 0) {
                char mtllib[256];
                sscanf(s.c_str(), "mtllib %s", mtllib);
                loadMaterialLibrary(currentFolder + mtllib);
            }
            else if(s.find("o ") == 0){
                char name[256];
                sscanf(s.c_str(), "o %s", name);
                current_mesh = new mesh;
                current_mesh->name.assign(name);
                meshes.push_back(current_mesh);
            }
            else if (s.find("usemtl") == 0) { // Use the defined material
                char material[256];
                sscanf(s.c_str(), "usemtl %s", material);
                currentMaterialName.reserve(strlen(material));
                currentMaterialName.assign(material);
                if(current_mesh!= nullptr)
                    current_mesh->material=currentMaterialName;
            }
            else
                // List of vertices
            if (s.find("v ") == 0) {
                float x, y, z;
                sscanf(s.c_str(), "v %f %f %f", &x, &y, &z);
                m_vertices.push_back({x, y, z});
            }
            else if (s.find("vt ") == 0) { // List of vertex texture coordinates
                float x, y;
                sscanf(s.c_str(), "vt %f %f", &x, &y);
                m_texcoords.push_back({x, y});
            }
            else if (s.find("vn ") == 0) {// List of vertex normals
                float x, y, z;
                sscanf(s.c_str(), "vn %f %f %f", &x, &y, &z);
                m_normals.push_back({x, y, z});
            }
            else if (s.find("vp ") == 0) {// Parameter space vertices, e.g. free form geometry statement
                float x, y, z;
                sscanf(s.c_str(), "vp %f %f %f", &x, &y, &z);
                m_parameter_coords.push_back({x, y, z});
            }
            else if (s.find("f ") == 0) { // Polygonal face element
                std::string str_face;
                std::istringstream is(s);

                int num_vertices_per_face = 0;
                std::vector<_index_elem> _vertex_indices;
                current_mesh->faces.push_back({});
                face& current_face = current_mesh->faces.back();
                while (getline(is, str_face, ' ')) {
                    if(str_face == "f")
                        continue;
                    size_t n = std::count(str_face.begin(), str_face.end(), '/');
                    int vi, ti, ni; // Vertex index, texture index, normal index;
                    vi = ti = ni = -1;
                    switch (n) {
                        case 0: // Only vertex index
                            sscanf(str_face.c_str(), "%i", &vi);
                            break;
                        case 1: // Vertex index + texture index
                            sscanf(str_face.c_str(), "%i/%i", &vi, &ti);
                            break;
                        case 2: // Vertex index + texture index + normal index or
                            // Vertex index + // + normal index
                            if (str_face.find("//") == std::string::npos) {
                                sscanf(str_face.c_str(), "%i/%i/%i", &vi, &ti, &ni);
                            }
                            else {
                                sscanf(str_face.c_str(), "%i//%i", &vi, &ni);
                            }
                            break;
                    }
                    current_face.indices.push_back({vi, ti, ni});
                    num_vertices_per_face++;
                }

            }
            else if (s.find("s ") == 0) { // Define the smooth shading parameter
                char smooth[256];
                sscanf(s.c_str(), "s %s", smooth);
            }
        }
    }
    return true;
}

/**
 * Load material file
 */
bool WavefrontLoader::loadMaterialLibrary(const std::string &filename) {
    std::ifstream fin(filename, std::ifstream::in);
    if (fin.is_open()) {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::ostringstream sout;

        std::copy(std::istreambuf_iterator<char>(fin),
                  std::istreambuf_iterator<char>(),
                  std::ostreambuf_iterator<char>(sout));
        fin.close();

        std::string str = sout.str();
        std::cout << str << std::endl;

        std::istringstream f(str);
        std::string s;
        _material* currentMaterial = nullptr;
        while (getline(f, s, '\n')) {
            if (*s.begin() == '#')
                continue;
            // Material Library file
            if (s.find("newmtl") == 0) {
                char material[256];
                sscanf(s.c_str(), "newmtl %s", material);

                currentMaterial = new _material;
                memset(currentMaterial, 0, sizeof(_material));
                materials.push_back(currentMaterial);
                currentMaterial->name.assign(material);

            }
            else if (s.find("Ns ") == 0) {
                float Ns;
                sscanf(s.c_str(), "Ns %f", &Ns);
                currentMaterial->Ns = Ns;
            }
            else if (s.find("Ka ") == 0) {
                float r,g,b;
                sscanf(s.c_str(), "Ka %f %f %f", &r, &g, &b);
                currentMaterial->Ka = {r,g,b};
            }
            else if (s.find("Kd ") == 0) {
                float r,g,b;
                sscanf(s.c_str(), "Kd %f %f %f", &r, &g, &b);
                currentMaterial->Kd = {r,g,b};
            }
            else if (s.find("Ks ") == 0) {
                float r,g,b;
                sscanf(s.c_str(), "Ks %f %f %f", &r, &g, &b);
                currentMaterial->Ks = {r,g,b};
            }
            else if (s.find("Ka ") == 0) {
                float r,g,b;
                sscanf(s.c_str(), "Ka %f %f %f", &r, &g, &b);
                currentMaterial->Ka = {r,g,b};
            }
            else if (s.find("Ni ") == 0) {
                float Ni;
                sscanf(s.c_str(), "Ni %f", &Ni);
                currentMaterial->Ni = Ni;
            }
            else if (s.find("d ") == 0) {
                float d;
                sscanf(s.c_str(), "d %f", &d);
                currentMaterial->d = d;
            }
            else if (s.find("map_Kd ") == 0) {
                char map_Kd[256];
                sscanf(s.c_str(), "map_Kd %s", map_Kd);
                currentMaterial->map_Kd.assign(map_Kd);
            }
            else if (s.find("map_Ka ") == 0) {
                char map_Ka[256];
                sscanf(s.c_str(), "map_Ka %s", map_Ka);
                currentMaterial->map_Ka.assign(map_Ka);
            }
            else if (s.find("map_Ks ") == 0) {
                char map_Ks[256];
                sscanf(s.c_str(), "map_Ks %s", map_Ks);
                currentMaterial->map_Ka.assign(map_Ks);
            }

        }
    }
    return true;
}

using namespace Omen;
std::unique_ptr<Omen::Mesh> WavefrontLoader::mesh::getMesh() {
    Material* material = new Material();
    WavefrontLoader::_material* mdef = nullptr;
    for(auto mat : WavefrontLoader::materials )
        if(mat->name==this->material){mdef=mat;break;}
    if(mdef!= nullptr){
        if(!mdef->map_Kd.empty())
            material->setTexture(new Omen::Texture(mdef->map_Kd));
        material->setDiffuseColor(glm::vec4(mdef->Kd,1));
        material->setAmbientColor(glm::vec4(mdef->Ka,1));
        material->setSpecularColor(glm::vec4(mdef->Ks,1));
    }
    std::string shader_name = "shaders/pass_through.glsl";

    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;
    std::vector<GLsizei> vertex_indices_tris;

    for(auto index : this->faces){
        vertices.push_back({1,1,1});vertices.push_back({1,1,1});vertices.push_back({1,1,1});
        texcoords.push_back({1,1});texcoords.push_back({1,1});texcoords.push_back({1,1});
        normals.push_back({1,1,1});normals.push_back({1,1,1});normals.push_back({1,1,1});
    }
    for(auto face : this->faces){

        for(auto index : face.indices) {
            int vi = index.vertex_index - 1;
            int ti = index.texture_index - 1;
            int ni = index.normal_index - 1;

            glm::vec3 v = WavefrontLoader::m_vertices[vi];
            glm::vec3 n = WavefrontLoader::m_normals[ni];
            glm::vec2 t = WavefrontLoader::m_texcoords[ti];

            glm::vec3 v1, v2, v3;
            v1 = WavefrontLoader::m_vertices[face.indices[0].vertex_index];
            v2 = WavefrontLoader::m_vertices[face.indices[1].vertex_index];
            v3 = WavefrontLoader::m_vertices[face.indices[2].vertex_index];
            glm::vec3 s1 = v3 - v1;
            glm::vec3 s2 = v3 - v2;
            glm::vec3 norm = glm::normalize(glm::cross(s1,s2));

            std::cout << "v: " << v.x << ", " << v.y << ", " << v.z << " -- ";
            std::cout << "n: " << n.x << ", " << n.y << ", " << n.z << " -- ";
            std::cout << "t: " << t.x << ", " << t.y <<  std::endl;

            vertices[vi] = WavefrontLoader::m_vertices[vi];
            if(index.texture_index>=0)texcoords[vi]=WavefrontLoader::m_texcoords[ti];
            if(index.normal_index>=0)normals[vi]=WavefrontLoader::m_normals[ni];
            vertex_indices_tris.push_back(vi);
        }
    }

    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(shader_name.c_str(), material, vertices, normals, texcoords, vertex_indices_tris);
    std::cout << "Qexit" << std::endl;
    vertices.erase(vertices.begin(), vertices.end());
    texcoords.erase(texcoords.begin(), texcoords.end());
    normals.erase(normals.begin(), normals.end());
    return std::move(mesh);
    /*material2->setDiffuseColor(glm::vec4(mdef->Kd,1));
    material2->setAmbientColor(glm::vec4(mdef->Ka,1));
    material2->setSpecularColor(glm::vec4(mdef->Ks,1));
    if(!mdef->map_Kd.empty())
        material2->setTexture(new Texture(mdef->map_Kd));

    std::unique_ptr<Model> model2 = std::make_unique<Model>(std::move(mesh2));
    model2->m_mesh->m_position = glm::vec3(0,0,0);
    model2->m_mesh->m_amplitude = 0.0;
     */
    return nullptr;
}
