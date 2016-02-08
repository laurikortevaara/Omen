//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#include "LinearLayout.h"

using namespace omen;
using namespace ui;

LinearLayout::LinearLayout(View *parentView, const std::string& name, LinearLayout::LayoutDirection dir) :
    Layout(parentView, name),
    m_layoutDirection(dir)
{

}

void LinearLayout::updateLayout() {
    float y = 0;
    float x = 0;
    for(auto childView : m_childViews) {
        switch(m_layoutDirection){
            case VERTICAL:
                break;
            case HORIZONTAL:
                break;
        }
        x += childView->width();
        y += childView->height();
    }
}

void LinearLayout::setLayoutDirection(LinearLayout::LayoutDirection dir) {
    m_layoutDirection = dir;
}

LinearLayout::LayoutDirection &LinearLayout::layoutDirection() {
    return m_layoutDirection;
}

