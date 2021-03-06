#include "editor/EditorWindow.h"

#include "utils.h"

#include "ui/LinearLayout.h"
#include "ui/TextView.h"

using namespace omen::ui;
using namespace omen::editor;

EditorWindow::EditorWindow(const std::string& title) :
	View(nullptr, title, { 0.0f,0.0f }, { 100.0f,100.0f })
{
	layoutParams().layoutSizingHeight = LayoutParams::Absolute;
	layoutParams().layoutSizingWidth = LayoutParams::Absolute;
	Entity* left = nullptr;
	Entity* right = nullptr;
	
	std::unique_ptr<omen::ui::LinearLayout> layout = std::make_unique<omen::ui::LinearLayout>(nullptr, "EditorWindow_VLayout", glm::vec2(0,0), glm::vec2(width(),height()));
	omen::ui::LinearLayout* lp = layout.get();
	addChild(std::move(layout));

	std::unique_ptr<omen::ui::TextView> titleView = std::make_unique<omen::ui::TextView>(nullptr, "EditorWindow_TitleTextView", glm::vec2(20, 3), glm::vec2(width(), 20.0f));
	titleView->setText(omen::string_to_wstring(title));
	
	lp->addChild(std::move(titleView));
}

void EditorWindow::updateLayout()
{
	View::updateLayout();
	int i = 1;
}

void EditorWindow::onMeasure(MeasureSpec horintalMeas, MeasureSpec verticalMeas)
{
	int i = 1;
}