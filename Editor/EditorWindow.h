#pragma once
#include <string>
#include "../ui/View.h"

class EditorWindow : public omen::ui::View
{
public:
	EditorWindow(const std::string& title);


protected:
	virtual void updateLayout();
	virtual void onMeasure(float maxwidth, float maxheight);
private:
};
