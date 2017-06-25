#pragma once

#include "../ui/View.h"
#include "EditorWindow.h"

class GUILayout : public omen::ui::View
{
public:
	GUILayout();
	void addWindow(std::unique_ptr<EditorWindow> window);
	
	virtual bool addChild(std::unique_ptr<View> e);

protected:	
	virtual void updateLayout();
	virtual void onMeasure(float maxwidth, float maxheight);
private:
	std::unique_ptr<omen::ui::View> m_layout;
};
