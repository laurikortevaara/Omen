//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#include "View.h"
#include "../Engine.h"

using namespace omen;
using namespace ui;

View::View(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size) :
	ecs::Entity(name),
	m_parentView(parentView)
{
	Entity::setParent(parentView);
	setLayer(1);

	setLocalPos2D(pos);
	setSize2D(size);
	tr()->pos().x = pos.x;
	tr()->pos().y = pos.y;
	tr()->pos().z = -1;
	tr()->setBounds(glm::vec3(0, 0, 0), glm::vec3(size, 0));

	setGravity(ALIGN_LEFT | ALIGN_TOP);

	Engine::instance()->window()->signal_window_size_changed.connect([this](int width, int height) {this->updateLayout();});
}


View::~View() {

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
	updateLayout();
}

void View::onLayout(bool changed, float left, float top, float right, float bottom) {
	updateLayout();
}