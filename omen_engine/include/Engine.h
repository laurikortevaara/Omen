//
// Created by Lauri Kortevaara on 20/12/15.
//

#ifndef OMEN_ENGINE_H
#define OMEN_ENGINE_H

#include "Scene.h"
#include "Camera.h"
#include "Window.h"
#include "Shader.h"
#include "Texture.h"
#include "Property.h"
#include "typedef.h"
#include "system/System.h"

#include <list>
#include <string>
#include <any>
#include <future>

#ifdef BUILD_BULLET_PHYSICS
#pragma warning(push)
#pragma warning(disable:4305)
#include <btBulletDynamicsCommon.h>
#pragma warning(pop)
#endif


namespace omen {

    class EditorScene;

	namespace ecs {
		class TextRenderer;
	}

	class Properties {
	private:
		std::map<std::string, Property> properties;
	public:
		Property& operator[](const char* ch) {
			return operator[](std::string(ch));
		}
		Property& operator[](std::string& name) {
			if (properties.find(name) == properties.end())
				properties[name] = Property(name.c_str());
			return properties[name];
		}
	};
		
	class Engine : public Object {
#define post_function Engine::instance()->post(std::make_unique<std::function<void()>>(std::function<void()>([&](void)
		/** Singleton interface **/
		static Engine* m_instance;
		Engine();
		~Engine();

		Properties m_properties;
	public:
		static uint32_t ShadowBlur;
		static float    ShadowFrustumNear, ShadowFrustumFar, LightDistance;
		static float    ShadowFrustumSize;
		static int32_t  blend_mode;
		static glm::vec3     MousePickRay;
		static glm::vec3     LightPos;
		static float    CameraSensitivity;

		Properties& properties() { return m_properties; }

		static float     AmbientFactor;
		static float     MaterialShininess;
		static float     LightIntensity;
		static float	 LightAzimuthAngle;
		static float	 LightZenithAngle;

		/*static long left_bytes;
		static long left_kbytes;
		static long left_mbytes;*/
        static Engine* instance();

        Scene *scene();
		EditorScene *editorScene();

        uint32_t getPolygonMode();
		static void getTextureMemoryInfo();
		void ray_cast_mouse();
		bool meshRendererEnabled() const { return m_bMeshRendererEnabled; }

    public:
        typedef Signal<std::function<void (omen::floatprec time, omen::floatprec delta_time)> > Update;
        Update signal_engine_update;

		typedef Signal<std::function<void()> > ShutDown;
		ShutDown signal_engine_shut_down;

        /** Public class interface **/
    public:

        const std::unique_ptr<Window>& createWindow(unsigned int width, unsigned int height);

		omen::floatprec time();
        void update();
        void render();
        Window* window() {return m_current_window;};
        Camera* camera() {return m_camera;};
		void setViewport(int x, int y, int w, int h);

        template <class type> type* findSystem(const char* system_name=nullptr) {
            for(omen::ecs::System* s : m_systems)

                if(dynamic_cast<type*>(s)!= nullptr)
                    return dynamic_cast<type*>(s);
                return nullptr;
        }

        template <class type> type* findComponent(const char* component_name=nullptr) {
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
            std::future<void> task;
			std::unique_ptr<std::function<void()>> fun;

			static int l;
			
			void run() {
				++l;
				//std::cout << "Static int i: " << l << '\n';
				/*task =*/// std::async(std::launch::async, *fun);
				//task.get();
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
		Window* m_current_window;
        std::list<std::unique_ptr<Window>> m_windows;
        
		Camera *m_camera;

        std::unique_ptr<Scene> m_scene;
		std::unique_ptr<EditorScene> m_editorScene;
        std::vector<ecs::System*> m_systems;
        std::unique_ptr<ecs::TextRenderer> m_text;
		bool m_is_shutting_down;
		bool m_waiting_for_shut_down;
		bool m_bMeshRendererEnabled;

		void shutDown() { m_is_shutting_down = true; }
        void keyHit(int key, int scanCode, int action, int mods);

		omen::floatprec m_time;
		omen::floatprec m_timeDelta;
		omen::floatprec m_fps;
		omen::floatprec m_avg_fps;



        void initializeSystems();

        int m_framecounter;

        void renderScene();

        Joystick *m_joystick;

        void handle_task_queue();

        clampf m_sample_coverage;

        bool createFramebuffer();

        uint32_t m_frame_buffer;
        uint32_t m_colorTexture;
        uint32_t m_depthTexture;

        omen::Model* m_currentSelection;
        omen::floatprec							m_mouse_x, m_mouse_y;

#ifdef BUILD_OMEN_PHYSICS_SYSTEM
    #ifdef BUILD_BULLET_PHYSICS
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
    #endif // BUILD_BULLET_PHYSICS
        void initPhysics();
        void doPhysics(omen::floatprec dt);
#endif //BUILD_OMEN_PHYSICS_SYSTEM

        void renderText();

        uint32_t m_polygonMode;

        bool createShadowFramebuffer();
    };
} // namespace omen


#endif //OMEN_ENGINE_H
