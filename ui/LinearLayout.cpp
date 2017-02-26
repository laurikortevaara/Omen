//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#include "../component/Component.h"
#include "LinearLayout.h"

using namespace omen;
using namespace ui;

LinearLayout::LinearLayout(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size, LinearLayout::LayoutDirection dir ) :
    Layout(parentView, name, pos, size),
    m_layoutDirection(dir)
{

}

void LinearLayout::updateLayout() {
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
		childPos.y = (h-eh)*0.5;
	}
	e->setLocalPos2D(e->localPos2D()+childPos+glm::vec2(m_margins));
	Entity::addChild(std::move(e));
	return true;
}
