//
// Created by Lauri Kortevaara(personal) on 08/11/15.
//

#include <iostream>
#include "Scene.h"
#include "Model.h"
#include "utils.h"
#include "WavefrontLoader.h"
#include "GL_error.h"
#include "Engine.h"
#include "MD3Loader.h"

using namespace Omen;

Scene::Scene() {
    std::string shader_name = "shaders/pass_through.glsl";

    GLfloat s = 50;
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
     std::vector<GLsizei> indices  = {0, 3, 1, 2, 1, 3};

     Material* material = new Material();
     material->setTexture(new Texture("textures/checker.jpg"));

     std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(shader_name, material, vertices, normals, texcoords, indices);
     std::unique_ptr<Model> model = std::make_unique<Model>(std::move(mesh));
     model->m_mesh->m_position = glm::vec3(0,0,0);
     model->m_mesh->m_amplitude = 0.0;
     model->m_mesh->m_frequency = 0.0f;
     m_models.push_back(std::move(model));


    /*
    {
        WavefrontLoader loader;
        loader.loadObj("models/ToDPirateHologuise/PirateHologuise.obj");

        for (auto mesh : loader.meshes) {
            std::unique_ptr<Mesh> mp = mesh->getMesh();
            std::unique_ptr<Model> model = std::make_unique<Model>(std::move(mp));
            model->m_mesh->m_position = glm::vec3(Omen::random(-10, 10), 5, Omen::random(-10, 10));
            model->m_mesh->m_amplitude = 2.0;
            m_models.push_back(std::move(model));
        }
        check_gl_error();
    }
    */
    Engine::instance()->window()->signal_file_dropped.connect( [this](std::vector<std::string> files)
    {
        Omen::MD3Loader loader;
        //loader.loadModel("models/cube.md3");
        //loader.loadModel("models/ToDPirateHologuise/pirate.md3");
        loader.loadModel(*files.begin());
        //loader.loadModel("models/test.md3");
        std::vector<std::unique_ptr<Omen::Mesh>> meshes;
        loader.getMesh(meshes);
        int i=0;
        for(auto& mesh : meshes){
            std::unique_ptr<Model> model = std::make_unique<Model>( std::move(mesh) );
            model->m_mesh->m_amplitude = 0.0;
            model->m_mesh->m_position.x = i * 3;
            m_models.push_back(std::move(model));
            i++;
        }

    });



    Engine* e = Engine::instance();
    JoystickInput* ji = (JoystickInput*)e->findComponent<JoystickInput>();
    if(ji!= nullptr){
        ji->joystick_button_pressed.connect([&](Joystick *joystick) {
                
        });
    }

}

Scene::~Scene() {
    m_models.clear();
}

void Scene::render(const glm::mat4 &viewProjection, const glm::mat4 &view) {
    check_gl_error();

    for (const auto &model : m_models)
        model->render(viewProjection, view);
}

