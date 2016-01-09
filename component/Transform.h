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
        glm::mat4   m_tr;
        glm::vec3   m_scale;
        glm::vec3   m_rotation;
        glm::vec3   m_position;
    public:
        Transform() : m_scale(1), m_tr(1), m_rotation(0), m_position(0) {};
        glm::vec3 pos() const  { return m_position; }
        glm::vec3& pos()  { return m_position; }

        glm::mat4 tr() {
            glm::mat4 translation = glm::translate(glm::mat4(1), m_position);
            //glm::mat4 rotation = glm::rotate( glm::mat4(), m_rotation.x, glm::vec3(1,0,0));
            glm::mat4 rotation = glm::rotate( glm::mat4(), m_rotation.y, glm::vec3(0,1,0));
            //rotation = glm::rotate( rotation, m_rotation.y, glm::vec3(0,0,1));
            glm::mat4 scale = glm::scale(glm::mat4(1), m_scale);

            m_tr = translation * rotation * scale;
            return m_tr;
        }

        virtual void update(double time, double deltaTime);

        glm::mat4 translate(const glm::vec3 &translation) { m_position += translation; return tr(); }
        glm::mat4 rotate(float angle, const glm::vec3 &axis) { m_rotation.y = angle; return m_tr;}
        glm::mat4 scale(const glm::vec3 &scale) { m_tr = glm::scale(m_tr, scale); return m_tr; }
    };
} // namespace Omen

#endif //OMEN_TRANSFORM_H
