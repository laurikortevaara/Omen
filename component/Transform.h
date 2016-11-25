//
// Created by Lauri Kortevaara on 26/12/15.
//

#ifndef OMEN_TRANSFORM_H
#define OMEN_TRANSFORM_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Component.h"

namespace omen {
    class Transform : public ecs::Component {
        glm::mat4   m_tr;
        glm::vec3   m_scale;
        glm::vec3   m_rotation;
        glm::vec3   m_position;
        glm::vec3   m_bounds_min;
        glm::vec3   m_bounds_max;

    protected:
        virtual void onAttach(ecs::Entity* e);
        virtual void onDetach(ecs::Entity* e);
    public:
        Transform() : m_scale(1), m_tr(1), m_rotation(0), m_position(0) {};
        Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale):
                m_position(position),m_rotation(rotation), m_scale(scale){}
        glm::vec3& pos() { return m_position; }


        explicit Transform(const Transform& tr) {
            m_tr = tr.m_tr;
            m_scale = tr.m_scale;
            m_rotation = tr.m_rotation;
            m_position = tr.m_position;
            m_isEnabled = tr.m_isEnabled;
        }

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

        void setBounds(glm::vec3 min, glm::vec3 max) {m_bounds_max = max; m_bounds_min = min;}
        void getBounds(glm::vec3& min, glm::vec3& max)const {max = m_bounds_max ; min = m_bounds_min;}
    };
} // namespace omen

#endif //OMEN_TRANSFORM_H
