//
// Created by Lauri Kortevaara(personal) on 08/11/15.
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
    class Scene {
    public:
        Scene();

        virtual ~Scene();

    public:
		typedef omen::Signal<std::function<void(omen::ecs::Entity* e)> > EntityAdded_t;
		EntityAdded_t signal_entity_added;

		void initialize();

        void render(const glm::mat4 &viewProjection, const glm::mat4 &view);
        std::vector<std::unique_ptr<Light> >& lights() {return m_lights;}

		const std::vector<std::unique_ptr<ecs::Entity>>& entities() { return m_entities; }

		void addEntity(std::unique_ptr<ecs::Entity> entity);
		ecs::Entity* findEntity(const std::string& name);

    private:
        std::vector<std::unique_ptr<ecs::Entity> > m_entities;
        std::vector<std::unique_ptr<Light> > m_lights;
        std::vector<std::unique_ptr<omen::Renderable> > m_renderables;
        Sky*    m_sky;

        std::unique_ptr<Model> loadModel(const std::string filename);

        void render();
    };
} // namespace omen

#endif //OMEN_SCENE_H
