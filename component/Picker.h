//
// Created by Lauri Kortevaara(Intopalo) on 10/01/16.
//

#ifndef OMEN_PICKER_H
#define OMEN_PICKER_H

#include <functional>
#include <GLFW/glfw3.h>
#include <map>
#include "../Signal.h"
#include "Component.h"
#include "../Mesh.h"
#include <glm/vec3.hpp>

namespace Omen {

    class Picker : public ecs::Component {
    public:
        typedef Omen::Signal<std::function<void(Omen::Mesh*)> > ObjectPicked_t;
        ObjectPicked_t signal_object_picked;

    public:
        Picker();

        virtual ~Picker();

        glm::vec3 const &ray() const { return ray_world; }

    private:
        void pick();

        glm::vec3 ray_world;
    };

} // namespace Omen


#endif //OMEN_PICKER_H
