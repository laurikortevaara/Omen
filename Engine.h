//
// Created by Lauri Kortevaara(personal) on 20/12/15.
//

#ifndef GLEW_TEST_ENGINE_H
#define GLEW_TEST_ENGINE_H

#include <string>
#include <glm/detail/type_mat4x4.hpp>

#include "Scene.h"
#include "Camera.h"
#include "Window.h"
#include "Shader.h"
#include "Texture.h"
#include "system/System.h"
#include "TextRenderer.h"

namespace Omen {

    class Engine {
        /** Singleton interface **/
        static Engine* m_instance;
        Engine();

    public:
        static Engine* instance();

        /** Signals **/
    public:
        typedef Signal< std::function<void (double time, double delta_time)> > Update;
        Update signal_engine_update;

        /** Public class interface **/
    public:

        Window *createWindow(unsigned int width, unsigned int height);

        void update();
        void render();
        Window* window() {return m_window;};

        template <class type> type* findSystem(const std::string& system_name="") {
            for(Omen::ecs::System* s : m_systems)

                if(dynamic_cast<type*>(s)!= nullptr)
                    return dynamic_cast<type*>(s);
                return nullptr;
        }

    private:
        Window *m_window;
        Camera *m_camera;
        Scene *m_scene;

        void keyHit(int key, int scanCode, int action, int mods);

        Shader *m_shader;
        GLuint m_vao;
        GLuint m_vbo;
        GLuint m_vbo_texcoord;
        GLuint m_ibo;

        Texture *m_texture;
        Texture *m_texture2;

        GLint m_vcoord_attrib;
        GLint m_tcoord_attrib;

        double time();

        double m_time;
        double m_timeDelta;

        std::vector<ecs::System*> m_systems;


        void initializeSystems();

        TextRenderer *m_text;
        int m_framecounter;

        void renderScene();

        Joystick *m_joystick;
    };
} // namespace Omen


#endif //GLEW_TEST_ENGINE_H
