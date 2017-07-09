//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#include "../component/Component.h"
#include "LinearLayout.h"
#include "../Editor/WindowDivider.h"

using namespace omen;
using namespace ui;

LinearLayout::LinearLayout(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size, LinearLayout::LayoutDirection dir ) :
    Layout(parentView, name, pos, size),
    m_layoutDirection(dir)
{

}

void LinearLayout::updateLayout() {
	View::updateLayout();
    float y = 0;
    float x = 0;
    for(const auto& childView : children()) {
        switch(m_layoutDirection){
            case VERTICAL:
				y += childView->height();
                break;
            case HORIZONTAL:
				x += childView->width();
                break;
        }
    }
}

void LinearLayout::setLayoutDirection(LinearLayout::LayoutDirection dir) {
    m_layoutDirection = dir;
}

LinearLayout::LayoutDirection &LinearLayout::layoutDirection() {
    return m_layoutDirection;
}

bool LinearLayout::addChild(std::unique_ptr<Entity> e)
{
	if (!children().empty() && dynamic_cast<WindowDivider*>(e.get()) == nullptr)
	{
		View* v = static_cast<View*>(children().back().get());
		if(v != nullptr && v->resizable())
			addChild(std::move(std::make_unique<WindowDivider>(this, children().back().get(), e.get())));
	}

	glm::vec2 childPos(m_margins.x, m_margins.y);
	for (const auto& child : children())
	{
		switch (m_layoutDirection)
		{
		case VERTICAL:
			childPos.y += child->height();
			break;
		case HORIZONTAL:
			childPos.x += child->width();
			break;
		}
	}
	if (m_layoutDirection == HORIZONTAL && e->gravity() & VERTICAL_CENTER)
	{
		float h = height();
		float eh = e->height();
		if (h == -1)
			h = eh;
		childPos.y = (h-eh)*0.5f;
	}
	e->setLocalPos2D(e->localPos2D()+childPos+glm::vec2(m_margins));
	View::addChild(std::move(e));
	return true;
}
