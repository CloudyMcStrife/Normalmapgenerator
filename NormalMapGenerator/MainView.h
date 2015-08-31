#pragma once

#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/checkbox.hpp>

#include "common.h"
#include "MainPresenter.h"
class MainPresenter;

class MainView
{
public:
	MainView();
	MainView(MainPresenter* presenter);
	~MainView();

	void setImage();
	void setHeightMap();

	/*nana::drawing* getImage(){
		return image;
	}
	nana::drawing* getHeightMap(){
		return heightMap;
	}*/

	bool invert(){ return m_invCheck.checked(); }

	void setImageHandle(nana::paint::image&);

	void setHeightMapHandle();

	void initializeHeightMap();

	int getSliderValue();
	float getBrightnessValue();

	int getImageWidth(){ return imageWidth; }
	int getImageHeight(){ return imageHeight; }
private:
	void initializeView();
	void calculateLabelSize(nana::paint::image& img);
	nana::form* m_window;
	
	MainPresenter* m_presenter;

	//GUI Objects
	nana::filebox* m_filebox;

	nana::button m_browseButton	;
	nana::button m_exitButton	;
	nana::button m_apply		;
	nana::button m_blurUp		;
	nana::button m_blurDown			;
	nana::button m_genNMap			;
	nana::button m_saveNMap		;
	nana::button m_bottom		;

	nana::button m_bezier;
	nana::button m_rectangle;
	nana::button m_spline;

	nana::checkbox m_invCheck;

	nana::slider m_slider;
	nana::slider m_brightnessSlider;

	nana::label m_imageLabel;
	nana::label m_HMLabel;

	nana::drawing* m_image;
	nana::drawing* m_heightMap;

	int imageWidth, imageHeight;
	nana::size imageSize;

	bool drag = false;

	glm::vec2 lastMousePos;

};

