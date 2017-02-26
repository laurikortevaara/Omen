//
// Created by Lauri Kortevaara(Intopalo) on 06/02/16.
//

#include "../component/TextRenderer.h"
#include "../component/Draggable.h"
#include "TextView.h"

using namespace omen;
using namespace ui;

TextView::TextView(View *parentView, const std::string& name, const glm::vec2& pos, const glm::vec2& size) : 
	View(parentView, name, pos, size)
{ 
	std::unique_ptr<ecs::TextRenderer> textRenderer = std::make_unique<ecs::TextRenderer>();
	addComponent(std::move(textRenderer));

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