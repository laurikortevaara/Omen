//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_ENTITY_H
#define OMEN_ENTITY_H

#include <string>
#include <stdint.h>

namespace Omen {
    namespace ecs {
        class Entity {
            static uint64_t id_counter;
        protected:
            uint64_t m_id;
            std::string m_name;

            Entity(const std::string &name) : m_id(++id_counter), m_name(name) { };

            uint64_t id() const { return m_id; }

        };
    } // namespace ecs
} // namespace Omen




#endif //OMEN_ENTITY_H
