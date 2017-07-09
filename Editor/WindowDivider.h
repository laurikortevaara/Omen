#pragma once

#include "../ui/View.h"
#include "GUILayout.h"

class WindowDivider : public omen::ui::View
{
public:
	WindowDivider(View* parent, omen::ecs::Entity* left, omen::ecs::Entity* right);
protected:
	virtual void updateLayout() {};
	virtual void onMeasure(float maxwidth, float maxheight) {};
private:
	omen::ecs::Entity* m_leftView;
	omen::ecs::Entity* m_rightView;	
};
