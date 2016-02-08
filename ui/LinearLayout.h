//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#ifndef OMEN_LINEARLAYOUT_H
#define OMEN_LINEARLAYOUT_H


#include "Layout.h"
#include "View.h"

namespace omen {
    namespace ui {
        class LinearLayout : public Layout {
        public:
            typedef enum {
                VERTICAL, HORIZONTAL
            } LayoutDirection;

            LinearLayout(View *parentView, const std::string& name, LayoutDirection dir = VERTICAL);

            void setLayoutDirection(LayoutDirection dir);

            LayoutDirection &layoutDirection();

        protected:
            LayoutDirection m_layoutDirection;

            virtual void updateLayout();

        private:
        };

    } // namespace ui
} // namespace omen


#endif //OMEN_LINEARLAYOUT_H
