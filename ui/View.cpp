//
// Created by Lauri Kortevaara on 06/02/16.
//

#include "View.h"
#include "../Engine.h"
#include "../component/Sprite.h"
#include "../component/SpriteRenderer.h"
#include "../component/PlainColorRenderer.h"
#include "LinearLayout.h"
#include "../component/TextRenderer.h"

using namespace omen;
using namespace ui;

View::View(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size) :
	ecs::Entity(name), m_resizeMode(DEFAULT_RESIZE_MODE), m_viewState(ViewState_Normal), m_measuredSize({ -1.f,-1.f })
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

	// Initially no weight preference
	layoutParams().weight = 0.0f;

	Engine::instance()->window()->signal_window_size_changed.connect(this,[this](int width, int height) {
		this->updateLayout();
	});
}


View::~View() {

}

/*
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
}*/

//
// Called to determine the size requirements for this view and all of its children.
//
void View::onMeasure(MeasureSpec horintalMeas, MeasureSpec verticalMeas) 
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
	if (m_viewState != ViewState_UpdatingLayout)
	{
		m_viewState = ViewState_UpdatingLayout;
		
		// Update size if sizing mode is match parent
		if (parentView() != nullptr && layoutParams().weight == 0.0f)
		{
			if(layoutParams().layoutSizingWidth == LayoutParams::MatchParent)
				setWidth(parent()->width()-parentView()->paddings()[0]-parentView()->paddings()[2]);
			if (layoutParams().layoutSizingHeight == LayoutParams::MatchParent)
				setHeight(parent()->height() - parentView()->paddings()[1] - parentView()->paddings()[3]);
		}

		// .. and if no parent exists and set to match parent, use the window size
		if (parent() == nullptr && Engine::instance()->window() != nullptr)
		{
			if (layoutParams().layoutSizingWidth == LayoutParams::MatchParent)
				setWidth(static_cast<float>(Engine::instance()->window()->width()));
			if (layoutParams().layoutSizingHeight == LayoutParams::MatchParent)
				setHeight(static_cast<float>(Engine::instance()->window()->height()));
		}
		
		// Update size if needed for content wrapping mode
		float children_width = m_paddings[0]+m_paddings[2];
		float children_height = m_paddings[1] + m_paddings[3];
		if (m_layoutParams.layoutSizingWidth == LayoutParams::WrapContent || m_layoutParams.layoutSizingHeight == LayoutParams::WrapContent)
		{
			if (!children().empty())
			{
				for (auto& c : children())
				{
					children_width = fmax(children_width, c->pos().x + c->width());
					children_height = fmax(children_height, c->pos().y + c->height());
				}
				if (m_layoutParams.layoutSizingWidth == LayoutParams::WrapContent)
					setWidth(children_width);
				if (m_layoutParams.layoutSizingHeight == LayoutParams::WrapContent)
					setHeight(children_height);
			}
			else
			if (getComponent<ecs::TextRenderer>() != nullptr)
			{
				setHeight(120.0f);
			}
		}

		for (auto& c : children())
		{
			View* v = dynamic_cast<View*>(c.get());
			if (v != nullptr)
			{
				if (v->layoutParams().layoutSizingWidth == LayoutParams::MatchParent)
				{
					c->setSize2D(glm::vec2(size2D().x - paddings().x - paddings().z, c->height()));
				}
				if(v->layoutParams().layoutSizingHeight == LayoutParams::MatchParent)
					c->setSize2D(glm::vec2(c->width(), size2D().y - paddings().y - paddings().w));

				if (v->layoutParams().layoutSizingWidth == LayoutParams::WrapContent)
					c->setSize2D(glm::vec2(v->measuredWidth(), v->height()));
				if (v->layoutParams().layoutSizingHeight == LayoutParams::WrapContent)
					c->setSize2D(glm::vec2(v->width(), v->measuredHeight()));

				if (v->layoutParams().layoutSizingWidth == LayoutParams::WrapContent || v->layoutParams().layoutSizingHeight == LayoutParams::WrapContent)
					c->setHeight(120.0f);
			}
		}

		m_viewState = ViewState_Normal;
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


void View::setBackground(Color& color)
{
	addComponent(std::make_unique<ecs::PlainColorRenderer>(color));
}

void View::setBackground(std::unique_ptr<ecs::Sprite> sprite)
{
	addComponent(std::make_unique<omen::ecs::SpriteRenderer>(std::move(sprite)));
}

float View::width() const 
{ 
	LinearLayout* l = dynamic_cast<LinearLayout*>(parentView());
	if (l && l->layoutDirection() == LinearLayout::HORIZONTAL)
	{
		float width = 0.0f;
		
		if (m_layoutParams.weight > 0)
			width = (l->width() - l->paddings()[0] - l->paddings()[2]) * m_layoutParams.weight;
		else
			width = Entity::width();
		
		return width - m_margins[0] - m_margins[2];
	}
	else
	{
		float width = Entity::width();
		width -= m_margins[0] + m_margins[2];
		return width;
	}
}

float View::height() const 
{ 
	LinearLayout* l = dynamic_cast<LinearLayout*>(parentView());
	if (l && l->layoutDirection() == LinearLayout::VERTICAL)
	{
		return m_layoutParams.weight > 0 ? (l->height() - l->paddings()[1] - l->paddings()[3]) * m_layoutParams.weight : Entity::height() - m_margins[1] - m_margins[3];
	}
	else
	{
		float height = Entity::height();
		height -= m_margins[1] + m_margins[3];
		return height;
	}
}

void View::measure(MeasureSpec horizontalMeas, MeasureSpec verticalMeas)
{
	this->onMeasure(horizontalMeas, verticalMeas);
}