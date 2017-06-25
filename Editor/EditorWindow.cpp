#include "EditorWindow.h"

#include "../ui/LinearLayout.h"
#include "../ui/TextView.h"
#include "../utils.h"
#include "../ui/ImageView.h"

EditorWindow::EditorWindow(const std::string& title) :
	View(nullptr, title, { 0.0f,0.0f }, { 100.0f,0.0f })
{
	//std::unique_ptr<omen::ui::ImageView> backgroundImage = std::make_unique<omen::ui::ImageView>(nullptr, "ImageView", "textures/smoke.jpg", glm::vec2(0, 0));
	//addComponent
	std::unique_ptr<omen::ui::LinearLayout> layout = std::make_unique<omen::ui::LinearLayout>(nullptr, "foo", glm::vec2(0,0), glm::vec2(width(),height()));
	std::unique_ptr<omen::ui::TextView> titleView = std::make_unique<omen::ui::TextView>(nullptr, "bar", glm::vec2(20, 3), glm::vec2(width(), 20.0f));
	titleView->setText(omen::string_to_wstring(title));
	layout->addChild(std::move(titleView));

	std::unique_ptr<omen::ui::ImageView> imgView = std::make_unique<omen::ui::ImageView>(nullptr, "ImageView", "textures/smoke.jpg", glm::vec2(0, 0));
	//setSize2D({ imgView->size().x, size().y });
	layout->addChild(std::move(imgView));
	
	addChild(std::move(layout));
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