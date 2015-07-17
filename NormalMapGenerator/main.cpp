/*#define MAGICKCORE_QUANTUM_DEPTH 8
//#define MAGICKCORE_HDRI_ENABLE 8

#include <Magick++.h> */
#include <iostream> 
#include "windowHelper.h"


#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/button.hpp>

using namespace std;
//using namespace Magick;
using namespace nana;

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
#include "common.h"
#include "Model.h"
#include "MainView.h"
#include "MainPresenter.h"

Model* model;
MainPresenter* presenter;
MainView* view;

int main(int argc, char **argv)
{

	
	//Magick::InitializeMagick(*argv);
	/*
	auto window = main_::window::initializeWindow(800, 600);
	//form window(API::make_center(800, 600));
	
	button b;
	b.caption(L"Button 1");
	
	button b2;
	b2.caption(L"Button 2");

	main_::window::addWidget(*window, b, 0, 0, 30, 30);
	main_::window::addWidget(*window, b2, 10, 10, 15, 15);

	/*
	place plc(*window);	//fm is an instance of ::nana::form
	plc.div("<efg>");
	plc.field("efg") << b << b2;	//These buttons are children of fm.
	plc.collocate();
	
	
	window->show();
	exec();

	delete window;
	*/
	model = new Model();
	presenter = new MainPresenter(model);
	view = new MainView(presenter);

	int a = 0;

	exec();

	return 0; 
}

/*
form fm(API::make_center(1000, 800)) ;
label lab;
//lab.move(nana::rectangle(0, 0, 0, 0));
lab.create(fm, nana::rectangle(0, 0, 800, 600), true);

lab.format(true);
lab.add_format_listener(listener);
lab.caption(STR("<image=\"C:/Users/Gimmillaro Fabio/Desktop/Projekte/NormalMapGenerator/x64/Debug/chill.jpg\" size=(800,600)>"));

fm.show();
exec();
*/