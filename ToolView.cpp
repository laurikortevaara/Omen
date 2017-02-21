#include "ToolView.h"
#include "component/BorderSprite.h"
#include "component/SpriteRenderer.h"
#include "ui/Button.h"
#include "ui/ImageView.h"
#include "ui/LinearLayout.h"
#include "utils.h"

using namespace omen;
using namespace ui;
using namespace ecs;

ToolView::ToolView(const std::string& name, const std::string& sprite, const glm::vec2& pos, const glm::vec2& size) :
	ui::View(nullptr, name, pos, size)
{
	Entity::setLocalPos2D({ 500, 500 });
	std::unique_ptr<BorderSprite> bs = std::make_unique<BorderSprite>("textures/toolbar.jpg", glm::vec2(0, 0), glm::vec2(150.0f, 150.0f), 2, 2, 2, 2);
	std::unique_ptr<omen::ecs::SpriteRenderer> sr = std::make_unique<omen::ecs::SpriteRenderer>(std::move(bs));
	addComponent(std::move(sr));

	std::unique_ptr<LinearLayout> layout = std::make_unique<LinearLayout>(this, "LayoutTools", glm::vec2(0, 0), glm::vec2(0, 0));
	layout->setMargins(glm::vec4(5, 5, 5, 5));

	std::unique_ptr<omen::ui::Button> button_translate = std::make_unique<omen::ui::Button>(layout.get(), "button_translate", "textures/button_translate.jpg");
	button_translate->signal_exited.connect([this](Entity* b, glm::vec2 v) {
		b->setLocalPos2D({ 100.0,0.0 });
	});
	layout->addChild(std::move(button_translate));

	std::unique_ptr<omen::ui::Button> button_rotate = std::make_unique<omen::ui::Button>(layout.get(), "button_rotate", "textures/button_rotate.jpg");
	button_rotate->signal_hovered.connect([this](Entity* b, glm::vec2 v) {
		//b->setLocalPos2D({ 100.0,0.0 });
		static int s = 0;
		s++;
		if (s > 100)
			b->setLocalPos2D({ 100.0,0.0 });
	});
	layout->addChild(std::move(button_rotate));

	std::unique_ptr<omen::ui::Button> button_scale = std::make_unique<omen::ui::Button>(layout.get(), "button_scale", "textures/button_scale.jpg");
	button_scale->signal_entered.connect([this](Entity* b, glm::vec2 v) {
		b->setLocalPos2D({ 100.0,0.0 });
	});
	layout->addChild(std::move(button_scale));

	addChild(std::move(layout));
}

void ToolView::updateLayout()
{
}