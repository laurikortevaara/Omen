//
// Created by Lauri Kortevaara on 08/11/15.
//

#ifndef OMEN_EDITOR_SCENE_H
#define OMEN_EDITOR_SCENE_H


#include <vector>
#include "../GameObject.h"
#include "../Model.h"
#include "../Renderable.h"
#include "../Entity.h"


namespace omen {
    class EditorScene : public omen::Object {
    public:
		EditorScene();

        virtual ~EditorScene();

    public:
		typedef omen::Signal<std::function<void(omen::ecs::Entity* e)> > EntityAdded_t;
		EntityAdded_t signal_entity_added;

		void initialize();


        void render(const glm::mat4 &viewProjection, const glm::mat4 &view);

		const std::vector<std::unique_ptr<ecs::Entity>>& entities(GLuint layer = 0 ) { return m_entities[layer]; }

		void addEntity(std::unique_ptr<ecs::Entity> entity, GLuint layer = 0);
		ecs::Entity* findEntity(const std::string& name);

    private:
        std::map<GLuint, std::vector<std::unique_ptr<ecs::Entity> > > m_entities;
        std::vector<std::unique_ptr<omen::Renderable> > m_renderables;
    };
} // namespace omen

#endif //OMEN_EDITOR_SCENE_H
