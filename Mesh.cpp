//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include "Mesh.h"
#include "OpenGL/gl3.h"
#include "GL_error.h.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION

#include <stb/stb_image.h>

#define BUFFER_OFFSET(offset)  (0 + offset)

GLuint vao = 0;
GLuint vbo = 0;
GLuint vbo_barycentric = 0;
GLuint vbo_texcoord = 0;
GLint attrib_vPosition;
GLint attrib_barycentric;
GLint attrib_texcoord;

GLuint shader_program = 0;
GLuint textureID = 0;
GLboolean draw_mesh = 1;
GLboolean draw_triangle_patches = 1;

void loadShader(const std::string &filename, std::string &out) {
    std::string line;
    std::ifstream in(filename.c_str());
    while (std::getline(in, line)) {
        out += line + "\n";
    }
}


Mesh::Mesh() : fInnerTess(1.0), fOuterTess(1.0), mPolygonMode(GL_FILL) {
    loadShaders();
    check_gl_error();
    loadTextures();
    check_gl_error();
    createPatches();
}

Mesh::~Mesh() {

}


void Mesh::loadTextures() {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int x, y, btm;
    FILE *f = fopen("heightmap.jpg", "r");
    stbi_uc *image = stbi_load_from_file(f, &x, &y, &btm, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexImage2D(GL_TEXTURE_2D, 0, btm == 3 ? GL_RGB : GL_RGBA, x, y, 0, btm == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE,
                 (const void *) image);
    check_gl_error();
    stbi_image_free(image);
}

void Mesh::loadShaders() {
    shader_program = glCreateProgram();

    std::string vertex_shader = "";
    std::string control_shader = "";
    std::string eval_shader = "";
    std::string geom_shader = "";
    std::string fragment_shader = "";
    if (draw_mesh) {
        loadShader("mesh_shaders/vs.glsl", vertex_shader);
        loadShader("mesh_shaders/fs.glsl", fragment_shader);
    }
    else {
        loadShader("patch_shaders/vs.glsl", vertex_shader);
        if (draw_triangle_patches) {
            loadShader("patch_shaders/tc_triangle.glsl", control_shader);
            loadShader("patch_shaders/te_triangle.glsl", eval_shader);
        }
        else {
            loadShader("patch_shaders/tc_quad.glsl", control_shader);
            loadShader("patch_shaders/te_quad.glsl", eval_shader);
            //loadShader("patch_shaders/gs.glsl", geom_shader);
        }
        loadShader("patch_shaders/fs.glsl", fragment_shader);
    }


    GLint sizei = 255;
    GLchar log1[255] = {""};
    GLchar log2[255] = {""};
    GLchar log3[255] = {""};
    GLchar log4[255] = {""};
    GLchar log5[255] = {""};
    GLchar log_program[255];

    if (!vertex_shader.empty()) {
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
        const GLchar *ss = vertex_shader.c_str();
        glShaderSource(vs, 1, &ss, NULL);
        glCompileShader(vs);
        glGetShaderInfoLog(vs, 255, &sizei, log1);
        std::cout << log1 << std::endl;
        assert(strlen(log1) == 0);
        glAttachShader(shader_program, vs);
    }

    if (!control_shader.empty()) {
        GLuint tc = glCreateShader(GL_TESS_CONTROL_SHADER);
        const GLchar *ss = control_shader.c_str();
        glShaderSource(tc, 1, &ss, NULL);
        glCompileShader(tc);
        glGetShaderInfoLog(tc, 255, &sizei, log2);
        std::cout << log2 << std::endl;
        assert(strlen(log2) == 0);
        glAttachShader(shader_program, tc);
        check_gl_error();
    }

    if (!eval_shader.empty()) {
        GLuint te = glCreateShader(GL_TESS_EVALUATION_SHADER);
        const GLchar *ss = eval_shader.c_str();
        glShaderSource(te, 1, &ss, NULL);
        glCompileShader(te);
        glGetShaderInfoLog(te, 255, &sizei, log3);
        std::cout << log3 << std::endl;
        assert(strlen(log3) == 0);
        glAttachShader(shader_program, te);
    }

    if (!geom_shader.empty()) {

        GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
        const GLchar *ss = geom_shader.c_str();
        glShaderSource(gs, 1, &ss, NULL);
        glCompileShader(gs);
        glGetShaderInfoLog(gs, 255, &sizei, log4);
        std::cout << log4 << std::endl;
        assert(strlen(log4) == 0);
        glAttachShader(shader_program, gs);
        check_gl_error();
    }

    if (!fragment_shader.empty()) {
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
        const GLchar *ss = fragment_shader.c_str();
        glShaderSource(fs, 1, &ss, NULL);
        glCompileShader(fs);
        glGetShaderInfoLog(fs, 255, &sizei, log5);
        assert(strlen(log5) == 0);
        glAttachShader(shader_program, fs);
        check_gl_error();
    }

    glLinkProgram(shader_program);
    glGetProgramInfoLog(shader_program, 255, &sizei, log_program);
    check_gl_error();
    assert(strlen(log5) == 0);
}

void Mesh::createPatches() {
    // Create the Vertex Array
    glGenVertexArrays(1, &m_vao);
    check_gl_error();
    glBindVertexArray(m_vao);
    check_gl_error();

    //createIcosahedron(m_vertices);
    loadModel( "test.obj", m_vertices );
    check_gl_error();

    // Create the Vertex Buffer Object
    //std::vector<GLfloat> vertices = {-1, -1, 0, 1,  -1, 0, 0,  1,  0};
    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;

    for (auto v : m_vertices) {
        vertices.insert(vertices.end(), {v->m_position.x, v->m_position.y, v->m_position.z});
        normals.insert(normals.end(), {v->m_normal.x, v->m_normal.y, v->m_normal.z});
    }

    glGenBuffers(1, &m_vbo_vertices);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    check_gl_error();

    glGenBuffers(1, &m_vbo_normals);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals) * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    check_gl_error();


    // Create the vertex index buffer
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);
    check_gl_error();

}


