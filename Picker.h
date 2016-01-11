//
// Created by Lauri Kortevaara(Intopalo) on 10/01/16.
//

#ifndef OMEN_PICKER_H
#define OMEN_PICKER_H

#include <glm/glm.hpp>
#include "component/Component.h"

namespace Omen {

class Picker : public ecs::Component {
public:
    Picker();
    virtual ~Picker();

    glm::vec3 const& ray() const { return ray_world; }
private:
    void pick();

    glm::vec3 ray_world;
};

} // namespace Omen


#endif //OMEN_PICKER_H
