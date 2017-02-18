//
// Created by Lauri Kortevaara on 20/12/15.
//

#ifndef GLEW_TEST_ENGINE_H
#define GLEW_TEST_ENGINE_H

#include <string>
#include <iostream>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <future>
#include <btBulletDynamicsCommon.h>

#include "Scene.h"
#include "Camera.h"
#include "Window.h"
#include "Shader.h"
#include "Texture.h"
#include "system/System.h"
#include "ui/Button.h"


namespace omen {

	typedef float floatprec;

	namespace ecs {
		class TextRenderer;
	}

    class Engine {
#define post_function Engine::instance()->post(std::make_unique<std::function<void()>>(std::function<void()>([&](void)
        /** Singleton interface **/
        static Engine* m_instance;
        Engine();
		~Engine();

    public:


		static GLuint ShadowBlur;
		static GLfloat ShadowFrustumNear, ShadowFrustumFar, LightDistance;
		static GLfloat ShadowFrustumSize;
		static GLint blend_mode;
		static glm::vec3 MousePickRay;
		static glm::vec3 LightPos;

		static float     AmbientFactor;
		static float     MaterialShininess;
		static float     LightIntensity;
		static float	 LightAzimuthAngle;
		static float	 LightZenithAngle;
		/*static long left_bytes;
		static long left_kbytes;
		static long left_mbytes;*/
        static Engine* instance();

        /** Signals **/
        Scene *scene();

        GLenum getPolygonMode();
		static void getTextureMemoryInfo();
		void ray_cast_mouse();

    public:
        typedef Signal< std::function<void (omen::floatprec time, omen::floatprec delta_time)> > Update;
        Update signal_engine_update;

        /** Public class interface **/
    public:

        const std::unique_ptr<Window>& createWindow(unsigned int width, unsigned int height);

		omen::floatprec time();
        void update();
        void render();
        Window* window() {return m_window.get();};
        Camera* camera() {return m_camera;};

        template <class type> type* findSystem(const std::string& system_name="") {
            for(omen::ecs::System* s : m_systems)

                if(dynamic_cast<type*>(s)!= nullptr)
                    return dynamic_cast<type*>(s);
                return nullptr;
        }

        template <class type> type* findComponent(const std::string& component_name="") {
            for(omen::ecs::System* s : m_systems){
                ecs::Component* c = s->findComponent<type>(component_name);
                if(c != nullptr)
                    return dynamic_cast<type*>(c);
            }

            return nullptr;
        }

        class t_future_task {
        public:
			t_future_task( std::unique_ptr<std::function<void()>> f, std::chrono::duration<omen::floatprec> timeout, bool repeat)
			: fun(std::move(f)), delay(timeout), repeating(repeat){

			}
			static std::mutex task_mutex;
            std::chrono::time_point<std::chrono::system_clock> task_created;
            std::chrono::duration<omen::floatprec> delay;
			bool repeating;
            std::shared_future<void> task;
			std::unique_ptr<std::function<void()>> fun;

			void run() {
				task = std::async(std::launch::async, *fun);
				task.get();
			}
			void reset() { 
				task_created = std::chrono::system_clock::now(); 
			}
			bool pending() const {
				std::chrono::duration<omen::floatprec> diff = std::chrono::system_clock::now() - task_created;
				return  diff.count() < delay.count();
			}
        } ;
        std::vector< t_future_task > m_future_tasks; // key,val == [seconds, task]
        void post(std::unique_ptr<std::function<void()>> task, omen::floatprec delay = 0.0, bool repeating = false );

		const omen::floatprec fps() const { return m_fps; }
		const omen::floatprec averageFps() const { return m_avg_fps; }
    private:
        std::unique_ptr<Window> m_window;
        Camera *m_camera;
        ui::Button *m_button;
        std::unique_ptr<Scene> m_scene;
        std::vector<ecs::System*> m_systems;
        std::unique_ptr<ecs::TextRenderer> m_text;


        void keyHit(int key, int scanCode, int action, int mods);

		omen::floatprec m_time;
		omen::floatprec m_timeDelta;
		omen::floatprec m_fps;
		omen::floatprec m_avg_fps;



        void initializeSystems();

        int m_framecounter;

        void renderScene();

        Joystick *m_joystick;

        void doPhysics(omen::floatprec dt);

        void handle_task_queue();

        GLclampf m_sample_coverage;

        bool createFramebuffer();

        GLuint m_frame_buffer;
        GLuint m_colorTexture;
        GLuint m_depthTexture;

        omen::Model* m_currentSelection;

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
		omen::floatprec							m_mouse_x, m_mouse_y;

        void initPhysics();

        void renderText();

        GLenum m_polygonMode;

        bool createShadowFramebuffer();
    };
} // namespace omen


#endif //GLEW_TEST_ENGINE_H
