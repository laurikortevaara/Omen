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
#include "PointLight.h"
#include "Ocean.h"

using namespace Omen;

Scene::Scene() {

    m_sky = new Sky();
    createGround();
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
        loadModel(files.front());
    });

    std::shared_ptr<Model> m = loadModel("models/cube.md3");
    m->m_mesh->m_transform.pos() = glm::vec3(3,0.2,0);


    Engine* e = Engine::instance();
    JoystickInput* ji = (JoystickInput*)e->findComponent<JoystickInput>();
    if(ji!= nullptr){
        ji->joystick_button_pressed.connect([&](Joystick *joystick) {
                
        });
    }
}

std::shared_ptr<Model> Scene::loadModel(const std::string filename){
    Omen::MD3Loader loader;
    //loader.loadModel("models/cube.md3");
    //loader.loadModel("models/ToDPirateHologuise/pirate.md3");
    loader.loadModel(filename);
    //loader.loadModel("models/test.md3");
    std::vector<std::shared_ptr<Omen::Mesh>> meshes;
    loader.getMesh(meshes);
    int i=0;
    std::shared_ptr<Model> model;
    for(auto& mesh : meshes){
        model = std::make_shared<Model>( mesh );
        model->m_mesh->m_amplitude = 0.0;
        model->m_mesh->m_transform.pos().x = i * 3;
        m_models.push_back(model);
        i++;
    }
    return model;
}

Scene::~Scene() {
    m_models.clear();
}

void Scene::createGround() {
    std::string shader_name = "shaders/pass_through.glsl";

    GLfloat s = 50;
    std::vector<glm::vec3> vertices = {
            {-s, 0, -s},
            {s,  0, -s},
            {s,  0, s},
            {-s, 0, s}};
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
    model->m_mesh->m_amplitude = 0.0;
    model->m_mesh->m_frequency = 0.0f;
    m_models.push_back(std::move(model));

    m_lights.push_back( std::make_shared<PointLight>(PointLight({0,0,0}, {1,1,1}, 1) ));

    Engine::instance()->signal_engine_update.connect([&](double time, double dt){
       m_lights.front()->tr().pos().x = (float) cos(time)*5;
        m_lights.front()->tr().pos().y = 5.0;
        m_lights.front()->tr().pos().z = (float) sin(time)*5;
    });

    m_renderables.push_back( std::make_shared<Ocean>(64,0.0005f, glm::vec2(32.0f, 32.0f), 64) );
}

void Scene::render(const glm::mat4 &viewProjection, const glm::mat4 &view) {
    check_gl_error();

    m_sky->render();
    /*for (const auto &model : m_models)
        model->render(viewProjection, view);*/
    for( const auto &r : m_renderables)
        r->render();
}

