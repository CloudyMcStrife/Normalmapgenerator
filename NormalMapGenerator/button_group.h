#pragma once
#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include "state.h"
class button_group : public nana::label
{
public:
	button_group(int);
	~button_group();
private:
	void initializeGroup();
	std::vector<nana::button> m_buttons;
};

