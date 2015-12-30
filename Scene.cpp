//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include <iostream>
#include "Scene.h"
#include "Model.h"
#include "utils.h"
#include "WavefrontLoader.h"
#include "GL_error.h"

using namespace Omen;

Scene::Scene() {
    std::string shader_name = "shaders/pass_through.glsl";

    GLfloat s = 1000;
     std::vector<glm::vec3> vertices = {
             {-s, -1, -s},
             {s,  -1, -s},
             {s,  -1, s},
             {-s, -1, s}};
     std::vector<glm::vec3> normals = {
             {0, 1, 0},
             {0, 1, 0},
             {0, 1, 0},
             {0, 1, 0}};
     std::vector<glm::vec2> texcoords = {
             {0, 0},
             {s, 0},
             {s, s},
             {0, s}};
     std::vector<GLsizei> indices  = {0, 1, 3, 1, 2, 3};

     Material* material = new Material();
     material->setTexture(new Texture("textures/checker.jpg"));

     /*std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(shader_name, material, vertices, normals, texcoords, indices);
     std::unique_ptr<Model> model = std::make_unique<Model>(std::move(mesh));
     model->m_mesh->m_position = glm::vec3(0,0,0);
     model->m_mesh->m_amplitude = 0.0;*/
     //m_models.push_back(std::move(model));


    /////
    WavefrontLoader loader;
    loader.loadObj("models/test.obj");

    for(auto mesh : loader.meshes){
        std::unique_ptr<Mesh> mp = std::move(mesh->getMesh());
        m_models.push_back(std::move(std::make_unique<Model>(
                std::move(mp) ) ) );
    }

    ////

   /* for (int i = 0; i < 1; ++i) {
        std::cout << "Creating model: " << i << std::endl;
        std::unique_ptr<Model> m = std::make_unique<Model>();
        m->m_mesh->m_position = glm::vec3(Omen::random(-40,40), Omen::random(0,0), Omen::random(-40,40) );
        m_models.push_back(std::move(m));
    }*/

    check_gl_error();

}

Scene::~Scene() {
    m_models.clear();
}

void Scene::render(const glm::mat4 &viewProjection) {
    check_gl_error();
    for (const auto &model : m_models)
        model->render(viewProjection);
}

