//
// Created by Lauri Kortevaara on 06/02/16.
//

#ifndef OMEN_LINEARLAYOUT_H
#define OMEN_LINEARLAYOUT_H


#include "Layout.h"
#include "View.h"
#include <string>

namespace omen {
    namespace ui {
        class LinearLayout : public Layout {
        public:
            typedef enum {
                VERTICAL, HORIZONTAL
            } LayoutDirection;

            LinearLayout(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size, LayoutDirection dir = VERTICAL );

            void setLayoutDirection(LayoutDirection dir);

            LayoutDirection &layoutDirection();

			virtual bool addChild(std::unique_ptr<Entity> e);

        protected:
            LayoutDirection m_layoutDirection;

			virtual void updateLayout();
			virtual void onMeasure(MeasureSpec horintalMeas, MeasureSpec verticalMeas) {};

        private:
        };

    } // namespace ui
} // namespace omen


#endif //OMEN_LINEARLAYOUT_H
