#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/place.hpp>
#include<nana/gui/widgets/label.hpp>

namespace main_{

	namespace window{
		//void initializeWindow(nana::form& f);
		
		template<typename DrawerTrigger>
		void addWidget(nana::form& f, nana::widget_object<nana::category::widget_tag , DrawerTrigger>& w, nana::string caption, int x, int y, int width, int height){
			w.create(f, nana::rectangle(x, y, width, height), true);
			w.caption(caption);
		};

		nana::form* initializeWindow(unsigned int, unsigned int);
	}

	namespace app{

		//void execute(form&);

	}

}

