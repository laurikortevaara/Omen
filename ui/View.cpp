//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#include "View.h"
#include "../Engine.h"

using namespace omen;
using namespace ui;

View::View(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size) :
	ecs::Entity(name), m_resizeMode(DEFAULT_RESIZE_MODE)
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

	Engine::instance()->window()->signal_window_size_changed.connect(this,[this](int width, int height) {
		this->updateLayout();
	});
}


View::~View() {

}

void View::measureSize() {
    float maxWidth = 0;
    float maxHeight = 0;

    if(parent()== nullptr){
        Engine* e = Engine::instance();
        Window* w = e->window();
        maxWidth = static_cast<omen::floatprec>(w->width());
        maxHeight = static_cast<omen::floatprec>(w->height());
    }
    else {
        maxWidth = parent()->width();
        maxHeight = parent()->height();
    }
    onMeasure(maxWidth,maxHeight);
}

//
// Called to determine the size requirements for this view and all of its children.
//
void View::onMeasure(float maxwidth, float maxheight) 
{
	updateLayout();
}

//
// Called when this view should assign a size and position to all of its children. 
//
void View::onLayout(bool changed, float left, float top, float right, float bottom) 
{
	updateLayout();
}

//
// Called when this view should assign a size and position to all of its children. 
//
void View::updateLayout()
{
	// Update width if needed for content wrapping mode
	float children_width = 0.0f;
	float children_height = 0.0f;
	if (!children().empty() && (m_layoutParams.layoutSizingWidth == LayoutParams::WrapContent || m_layoutParams.layoutSizingHeight == LayoutParams::WrapContent))
	{
		for (auto& c : children())
		{
			children_width = fmax(children_width, c->pos().x+c->width());
			children_height = fmax(children_height, c->pos().y+c->height());
		}
		if (m_layoutParams.layoutSizingWidth == LayoutParams::WrapContent)
			setWidth(children_width);
		if (m_layoutParams.layoutSizingHeight == LayoutParams::WrapContent)
			setHeight(children_height);
	}
	if (!children().empty() && (m_layoutParams.layoutSizingWidth == LayoutParams::MatchParent || m_layoutParams.layoutSizingHeight == LayoutParams::MatchParent))
	{
		for(auto& c : children())
		{
			c->setSize2D(size2D());
		}
	}
}

//
// Called when the size of this view has changed. 
//
void View::onSizeChanged(glm::vec3 size, glm::vec3 oldSize)
{
	Entity::onSizeChanged(size, oldSize);

	if(parentView()!=nullptr)
		parentView()->updateLayout();
}

//
// Called when this view should assign a size and position to all of its children. 
//
bool View::addChild(std::unique_ptr<Entity> e) {
	if (std::find(children().begin(), children().end(), e) == children().end())
	{
		e->setParent(this);
		Engine::instance()->scene()->signal_entity_added.notify(e.get());
		children().push_back(std::move(e));

		updateLayout();

		return true;
	}
	else
		return false;
	
}

bool View::removeChild(std::unique_ptr<Entity> e) {
	std::vector<std::unique_ptr<Entity>>::iterator iter =
		std::find(children().begin(), children().end(), e);
	if (iter != children().end())
	{
		children().erase(iter);
		e->setParent(nullptr);
		return true;
	}
	else
		return false;
}
