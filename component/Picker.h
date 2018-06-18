//
// Created by Lauri Kortevaara on 10/01/16.
//

#ifndef OMEN_PICKER_H
#define OMEN_PICKER_H

#include <functional>
#include <map>
#include "../Signal.h"
#include "Component.h"
#include "../Mesh.h"
#include <glm/vec3.hpp>
#include "../GameObject.h"

namespace omen {

    class Picker : public ecs::Component {
    protected:
        virtual void onAttach(ecs::Entity* e);
        virtual void onDetach(ecs::Entity* e);
    public:
        typedef omen::Signal<std::function<void(ecs::Entity*, glm::vec3 intersectPos)> > ObjectPicked_t;
        static ObjectPicked_t signal_object_picked;

    public:
        Picker();

        virtual ~Picker();

        glm::vec3 const &ray() const { return ray_world; }

		static glm::vec3 IntersectPos;
		static omen::ecs::Entity* CurrentlySelected;

    private:
        void pick(omen::ecs::Entity* parentEntity=nullptr);

        glm::vec3 ray_world;
    };

} // namespace omen


#endif //OMEN_PICKER_H
