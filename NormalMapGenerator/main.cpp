#include <iostream> 
#include "windowHelper.h"


#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>

using namespace std;
using namespace nana;

#include "common.h"
#include "Model.h"
#include "MainView.h"
#include "MainPresenter.h"

Model* model;
MainPresenter* presenter;
MainView* view;

int main(int argc, char **argv)
{
	
	model = new Model();
	presenter = new MainPresenter(model);
	view = new MainView(presenter);

	exec();

	return 0; 
}


/*
void listener(label::command cmd, const nana::string& s)
{
if (label::command::click == cmd)
{
msgbox mb(STR("target clicked"));
mb << STR("the target \"") << s << "\" is clicked";
mb();
}
}*/