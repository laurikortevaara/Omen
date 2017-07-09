#include "WindowDivider.h"
#include "../ui/LinearLayout.h"
#include "../component/MouseInput.h"
#include "../component/Draggable.h"
#include "../Engine.h"

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
		setSize2D({ 5.0f, parent->height() });
	else
		setSize2D({ parent->width(), 5.0f });

	tr()->setBounds(glm::vec3(0, 0, 0), glm::vec3(width(), height(), 0));

	// Create a draggable component which allows to resize the views
	std::unique_ptr<omen::ecs::Draggable> dragCtl = std::make_unique<omen::ecs::Draggable>(parent->pos2D(), parent->size2D());
	parent->signal_size_changed.connect(this, [this](omen::ecs::Entity* e, glm::vec3 size, glm::vec3 oldSize) {
		this->getComponent<omen::ecs::Draggable>()->setGrooveSize(size);
	});
	dragCtl->signal_draggedXY.connect(this, [this](glm::vec2 value) -> void {
		//std::cout << "Value: " << value << "\n";
		LinearLayout* layout = dynamic_cast<LinearLayout*>(this->parent());
		omen::ui::LinearLayout::LayoutDirection dir = layout->layoutDirection();
		if (dir == LinearLayout::HORIZONTAL)
		{
			m_leftView->setWidth(value.x*this->parent()->width());
			m_rightView->setWidth((1.0f - value.x)*this->parent()->width());
			m_rightView->setLocalPos2D(glm::vec2(this->localPos2D().x + this->width(), m_rightView->localPos2D().y));
		}
		else
		{
			m_leftView->setHeight(value.y*this->parent()->height());
			m_rightView->setHeight((1.0f - value.y)*this->parent()->height());
			m_rightView->setLocalPos2D(glm::vec2(m_rightView->localPos2D().x, this->localPos2D().y + this->height()));
		}

	});
	//dragCtl->setEnabled(false);
	addComponent(std::move(dragCtl));

	signal_entered.connect(this, [this](omen::ecs::Entity* e, glm::vec2 pos) 
	{
		LinearLayout* layout = dynamic_cast<LinearLayout*>(this->parent());
		omen::ui::LinearLayout::LayoutDirection dir = layout->layoutDirection();
		if(dir == omen::ui::LinearLayout::HORIZONTAL)
			glfwSetCursor(omen::Engine::instance()->window()->window(), glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR));
		else
			glfwSetCursor(omen::Engine::instance()->window()->window(), glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
		//this->getComponent<omen::ecs::Draggable>()->setEnabled(true);
	});

	signal_exited.connect(this, [this](omen::ecs::Entity* e, glm::vec2 pos)
	{
		glfwSetCursor(omen::Engine::instance()->window()->window(), glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
		//this->getComponent<omen::ecs::Draggable>()->setEnabled(false);
	});
}