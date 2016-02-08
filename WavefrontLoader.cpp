//
// Created by Lauri Kortevaara(Intopalo) on 29/12/15.
//

#include "WavefrontLoader.h"
#include "Model.h"
#include "utils.h"
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

using namespace omen;
std::unique_ptr<omen::Mesh> WavefrontLoader::mesh::getMesh() {
    GLfloat s = 1000;
    std::vector<GLsizei> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    for(auto face : this->faces){
        for(auto index : face.indices){
            indices.push_back(indices.size());
            vertices.push_back(WavefrontLoader::m_vertices.at(index.vertex_index-1));
            if(!WavefrontLoader::m_normals.empty())normals.push_back(WavefrontLoader::m_normals.at(index.normal_index-1));
            if(!WavefrontLoader::m_texcoords.empty())texcoords.push_back(WavefrontLoader::m_texcoords.at(index.texture_index-1));
        }
    }

    Material* material = new Material();
    for(auto matdef : WavefrontLoader::materials) {
        if(matdef->name==this->material){
            material->setDiffuseColor(glm::vec4(matdef->Kd,1));
            material->setAmbientColor(glm::vec4(matdef->Ka,1));
            material->setSpecularColor(glm::vec4(matdef->Ks,1));
            if(!matdef->map_Kd.empty()) material->setTexture(new Texture(matdef->map_Kd));
            break;
        }
    }

    std::string shader_name = "shaders/pass_through.glsl";
    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(shader_name, material, vertices, normals, texcoords, indices);
    return mesh;
}
