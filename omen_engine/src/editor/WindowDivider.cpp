#include "editor/WindowDivider.h"

#include "Engine.h"
#include "ui/LinearLayout.h"
#include "component/MouseInput.h"
#include "component/Draggable.h"

#include <GLFW/glfw3.h>

using namespace omen::ui;

WindowDivider::WindowDivider(View* parent, omen::ecs::Entity* leftView, omen::ecs::Entity* rightView) :
	View(parent, "WinDiv", { 0.0f,0.0f }, { 0.0f,0.0f }),
	m_leftView(leftView),
	m_rightView(rightView)
{
	LinearLayout* layout = dynamic_cast<LinearLayout*>(this->parent());
	omen::ui::LinearLayout::LayoutDirection dir = layout->layoutDirection();
	std::string name = layout->name();

	glm::vec2 s = parent->size2D();

	if (layout->layoutDirection() == omen::ui::LinearLayout::LayoutDirection::HORIZONTAL)
	{
		layoutParams().layoutSizingWidth = LayoutParams::Absolute;
		layoutParams().layoutSizingHeight = LayoutParams::MatchParent;
		setSize2D({ 2.0f, parent->height() });
	}
	else
	{
		layoutParams().layoutSizingWidth = LayoutParams::MatchParent;
		layoutParams().layoutSizingHeight = LayoutParams::Absolute;
		setSize2D({ parent->width(), 2.0f });
	}

	tr()->setBounds(glm::vec3(0, 0, 0), glm::vec3(width(), height(), 0));

	// Create a draggable component which allows to resize the views
	std::unique_ptr<omen::ecs::Draggable> dragCtl = std::make_unique<omen::ecs::Draggable>(parent->pos2D(), parent->size2D());
	parent->signal_size_changed.connect(this, [this](omen::ecs::Entity* e, glm::vec3 size, glm::vec3 oldSize) {
		this->getComponent<omen::ecs::Draggable>()->setGrooveSize(size);
	});
	dragCtl->signal_draggedXY.connect(this, [this](glm::vec2 value) -> void {
		std::cout << "Value: " << value.x << "\n";
		LinearLayout* layout = dynamic_cast<LinearLayout*>(this->parent());
		omen::ui::LinearLayout::LayoutDirection dir = layout->layoutDirection();
		if (dir == LinearLayout::HORIZONTAL)
		{
			float rvalue = (layout->width() * value.x - m_leftView->pos2D().x)/(m_leftView->width()+m_rightView->width()+width());
			float availableWidth = m_leftView->width() + m_rightView->width();
			m_leftView->setWidth(rvalue*availableWidth);
			m_rightView->setWidth((1.0f - rvalue)*availableWidth);
			dynamic_cast<Layout*>(parentView())->updateLayout();
		}
		else
		{
			float rvalue = ((layout->height()) * value.y - m_leftView->pos2D().y) / (m_leftView->height() + m_rightView->height() + height());
			float availableheight = m_leftView->height() + m_rightView->height();
			m_leftView->setHeight(rvalue*availableheight);
			m_rightView->setHeight((1.0f - rvalue)*availableheight);
			dynamic_cast<Layout*>(parentView())->updateLayout();
		}

	});
	//dragCtl->setEnabled(false);
	addComponent(std::move(dragCtl));

	signal_entered.connect(this, [this](omen::ecs::Entity* e, glm::vec2 pos) 
	{
		LinearLayout* layout = dynamic_cast<LinearLayout*>(this->parent());
		omen::ui::LinearLayout::LayoutDirection dir = layout->layoutDirection();
		if(dir == omen::ui::LinearLayout::HORIZONTAL)
			glfwSetCursor(omen::Engine::instance()->window()->window(), glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
		else
			glfwSetCursor(omen::Engine::instance()->window()->window(), glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR));
		//this->getComponent<omen::ecs::Draggable>()->setEnabled(true);
	});

	signal_exited.connect(this, [this](omen::ecs::Entity* e, glm::vec2 pos)
	{
		glfwSetCursor(omen::Engine::instance()->window()->window(), glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
		//this->getComponent<omen::ecs::Draggable>()->setEnabled(false);
	});
}