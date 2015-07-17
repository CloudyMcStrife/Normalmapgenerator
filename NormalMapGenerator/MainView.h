#pragma once

#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
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

	void setImageHandle(nana::paint::image&);

	void setHeightMapHandle();

	void initializeHeightMap();

private:
	void initializeView();
	void calculateLabelSize(nana::paint::image& img);
	nana::form* m_window;

	MainPresenter* m_presenter;

	//GUI Objects
	nana::filebox* m_filebox;

	nana::button m_browseButton	;
	nana::button m_exitButton	;
	nana::button m_plane		;
	nana::button m_curved		;
	nana::button m_top			;
	nana::button m_left			;
	nana::button m_right		;
	nana::button m_bottom		;

	nana::label m_imageLabel;
	nana::label m_HMLabel;

	nana::drawing* m_image;
	nana::drawing* m_heightMap;

	int imageWidth, imageHeight;
	nana::size imageSize;

	bool drag = false;

};

