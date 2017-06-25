#pragma once

#include "../ui/View.h"
#include "GUILayout.h"

class WindowDivider : public omen::ui::View
{
public:
	WindowDivider(GUILayout* parent);
protected:
	virtual void updateLayout() {};
	virtual void onMeasure(float maxwidth, float maxheight) {};
private:
};
