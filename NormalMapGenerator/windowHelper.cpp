#include "windowHelper.h"

using namespace nana;
namespace main_{
	namespace window{
	//Window Functions
		form* initializeWindow(unsigned int x, unsigned int y){
			return new form(API::make_center(x, y), appear::decorate<appear::minimize, appear::maximize>());
		}
	}
	//App Functions

	namespace app{
		/*void execute(form& f){
			f.show();
			exec();
		}*/
	}
}

