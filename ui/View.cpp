//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#include "View.h"
#include "../Engine.h"

using namespace omen;
using namespace ui;

View::View(View *parentView, const std::string& name) :
    ecs::Entity(name),
    m_parentView(parentView) {
    if(m_parentView!= nullptr)
        m_parentView->addChildView(this);
}


View::~View() {

}

void View::addChildView(View *pView) {
    m_childViews.push_back(pView);
    updateLayout();
}

float View::width() {
    return m_width;
}

float View::height() {
    return m_height;
}

float View::x() {
    return m_x;
}

float View::y() {
    return m_y;
}

float View::z() {
    return m_z;
}

glm::vec2 View::size() {
    return {m_width,m_height};
}

glm::vec2 View::pos() {
    return {m_x,m_y};
}

void View::setWidth(float width) {
    m_width = width;

}

void View::setHeight(float height) {
    m_height = height;
}

void View::measureSize() {
    float maxWidth = 0;
    float maxHeight = 0;

    if(m_parentView== nullptr){
        Engine* e = Engine::instance();
        Window* w = e->window();
        maxWidth = static_cast<omen::floatprec>(w->width());
        maxHeight = static_cast<omen::floatprec>(w->height());
    }
    else {
        maxWidth = m_parentView->width();
        maxHeight = m_parentView->height();
    }
    onMeasure(maxWidth,maxHeight);
}

void View::onMeasure(float maxwidth, float maxheight) {

}

void View::onLayout(bool changed, float left, float top, float right, float bottom) {

}
