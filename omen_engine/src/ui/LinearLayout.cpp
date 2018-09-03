//
// Created by Lauri Kortevaara on 06/02/16.
//

#include "ui/LinearLayout.h"

#include "component/Component.h"
#include "editor/WindowDivider.h"

using namespace omen;
using namespace ui;

LinearLayout::LinearLayout(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size, LinearLayout::LayoutDirection dir ) :
    Layout(parentView, name, pos, size),
    m_layoutDirection(dir)
{

}

void LinearLayout::updateLayout() {
	View::updateLayout();
	
	float x = m_paddings[0];
	float y = m_paddings[1];
    
    for(const auto& child : children()) {
		View* childView = dynamic_cast<View*>(child.get());
		childView->setLocalPos2D(glm::vec2(x, y));
        switch(m_layoutDirection){
            case VERTICAL:
				y += childView->height() + childView->margins()[1]+childView->margins()[3];
                break;
            case HORIZONTAL:
				x += childView->width() + childView->margins()[0] + childView->margins()[2];
                break;
        }
    }

	for (const auto& child : children()) {
		View* childView = dynamic_cast<View*>(child.get());
		if (childView->gravity() == VERTICAL_CENTER)
			childView->setLocalPos2D(glm::vec2(childView->localPos2D().x, (height()-childView->height()) / 2.0f));
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

	glm::vec2 childPos(m_paddings.x, m_paddings.y);
	for (const auto& child : children())
	{
		const View* v = dynamic_cast<const View*>(child.get());
		switch (m_layoutDirection)
		{
		case VERTICAL:
			childPos.y += child->height() + v->margins()[1] + v->margins()[3];
			break;
		case HORIZONTAL:
			childPos.x += child->width() + v->margins()[0] + v->margins()[2];
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
	e->setLocalPos2D(e->localPos2D()+childPos);
	View::addChild(std::move(e));
	return true;
}