void Mesh::setupModelView() {
    ////
    //// Projection Matrix
    ////
    GLint normal_matrix_handle = glGetUniformLocation(shader_program, "NormalMatrix");
    GLint modelview_matrix__handle = glGetUniformLocation(shader_program, "ModelViewMatrix");
    GLint mvp_handle = glGetUniformLocation(shader_program, "ModelViewProjectionMatrix");
    if (mvp_handle >= 0) {
        double ViewPortParams[4];
        glGetDoublev(GL_VIEWPORT, ViewPortParams);
        check_gl_error();
        // Generates a really hard-to-read matrix, but a normal, standard 4x4 matrix nonetheless
        glm::mat4 ProjectionMatrix = glm::perspective(
                90.0f,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
                1280.0f /
                720.0f, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
                0.01f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
                10.0f       // Far clipping plane. Keep as little as possible.
        );
        // Camera matrix
        glm::mat4 ViewMatrix = glm::lookAt(
                glm::vec3(0, 0, -5), // Camera is at (4,3,3), in World Space
                glm::vec3(0, 0, 0), // and looks at the origin
                glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
        );
        static float angle = 0.0f;
        angle += 0.01;
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 ModelMatrix = glm::mat4(1.0f);

        ModelMatrix = glm::rotate(ModelMatrix, angle, glm::vec3(1.1f, 1.0f, 0.0f));
        // Our ModelViewProjection : multiplication of our 3 matrices
        glm::mat3 NormalMatrix = glm::transpose(glm::inverse(
                glm::mat3(ViewMatrix * ModelMatrix))); // Remember, matrix multiplication is the other way around
        glm::mat3 ModelViewMatrix = glm::mat3(ViewMatrix * ModelMatrix);
        glm::mat4 ModelViewProjectionMatrix =
                ProjectionMatrix * ViewMatrix * ModelMatrix; // Remember, matrix multiplication is the other way around

        glUniformMatrix3fv(normal_matrix_handle, 1, GL_FALSE, &NormalMatrix[0][0]);
        check_gl_error();
        glUniformMatrix4fv(modelview_matrix__handle, 1, GL_FALSE, &ModelViewMatrix[0][0]);
        check_gl_error();
        glUniformMatrix4fv(mvp_handle, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
        check_gl_error();
    }

}

void Mesh::render() {
    glPolygonMode(GL_FRONT_AND_BACK, mPolygonMode);
    glUseProgram(shader_program);


    setupModelView();
    GLint renderWireframe_handle = glGetUniformLocation(shader_program, "RenderWireframe");
    glUniform1i(renderWireframe_handle, 0);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);

    /*
    glUniform1i(renderWireframe_handle, 1);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
     */
    glBindVertexArray(0);
}

