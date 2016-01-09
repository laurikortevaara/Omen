//
// Created by Lauri Kortevaara(Intopalo) on 02/01/16.
//

#include "MD3Loader.h"
#include <iomanip>
#include <iostream>
#include <fstream>

#define _USE_MATH_DEFINES

#include <math.h>

using namespace std;
using namespace Omen;


bool MD3Loader::loadModel(const string &filepath) {
    ifstream ifs(filepath, ios::ate | ios::binary | ios::in);
    if (ifs.is_open()) {
        m_bufferSize = ifs.tellg();

        // Read the whole file into a buffer
        m_buffer = new U8[m_bufferSize];
        memset(m_buffer, 0, m_bufferSize);
        ifs.seekg(0, ios::beg);
        ifs.read((char *) m_buffer, m_bufferSize);
        ifs.close();


        // Debug print out
        /*cout.setf(ios::hex, ios::basefield);
        for(size_t i=0; i < filesize; ++i){
            unsigned short h = buffer[i];
            cout << h;
        }*/

        // Start reading the content, from the header
        m_currentPos = 0;
        if (readHeader())
            readSurfaces();
        delete[] m_buffer;
    }
    return true;
}

bool MD3Loader::readHeader() {
    m_header = (s_header * )((unsigned long) m_buffer + m_currentPos);
    m_surfaces = (s_surface * )((unsigned long) m_buffer + m_header->ofs_surfaces);
    m_frames = (s_frame * )((unsigned long) m_buffer + m_header->ofs_frames);
    return true;
}

void MD3Loader::readSurfaces() {
    for (int surf_i = 0; surf_i < m_header->num_surfaces; ++surf_i) {
        s_surface *s = m_surfaces + sizeof(s_surface) * (surf_i);

        m_meshes.push_back(new mesh);
        // Read triangles
        for (int tri_i = 0; tri_i < s->num_triangles; ++tri_i) {
            s_triangle *tri = (s_triangle * )((unsigned long) s + s->ofs_triangles + tri_i * sizeof(s_triangle));
            mesh *mesh = m_meshes.back();
            face f;

            // Read the vertex indices
            _index_elem i;
            i.vertex_index = i.normal_index = i.texture_index = tri->m_indices[0];
            f.indices.push_back(i);
            i.vertex_index = i.normal_index = i.texture_index = tri->m_indices[1];
            f.indices.push_back(i);
            i.vertex_index = i.normal_index = i.texture_index = tri->m_indices[2];
            f.indices.push_back(i);
            mesh->faces.push_back(f);
        }

        // Read the texture coordinates
        for (int vert_i = 0; vert_i < s->num_verts; ++vert_i) {
            s_texcoord *tcoord = (s_texcoord * )((unsigned long) s + s->ofs_st + vert_i * sizeof(s_vertex));
            std::cout << "Tcoord: " << tcoord->x << ", " << tcoord->y << std::endl;
            m_texcoords.push_back(*tcoord);
        }

        // Read shaders
        for( int shader_i = 0; shader_i < s->num_shaders; ++shader_i){
            s_shader* shader = (s_shader*)((unsigned long) s + s->ofs_shaders + shader_i * sizeof(s_shader));
            std::string shader_path = (const char*)shader->name;
            m_shaders.push_back(shader_path);
        }

        // Read the frames (consisting of vertices, normals and texcoords)
        for (int frame_i = 0; frame_i < m_header->num_frames; ++frame_i) {
            frame f;
            for (int vert_i = 0; vert_i < s->num_verts; ++vert_i) {
                s_vertex *vertex = (s_vertex * )(
                        (unsigned long) s + s->ofs_xyznormal + vert_i * sizeof(s_vertex) +
                        sizeof(s_vertex) * s->num_verts * frame_i);

                glm::vec2 *normal = (glm::vec2 *) &vertex->normal;
                f.m_vertices.push_back(getRealVertex(vertex->coord));
                //f.m_vertices.back().z += 2*frame_i;
                f.m_normals.push_back(fromSpherical(vertex->normal));
            }
            mesh *mesh = m_meshes.back();
            mesh->frames.push_back(f);
        }
    }
}


void MD3Loader::readFrames() {
    for (int frame_i = 0; frame_i < m_header->num_frames; ++frame_i) {
        s_frame *frame = (s_frame * )((unsigned long) m_header->ofs_frames + frame_i * sizeof(s_frame));
        std::string frame_name = (const char *) frame->name;
        glm::vec3 origin = frame->local_origin;
    }
}

glm::vec3 MD3Loader::getRealVertex(S16 vertices[3]) {
    float conv = 1.0f / 64;
    conv *= 0.1;
    return glm::vec3(vertices[0] * conv, vertices[1] * conv, vertices[2] * conv);
}

glm::vec3 rationalVec(glm::vec3 vec) {
    glm::vec3 v(vec.x, vec.y, vec.z);
    v *= 0.1;
    float x_sign = v.x / fabs(v.x);
    float y_sign = v.y / fabs(v.y);
    float z_sign = v.z / fabs(v.z);
    float threshold = 0.05;
    if (1.0 - fabs(v.x) < threshold) v.x = 1.0f * x_sign;
    if (1.0 - fabs(v.y) < threshold) v.y = 1.0f * y_sign;
    if (1.0 - fabs(v.z) < threshold) v.z = 1.0f * z_sign;
    if (fabs(v.x) < threshold) v.x = 0.0f;
    if (fabs(v.y) < threshold) v.y = 0.0f;
    if (fabs(v.z) < threshold) v.z = 0.0f;
    return v;
}

glm::vec3 MD3Loader::fromSpherical(U8 spherical_coord[2]) {
    glm::vec3 vec;
    float lat = spherical_coord[0] * (2 * M_PI) / 255.0f;
    float lng = spherical_coord[1] * (2 * M_PI) / 255.0f;
    return rationalVec(glm::normalize(glm::vec3(cos(lng) * sin(lat), sin(lng) * sin(lat), cos(lat))));
}


using namespace Omen;

void MD3Loader::getMesh(std::vector<std::unique_ptr<Omen::Mesh>> &meshes) {
    GLfloat s = 1000;
    std::vector<GLsizei> indices;
    std::vector<Omen::Mesh::Frame> frames;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;

    for (auto m : m_meshes) {
        std::unique_ptr<Mesh> mesh = nullptr;

        for (auto face : m->faces) {
            for (auto index : face.indices) {
                indices.push_back(indices.size());
                if (!m_texcoords.empty())texcoords.push_back(m_texcoords.at(index.texture_index));
            }
        }

        for (auto frame : m->frames) {
            Omen::Mesh::Frame f;
            for (auto face : m->faces) {
                for (auto index : face.indices) {
                    f.m_vertices.push_back(frame.m_vertices.at(index.vertex_index));
                    if (!frame.m_normals.empty())f.m_normals.push_back(frame.m_normals.at(index.normal_index));
                }
            }
            frames.push_back(f);
        }


        Material *material = new Material();

        material->setDiffuseColor(glm::vec4(0.75, 0.75, 0.75, 1));
        material->setAmbientColor(glm::vec4(0.75, 0.75, 0.75, 1));
        material->setSpecularColor(glm::vec4(0.75, 0.75, 0.75, 1));
        material->setTexture(new Texture("models/ToDPirateHologuise/Hologuise3_Color.png"));
        //material->setMatcapTexture(new Texture("textures/generator1.jpg"));

        std::string shader_name = "shaders/pass_through.glsl";
        mesh = std::make_unique<Mesh>(shader_name, material, frames, texcoords, indices);
        meshes.push_back(std::move(mesh));
    }
}
