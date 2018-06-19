//
// Created by Lauri Kortevaara on 06/02/16.
//

#include "ui/TextView.h"

#include "component/TextRenderer.h"
#include "component/Draggable.h"
#include "utils.h"

using namespace omen;
using namespace ui;

TextView::TextView(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size) : 
	View(parentView, name, pos, size)
{ 
	std::unique_ptr<ecs::TextRenderer> textRenderer = std::make_unique<ecs::TextRenderer>();
	addComponent(std::move(textRenderer));

	setText(string_to_wstring(name));
	/*Entity* slider = Engine::instance()->scene()->findEntity("Slider1");
	if (slider != nullptr)
	{
		slider->getComponent<ecs::Draggable>();
	}*/
}

void TextView::setText(const std::wstring& text) { 
	m_text = text; 
	ecs::TextRenderer* tr = getComponent<ecs::TextRenderer>();
	tr->setText(text);
}

void TextView::updateLayout()
{
	getComponent<ecs::TextRenderer>()->invalidate();
}

void TextView::onParentChanged()
{
	getComponent<ecs::TextRenderer>()->invalidate();
}

void TextView::setHeight(float height)
{
	Entity::setHeight(height);
	getComponent<ecs::TextRenderer>()->invalidate();
}

void TextView::setWidth(float width)
{
	Entity::setWidth(width);
	getComponent<ecs::TextRenderer>()->invalidate();
}