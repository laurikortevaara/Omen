//
// Created by Lauri Kortevaara(personal) on 26/12/15.
//

#ifndef OMEN_COMPONENT_H
#define OMEN_COMPONENT_H

namespace Omen {
    namespace ecs {
        class Component {
        protected:
            bool m_isEnabled;
        public:
            Component() : m_isEnabled(true) { };

            virtual ~Component() { };

            virtual void update(double time, double deltaTime) { };

            bool enabled() const { return m_isEnabled; }

            void setEnabled(bool enabled) { m_isEnabled = enabled; }
        };
    } // namespace ecs
} // namespace Omen


#endif //OMEN_COMPONENT_H
