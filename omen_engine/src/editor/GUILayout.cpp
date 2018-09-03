#include "editor/GUILayout.h"

#include "Engine.h"
#include "utils.h"

#include "editor/WindowDivider.h"

#include "ui/ImageView.h"
#include "ui/LinearLayout.h"
#include "ui/TextView.h"

#include "component/SpriteRenderer.h"
#include "component/Sprite.h"

using namespace omen;
using namespace omen::ui;
using namespace omen::editor;

GUILayout::GUILayout(LinearLayout::LayoutDirection dir) :
	LinearLayout(nullptr, "GUI Layout", { 0.0f,0.0f }, { 0.0f, 0.0f }, dir)
{
	layoutParams().layoutSizingHeight = LayoutParams::MatchParent;
	layoutParams().layoutSizingWidth = LayoutParams::MatchParent;

	if (Engine::instance()->window() != nullptr)
	{
		setSize2D(glm::vec2(Engine::instance()->window()->width(), Engine::instance()->window()->height()));
		updateLayout();
	}
	else {
		Engine::instance()->window()->signal_window_created.connect(this, [this](Window* window) {
			setSize2D(glm::vec2(window->width(), window->height()));
			updateLayout();
		});
	}

	Engine::instance()->window()->signal_window_size_changed.connect(this, [this](int width, int height) {
		setSize2D(glm::vec2(width, height));
		updateLayout();
	});
}

void GUILayout::updateLayout()
{
	LinearLayout::updateLayout();

}

void GUILayout::onMeasure(MeasureSpec horintalMeas, MeasureSpec verticalMeas)
{

}

bool GUILayout::addChild(std::unique_ptr<View> e) {
	LinearLayout* rootLayout = this;
	if(!rootLayout->children().empty())
		rootLayout->addChild(std::move(std::make_unique<WindowDivider>(rootLayout, children().back().get(), e.get())));
	return LinearLayout::addChild(std::move(e));
}