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
            float m_x, m_y, m_z;
            float m_width, m_height;

            View *m_parentView;
            std::vector<View *> m_childViews;

            View(View *parentView, const std::string& name);

            virtual void updateLayout() = 0;
            virtual void onMeasure(float maxwidth, float maxheight) = 0;
            virtual void onLayout(bool changed, float left, float top, float right, float bottom);

        public:
            virtual ~View();

            void measureSize();
            float width();

            float height();

            void setWidth(float width);

            void setHeight(float height);

            void resize(float width, float height);

            float x();

            float y();

            float z();

            glm::vec2 pos();
			void setPos(const glm::vec2& pos) { m_x = pos.x; m_y = pos.y; }

            glm::vec2 size();
			void setSize(const glm::vec2& size) { m_width = size.x; m_height = size.y; }

            void addChildView(View *pView);

			View* parentView() const { return m_parentView; }

            // Layout
            void layout(float left, float top, float right, float bottom);
        };
    } // namespace UI
} // namespace omen

#endif //OMEN_VIEW_H
