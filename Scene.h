//
// Created by Lauri Kortevaara on 08/11/15.
//

#ifndef OMEN_SCENE_H
#define OMEN_SCENE_H


#include <vector>
#include "Model.h"
#include "Light.h"
#include "Sky.h"
#include "Renderable.h"
#include "Entity.h"
#include "GameObject.h"

namespace omen {
    class Scene : public omen::Object {
    public:
        Scene();

        virtual ~Scene();

    public:
		typedef omen::Signal<std::function<void(omen::ecs::Entity* e)> > EntityAdded_t;
		EntityAdded_t signal_entity_added;

		void initialize();

		void renderBackground();
		void renderArrow();

        void render(const glm::mat4 &viewProjection, const glm::mat4 &view);
        std::vector<std::unique_ptr<Light> >& lights() {return m_lights;}

		const std::vector<std::unique_ptr<ecs::Entity>>& entities(GLuint layer = 0 ) { return m_entities[layer]; }

		void addEntity(std::unique_ptr<ecs::Entity> entity, GLuint layer = 0);
		ecs::Entity* findEntity(const std::string& name);

    private:
        std::map<GLuint, std::vector<std::unique_ptr<ecs::Entity> > > m_entities;
        std::vector<std::unique_ptr<Light> > m_lights;
        std::vector<std::unique_ptr<omen::Renderable> > m_renderables;
        Sky*    m_sky;

        std::unique_ptr<Model> loadModel(const std::string filename);
		std::unique_ptr<ecs::GameObject> createObject(const std::string& filename);

        void render();
    };
} // namespace omen

#endif //OMEN_SCENE_H
