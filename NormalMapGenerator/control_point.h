#pragma once
#include <nana/gui/wvl.hpp>
class control_point
{
public:
	control_point();
	control_point(int, int, nana::color);
	~control_point();

	int getX(){ return m_x; }
	int getY(){ return m_y; }
	nana::color getColor(){ return m_color; }

	void setX(int x){ m_x = x; }
	void setY(int y){ m_y = y; }
	void setColor(nana::color color){ m_color = color; }

private:
	int m_x, m_y;
	nana::color m_color;


};

