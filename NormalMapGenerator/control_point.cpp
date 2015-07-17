#include "control_point.h"

control_point::control_point(int x, int y, nana::color color) : m_x(x), m_y(y), m_color(color)
{
	
}

control_point::control_point() : m_x(0), m_y(0), m_color(nana::colors::red)
{

}


control_point::~control_point()
{
}