bool Mesh::loadModel(const char *filename, std::vector<Vertex *> &mesh) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }

    int vi = 0;
    int vti = 0;
    int vni = 0;
    int fi = 0;

    std::vector<glm::vec3> vertices_temp;
    std::vector<glm::vec3> normals_temp;
    std::vector<glm::vec2> uvs_temp;
    std::vector<unsigned int> faces_vert_temp;
    std::vector<unsigned int> faces_uvs_temp;
    std::vector<unsigned int> faces_normals_temp;

    while (1) {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break;

        // Read a vertex
        if(strcmp(lineHeader, "v")==0){
            glm::vec3 v;
            fscanf(file, "%f %f %f\n", &v.x, &v.y, &v.z );
            vertices_temp.push_back(v);
            mesh.push_back(new Vertex(v.x,v.y,v.z));
            vi++;
        }else
        // Read a vertex tex coord
        if(strcmp(lineHeader, "vt")==0){
            glm::vec2 vt;
            fscanf(file, "%f %f\n", &vt.x, &vt.x);
            uvs_temp.push_back(vt);
            vti++;
        }else
        // Read a vertex normal
        if(strcmp(lineHeader, "vn")==0){
            glm::vec3 n;
            fscanf(file, "%f %f %f\n", &n.x, &n.y, &n.z );
            normals_temp.push_back(n);
            vni++;
        }else
        // Read a face
        if(strcmp(lineHeader, "f")==0){
            unsigned int vi1, uvi1, ni1, vi2, uvi2, ni2, vi3, uvi3, ni3;
            char str[256];
            fscanf(file, " %[^\n]", &str);

            if(strstr(str, "//"))
                sscanf(str, "%i//%i %i//%i %i//%i\n", &vi1, &ni1, &vi2, &ni2, &vi3, &ni3); // vi//uvi//ni
            else
                sscanf(str, "%i/%i/%i %i/%i/%i %i/%i/%i\n", &vi1, &uvi1, &ni1, &vi2, &uvi2, &ni2, &vi3, &uvi3, &ni3); // vi//uvi//ni
            faces_vert_temp.insert(faces_vert_temp.end(),{vi1-1, vi2-1, vi3-1});
            faces_uvs_temp.insert(faces_uvs_temp.end(),{uvi1-1, uvi2-1, uvi3-1});
            faces_normals_temp.insert(faces_normals_temp.end(),{ni1-1, ni2-1, ni3-1});

            m_indices.insert(m_indices.end(), {vi1-1,vi2-1,vi3-1});
            fi++;
        }
    }

    for(size_t i=0; i <faces_vert_temp.size()-3; i+=3){
        Vertex* v1 = mesh[faces_vert_temp[i]];
        Vertex* v2 = mesh[faces_vert_temp[i+1]];
        Vertex* v3 = mesh[faces_vert_temp[i+2]];
        v1->m_normal = normals_temp[faces_normals_temp[i]];
        v2->m_normal = normals_temp[faces_normals_temp[i+1]];
        v3->m_normal = normals_temp[faces_normals_temp[i+2]];
    }
    return 0;
}

void Mesh::createIcosahedron(std::vector<Vertex *> &mesh) {
    float t = 1.0f;
    mesh.insert(mesh.end(), new Vertex(-t, -t, t)); // 0
    mesh.insert(mesh.end(), new Vertex(t, -t, t)); // 1
    mesh.insert(mesh.end(), new Vertex(t, t, t)); // 2
    mesh.insert(mesh.end(), new Vertex(-t, t, t)); // 3

    mesh.insert(mesh.end(), new Vertex(-t, -t, -t));
    mesh.insert(mesh.end(), new Vertex(t, -t, -t));
    mesh.insert(mesh.end(), new Vertex(t, t, -t));
    mesh.insert(mesh.end(), new Vertex(-t, t, -t));

    //Front
    m_indices.insert(m_indices.end(), {0, 1, 3});
    m_indices.insert(m_indices.end(), {1, 2, 3});

    //Back
    m_indices.insert(m_indices.end(), {5, 4, 6});
    m_indices.insert(m_indices.end(), {4, 7, 6});

    //Left
    m_indices.insert(m_indices.end(), {1, 5, 2});
    m_indices.insert(m_indices.end(), {5, 6, 2});

    //Right
    m_indices.insert(m_indices.end(), {4, 0, 7});
    m_indices.insert(m_indices.end(), {0, 3, 7});

    //Top
    m_indices.insert(m_indices.end(), {3, 2, 7});
    m_indices.insert(m_indices.end(), {2, 6, 7});

    //Bottom
    m_indices.insert(m_indices.end(), {0, 4, 1});
    m_indices.insert(m_indices.end(), {1, 4, 5});

    for (unsigned long i = 0; i <= m_indices.size() - 3; i += 3) {
        Triangle *t = new Triangle(mesh[m_indices[i]], mesh[m_indices[i + 1]], mesh[m_indices[i + 2]]);
        m_triangles.push_back(t);
        t->m_p1->m_triangles.insert(t);
        t->m_p2->m_triangles.insert(t);
        t->m_p3->m_triangles.insert(t);
    }

    for (auto v : mesh) {
        glm::vec3 n;
        for (auto t : v->m_triangles)
            n += t->m_normal;
        n /= v->m_triangles.size();
        v->m_normal = n;
    }
}
