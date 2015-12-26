//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_TRANSFORM_H
#define OMEN_TRANSFORM_H


#include <glm/glm.hpp>

namespace Omen {
    class Transform {
        glm::mat4 m_tr;
    public:
        glm::vec4 pos() const { return m_tr[3]; }

        glm::vec4 rot_x() const { return m_tr[0]; }

        glm::vec4 rot_y() const { return m_tr[1]; }

        glm::vec4 rot_z() const { return m_tr[2]; }

        glm::mat4 tr() const { return m_tr; }
    };
} // namespace Omen

#endif //OMEN_TRANSFORM_H
