#include "GUILayout.h"

#include "GUILayout.h"

#include "../ui/LinearLayout.h"
#include "../ui/TextView.h"
#include "../utils.h"
#include "../ui/ImageView.h"
#include "WindowDivider.h"

GUILayout::GUILayout() :
	View(nullptr, "GUI Layout", { 0.0f,0.0f }, { 100.0f,100.0f })
{
	layoutParams().layoutSizingHeight = omen::ui::LayoutParams::WrapContent;
	layoutParams().layoutSizingWidth = omen::ui::LayoutParams::WrapContent;
	std::unique_ptr<omen::ui::LinearLayout> layout = std::make_unique<omen::ui::LinearLayout>(nullptr, "GUI Main Layout", glm::vec2(0, 0), glm::vec2(width(), height()), omen::ui::LinearLayout::LayoutDirection::HORIZONTAL);
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
	if(!children().front()->children().empty())
		children().front()->addChild(std::move(std::make_unique<WindowDivider>(this, children().front()->children().front().get(), e.get())));
	return children().front()->addChild(std::move(e));
}