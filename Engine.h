//
// Created by Lauri Kortevaara(personal) on 20/12/15.
//

#ifndef GLEW_TEST_ENGINE_H
#define GLEW_TEST_ENGINE_H

#include <string>
#include <glm/detail/type_mat4x4.hpp>
#include <future>
#include <btBulletDynamicsCommon.h>

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

        std::shared_ptr<Window> createWindow(unsigned int width, unsigned int height);

        double time();
        void update();
        void render();
        Window* window() {return m_window.get();};
        Camera* camera() {return m_camera;};

        template <class type> type* findSystem(const std::string& system_name="") {
            for(Omen::ecs::System* s : m_systems)

                if(dynamic_cast<type*>(s)!= nullptr)
                    return dynamic_cast<type*>(s);
                return nullptr;
        }

        template <class type> type* findComponent(const std::string& component_name="") {
            for(Omen::ecs::System* s : m_systems){
                ecs::Component* c = s->findComponent<type>(component_name);
                if(c != nullptr)
                    return dynamic_cast<type*>(c);
            }

            return nullptr;
        }

        class t_future_task {
        public:
            std::chrono::time_point<std::chrono::system_clock> task_created;
            std::chrono::duration<double> delay;
            std::future<void> task;

            bool pending() const {return  (std::chrono::system_clock::now()-task_created) >= delay; }
        } ;
        std::vector< t_future_task > m_future_tasks; // key,val == [seconds, task]
        void post(std::future<void> task, double delay = 0.0 );


    private:
        std::shared_ptr<Window> m_window;
        Camera *m_camera;
        std::unique_ptr<Scene> m_scene;
        std::vector<ecs::System*> m_systems;
        std::unique_ptr<TextRenderer> m_text;


        void keyHit(int key, int scanCode, int action, int mods);

        double m_time;
        double m_timeDelta;



        void initializeSystems();

        int m_framecounter;

        void renderScene();

        Joystick *m_joystick;

        void doPhysics(double dt);

        void handle_task_queue();

        GLclampf m_sample_coverage;

        bool createFramebuffer();

        GLuint m_frame_buffer;
        GLuint m_depthTexture;

        Omen::Model* m_currentSelection;

        // Bullet phycics
        btBroadphaseInterface*                  m_broadphase;
        btDefaultCollisionConfiguration*        m_collisionConfiguration;
        btCollisionDispatcher*                  m_dispatcher;
        btSequentialImpulseConstraintSolver*    m_solver;
        btDiscreteDynamicsWorld*                m_dynamicsWorld;
        btCollisionShape*                       m_groundShape;
        btCollisionShape*                       m_fallShape;
        btDefaultMotionState*                   m_groundMotionState;
        btRigidBody*                            m_groundRigidBody;
        btDefaultMotionState*                   m_fallMotionState;
        btRigidBody*                            m_fallRigidBody;

        void initPhysics();
    };
} // namespace Omen


#endif //GLEW_TEST_ENGINE_H
