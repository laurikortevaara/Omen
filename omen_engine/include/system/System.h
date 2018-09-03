//
// Created by Lauri Kortevaara on 26/12/15.
//

#ifndef OMEN_SYSTEM_H
#define OMEN_SYSTEM_H

#include "Object.h"

#include <string>
#include <vector>
#include <algorithm>

namespace omen {
    namespace ecs {
        class Component;

        class System : public omen::Object {
        private:
            std::vector<Component *> m_components;
		
		protected:
			void add(Component *component);
			std::vector<Component *>& components() {
				return m_components;
			};

        public:
            System();

            virtual ~System() { };
						
			virtual void addComponent(Component* component) {
				add(component);
			};

			void detachFromSystem(Component* component);
			virtual void shutDown() = 0;

            void remove(Component *component) {
                std::vector<Component *>::iterator c = std::find(m_components.begin(), m_components.end(), component);
                if (c != m_components.end())
                    m_components.erase(c);
            };

            void update(double time, double deltaTime);

            template<class type>
            type *findComponent(const char* component_name = nullptr) {
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
