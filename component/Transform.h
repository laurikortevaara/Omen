//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_TRANSFORM_H
#define OMEN_TRANSFORM_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Component.h"

namespace Omen {
    class Transform : public ecs::Component {
        glm::mat4 m_tr;
    public:
        glm::vec4 pos() const { return m_tr[3]; }

        glm::vec4 rot_x() const { return m_tr[0]; }

        glm::vec4 rot_y() const { return m_tr[1]; }

        glm::vec4 rot_z() const { return m_tr[2]; }

        glm::mat4 tr() const { return m_tr; }

        virtual void update(double time, double deltaTime);

        glm::mat4& translate(const glm::vec3 &translation) { m_tr = glm::translate(m_tr, translation); return m_tr;}
        glm::mat4& rotate(const glm::vec3 &axis, float angle) { m_tr = glm::rotate(m_tr, angle, axis); return m_tr;}
        glm::mat4& scale(const glm::vec3 &scale) { m_tr = glm::scale(m_tr, scale); return m_tr; }
    };
} // namespace Omen

#endif //OMEN_TRANSFORM_H
