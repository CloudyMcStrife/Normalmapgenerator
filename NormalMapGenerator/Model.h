#pragma once

#include <Magick++.h>
#include <nana/paint/pixel_buffer.hpp>
#include "state.h"
#include "control_point.h"

class Model
{
public:
	Model();
	~Model();

	void setImagePixelBuffer(nana::paint::drawable_type t, nana::size size);
	void clearHM_Buffer();

	void setHM_PixelAt(int, int, nana::color);
	nana::paint::pixel_buffer getHM_PixelBuffer();

	states::HM_State getHM_State(){ return hm_state; }
	void setHM_State(states::HM_State state){ hm_state = state; }

	void addControlPoint(int, int);

	int getCtrlPoint_X(int);
	int getCtrlPoint_Y(int);

	void setCtrlPoint_X(int, int);
	void setCtrlPoint_Y(int, int);

	int getCtrlSize();
	//std::vector<Pixel> getImagePixelData();
	//std::vector<Pixel> getMaskedPixelData();
	//void calculateMaskedPixels(Spline);

private:

	nana::paint::pixel_buffer* m_imagePixels;
	nana::paint::pixel_buffer* m_heightMapPixels;
	std::vector<nana::pixel_color_t> t;
	
	
	states::HM_State hm_state;

	std::vector<control_point> controlPoints;
};

