//
// Created by Lauri Kortevaara(Intopalo) on 04/02/16.
//

#include "Slider.h"
#include "ImageView.h"
#include "../component/Draggable.h"
#include "../component/Sprite.h"
#include "../component/SpriteRenderer.h"
#include "../Engine.h"
#include "../../component/Sprite.h"
#include "../../component/BorderSprite.h"
#include "../ui/LinearLayout.h"
#include "../ui/TextView.h"
#include "../utils.h"

using namespace omen;
using namespace ui;

Slider::~Slider() = default;

Slider::Slider(View* parentView, const std::string &name, const std::string &spriteName, const glm::vec2& pos, const glm::vec2& size, const glm::vec2& minMax) :
	View(parentView, name, pos, size),
	m_min_value(minMax.x),
	m_max_value(minMax.y),
	m_current_value(0.0f),
	m_pKnot(nullptr)
{
	// Create main slider layout
	std::unique_ptr<LinearLayout> layout = std::make_unique<LinearLayout>(this, "LayoutSlider", glm::vec2(0, 0), size, LinearLayout::LayoutDirection::HORIZONTAL);
	layout->setMargins(glm::vec4(0, 2, 0, 2));

	// Create name label
	std::unique_ptr<TextView> tvName = std::make_unique<TextView>(nullptr, "LabelName", glm::vec2(0, 0), glm::vec2(100, 20));
	tvName->setText(omen::string_to_wstring(name));
	tvName->setGravity(VERTICAL_CENTER);
	layout->addChild(std::move(tvName));

	// Create slider groove layout
	m_groovePos = glm::vec2(0, 0);
	int margin = 10;
	m_grooveSize = glm::vec2(300, size.y);
	std::unique_ptr<LinearLayout> layoutGroove = std::make_unique<LinearLayout>(nullptr, "LayoutSliderGroove", m_groovePos, m_grooveSize, LinearLayout::LayoutDirection::HORIZONTAL);
	layoutGroove->setMargins(glm::vec4(margin, 0, margin, 0));
	layoutGroove->setGravity(VERTICAL_CENTER);
	
	// Create slider groove and add it to the groove layout
	std::unique_ptr<omen::ecs::Sprite> sliderGroove = nullptr;
	sliderGroove = std::make_unique<omen::ecs::BorderSprite>(spriteName, m_groovePos+glm::vec2(0,22.5), glm::vec2(m_grooveSize.x-20,-1), 10, 10, 3, 3);
	std::unique_ptr<omen::ecs::SpriteRenderer> sr = std::make_unique<omen::ecs::SpriteRenderer>(std::move(sliderGroove));
	layoutGroove->addComponent(std::move(sr));
	
	// Create knot button to allow slider value changes
	glm::vec2 knotPos{0,0};
	std::unique_ptr<omen::ui::ImageView> knot = std::make_unique<omen::ui::ImageView>(nullptr, "Knot", "textures/slider_knot.png", knotPos);
	knot->setGravity(VERTICAL_CENTER);
	
	// Create a draggable component and add it to the knot
	std::unique_ptr<omen::ecs::Draggable> dragKnot = std::make_unique<omen::ecs::Draggable>(m_groovePos, glm::vec2(m_grooveSize.x-20,m_grooveSize.y));
	dragKnot->signal_dragged.connect([this](float value) -> void {
		setCurrentValue(this->m_min_value + value*(this->m_max_value - this->m_min_value));
	});

	knot->addComponent(std::move(dragKnot));
	
	m_pKnot = knot.get();
	// Add the knot to the groove layout
	layoutGroove->addChild(std::move(knot));
	

	std::unique_ptr<omen::ecs::Clickable> click = std::make_unique<omen::ecs::Clickable>();
	click->signal_entity_clicked.connect([this](Entity* e, glm::vec2 pos, int button) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			setCurrentValue(this->m_min_value + 0.5f*(this->m_max_value - this->m_min_value));
		}
	});
	layoutGroove->addComponent(std::move(click));

	// Add the groove layout to the main slider layout
	layout->addChild(std::move(layoutGroove));

	// Create a text view for the slider value and add it to the slider layout
	std::unique_ptr<TextView> textView = std::make_unique<TextView>(nullptr, "SliderValue", glm::vec2(0, 0), glm::vec2(300, 20));
	signal_slider_dragged.connect([&](Slider*, float value) {
		TextView* tv = dynamic_cast<TextView*>(findChild("SliderValue"));
		tv->setText(to_wstring_with_precision(value,4)); 
	});
	textView->setGravity(VERTICAL_CENTER);
	layout->addChild(std::move(textView));

	// Add the slider layout to the slider view
	addChild(std::move(layout));
}

void Slider::updateLayout() {

}

void Slider::onMeasure(float maxwidth, float maxheight) {

}

void Slider::setValue(float value) 
{
	Entity* knot = findChild("Knot");
	float width = size().x;
	float proportional_value = value / (m_max_value - m_min_value);
	m_current_value = proportional_value;
	float px = m_groovePos.x + proportional_value*(m_grooveSize.x-knot->width());
	glm::vec2 p = knot->localPos2D();

	TextView* tv = dynamic_cast<TextView*>(findChild("SliderValue"));
	tv->setText(to_wstring_with_precision(value,4));

	knot->setLocalPos2D(glm::vec2(px, p.y));
}

float Slider::value() const {
	return m_current_value * size().x;
}

const std::wstring& Slider::label()
{
	TextView* tvLabel = dynamic_cast<TextView*>(findChild("LabelName"));
	return tvLabel->text();
}

void Slider::setLabel(const std::wstring& label)
{
	TextView* tvLabel = dynamic_cast<TextView*>(findChild("LabelName"));
	tvLabel->setText(label);
}

void Slider::setCurrentValue(float value) {
	m_current_value = value / (this->m_max_value - this->m_min_value);
	signal_slider_dragged.notify(this, m_current_value*(this->m_max_value - this->m_min_value));
	if (m_pKnot != nullptr)
		m_pKnot->setLocalPos2D(glm::vec2(m_current_value*(m_grooveSize.x - 40), m_pKnot->localPos2D().y));
}