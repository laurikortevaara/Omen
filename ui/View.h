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
			
			enum MARGIN_POS {
				left = 0,
				top = 1,
				right = 2,
				bottom = 3
			};
			glm::vec4 m_margins; // left, top, right, bottom;
			
			virtual glm::vec3 pos() const { return Entity::pos() + glm::vec3(m_margins[left], m_margins[top], 0); }
			virtual glm::vec2 pos2D() const { return Entity::pos2D() + glm::vec2(m_margins[left], m_margins[top]); };

			virtual glm::vec3 localPos() const { return Entity::tr_const()->pos(); }
			virtual void setLocalPos(const glm::vec3& pos) { Entity::tr()->setPos(pos); }

			virtual glm::vec2 localPos2D() const { return glm::vec2(Entity::tr_const()->pos()); }
			virtual void setLocalPos2D(const glm::vec2& pos) { Entity::tr()->setPos(glm::vec3(pos, Entity::tr()->pos().z)); }

			virtual float width() const { return Entity::width();}
			virtual float height() const { return Entity::height(); }

		};
    } // namespace UI
} // namespace omen

#endif //OMEN_VIEW_H
