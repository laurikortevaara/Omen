//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_SYSTEM_H
#define OMEN_SYSTEM_H

#include <string>
#include <vector>
#include <algorithm>
#include "../component/Component.h"

namespace omen {
    namespace ecs {
        class System {
        protected:
            std::vector<Component *> m_components;

        public:
            System();

            virtual ~System() { };

            void add(Component *component) { m_components.push_back(component); };

            void remove(Component *component) {
                std::vector<Component *>::iterator c = std::find(m_components.begin(), m_components.end(), component);
                if (c != m_components.end())
                    m_components.erase(c);
            };

            void update(double time, double deltaTime);

            template<class type>
            type *findComponent(const std::string &component_name = "") {
                for (auto c : m_components)
                    if (dynamic_cast<type *>(c) != nullptr)
                        return dynamic_cast<type *>(c);
                return nullptr;
            }

            bool m_isEnabled;
        };
    }
}

#endif //OMEN_SYSTEM_H
