//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#ifndef OMEN_VIEW_H
#define OMEN_VIEW_H


#include <glm/glm.hpp>
#include <vector>
#include "../Signal.h"
#include "../Entity.h"

namespace omen {
    namespace ui {
        class View : public ecs::Entity {
        public:
        protected:
            View *m_parentView;

            View(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size);

            virtual void updateLayout() = 0;
            virtual void onMeasure(float maxwidth, float maxheight) = 0;
            virtual void onLayout(bool changed, float left, float top, float right, float bottom);
        public:
            virtual ~View();

            void measureSize();

			View* parentView() const { return m_parentView; }

            // Layout
            void layout(float left, float top, float right, float bottom);

			glm::vec4 margins() const { return m_margins; }
			void setMargins(const glm::vec4& margins) { m_margins = margins; }
			
			glm::vec4 m_margins; // left, top, right, bottom;
			glm::vec2 m_size;
			glm::vec2 size() const { return m_size; }
			void setSize(const glm::vec2& size) { m_size = size; }
		};
    } // namespace UI
} // namespace omen

#endif //OMEN_VIEW_H
