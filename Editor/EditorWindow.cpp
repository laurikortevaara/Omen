#include "EditorWindow.h"

#include "../ui/LinearLayout.h"
#include "../ui/TextView.h"
#include "../utils.h"
#include "../ui/ImageView.h"
#include "WindowDivider.h"

EditorWindow::EditorWindow(const std::string& title) :
	View(nullptr, title, { 0.0f,0.0f }, { 100.0f,0.0f })
{
	Entity* left = nullptr;
	Entity* right = nullptr;
	
	std::unique_ptr<omen::ui::LinearLayout> layout = std::make_unique<omen::ui::LinearLayout>(nullptr, "EditorWindow_VLayout", glm::vec2(0,0), glm::vec2(width(),height()));
	omen::ui::LinearLayout* lp = layout.get();
	addChild(std::move(layout));

	std::unique_ptr<omen::ui::TextView> titleView = std::make_unique<omen::ui::TextView>(nullptr, "EditorWindow_TitleTextView", glm::vec2(20, 3), glm::vec2(width(), 20.0f));
	titleView->setText(omen::string_to_wstring(title));
	
	lp->addChild(std::move(titleView));

	std::unique_ptr<omen::ui::ImageView> imgView = std::make_unique<omen::ui::ImageView>(nullptr, "EditorWindow_ImageView", "textures/smoke.jpg", glm::vec2(0, 0));
	lp->addChild(std::move(imgView));
	
	
}

void EditorWindow::updateLayout()
{
	View::updateLayout();
	int i = 1;
}

void EditorWindow::onMeasure(float maxwidth, float maxheight)
{
	int i = 1;
}