#include "GUILayout.h"

#include "GUILayout.h"

#include "../ui/LinearLayout.h"
#include "../ui/TextView.h"
#include "../utils.h"
#include "../ui/ImageView.h"
#include "WindowDivider.h"

using namespace omen::ui;

GUILayout::GUILayout() :
	View(nullptr, "GUI Layout", { 0.0f,0.0f }, { 100.0f,100.0f })
{
	layoutParams().layoutSizingHeight = LayoutParams::WrapContent;
	layoutParams().layoutSizingWidth = LayoutParams::WrapContent;
	std::unique_ptr<LinearLayout> layout = std::make_unique<LinearLayout>(nullptr, "GUI Main Layout", glm::vec2(0, 0), glm::vec2(width(), height()), LinearLayout::LayoutDirection::HORIZONTAL);
	View::addChild(std::move(layout));
}

void GUILayout::updateLayout()
{
	View::updateLayout();

}

void GUILayout::onMeasure(float maxwidth, float maxheight)
{

}

bool GUILayout::addChild(std::unique_ptr<View> e) {
	LinearLayout* rootLayout = static_cast<LinearLayout*>(children().front().get());
	if(!rootLayout->children().empty())
		rootLayout->addChild(std::move(std::make_unique<WindowDivider>(rootLayout, children().front()->children().back().get(), e.get())));
	return children().front()->addChild(std::move(e));
}