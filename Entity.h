//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_ENTITY_H
#define OMEN_ENTITY_H

#include <cstdint>
#include <string>

namespace Omen {
    namespace ecs {
        class Entity {
            static std::uint64_t id_counter;
        protected:
            std::uint64_t m_id;
            std::string m_name;

            Entity(const std::string &name) : m_id(++id_counter), m_name(name) { };

            std::uint64_t id() const { return m_id; }

        };
    } // namespace ecs
} // namespace Omen




#endif //OMEN_ENTITY_H
