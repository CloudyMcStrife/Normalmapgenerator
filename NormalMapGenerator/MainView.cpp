#include <nana/any.hpp>
#include <nana/gui/wvl.hpp>
#include <nana/gui.hpp>
#include <iostream>
#include "MainView.h"
#include "windowHelper.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 700

#define IMG_WIDTH 400
#define IMG_HEIGHT 600

#define IMG_POS_X 200
#define IMG_POS_Y 75

#define HM_POS_X (IMG_POS_X + (IMG_WIDTH*1.1))
#define HM_POS_Y (IMG_POS_Y)

#define BUTTON_GROUP_POSITION_X 0
#define BUTTON_GROUP_POSITION_Y 100

#define BUTTON_WIDTH 100
#define BUTTON_HEIGHT 60

#define BUTTON_GROUP_OFFSET BUTTON_HEIGHT

using namespace main_;

MainView::MainView(){

}

MainView::MainView(MainPresenter* presenter) : m_presenter(presenter)
{
	m_window = window::initializeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
	m_presenter->setView(this);
	initializeView();
	nana::paint::image img;
	//m_presenter->loadImage();
	initializeHeightMap();
	m_window->show();
	
}


MainView::~MainView()
{
	if (m_presenter)
		delete m_presenter;
	delete m_image;
	delete m_heightMap;
	delete m_filebox;
}

void MainView::initializeView(){
	
	window::addWidget(*m_window, m_browseButton, L"Browse", 0, 0, 100, 60);
	window::addWidget(*m_window, m_exitButton, L"Exit", 0, 120, 100, 60);
	
	window::addWidget(*m_window, m_plane,	L"Plane",	BUTTON_GROUP_POSITION_X, BUTTON_GROUP_POSITION_Y,							BUTTON_WIDTH, BUTTON_HEIGHT);
	window::addWidget(*m_window, m_curved,	L"Curved",	BUTTON_GROUP_POSITION_X, (BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET),	BUTTON_WIDTH, BUTTON_HEIGHT);
	window::addWidget(*m_window, m_top,		L"Top",		BUTTON_GROUP_POSITION_X, (BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET*2), BUTTON_WIDTH, BUTTON_HEIGHT);
	window::addWidget(*m_window, m_left,	L"Left",	BUTTON_GROUP_POSITION_X, (BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET*3), BUTTON_WIDTH, BUTTON_HEIGHT);
	window::addWidget(*m_window, m_right,	L"Right",	BUTTON_GROUP_POSITION_X, (BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET*4), BUTTON_WIDTH, BUTTON_HEIGHT);
	window::addWidget(*m_window, m_bottom,	L"Bottom",	BUTTON_GROUP_POSITION_X, (BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET*5), BUTTON_WIDTH, BUTTON_HEIGHT);

	window::addWidget(*m_window, m_imageLabel, L"Sprite", IMG_POS_X, IMG_POS_Y, IMG_WIDTH, IMG_HEIGHT);
	window::addWidget(*m_window, m_HMLabel, L"Height-Map", HM_POS_X, HM_POS_Y, IMG_WIDTH, IMG_HEIGHT);
	nana::screen screen;
	int screenWidth = screen.desktop_size().width;
	int screenHeight = screen.desktop_size().height;
	m_window->move(screenWidth / 2 - WINDOW_WIDTH / 2, screenHeight / 2 - WINDOW_HEIGHT / 2 - 40);

	m_imageLabel.bgcolor(nana::colors::blue);
	m_HMLabel.bgcolor(nana::colors::blue);
	

	m_filebox = new nana::filebox(*m_window, false);
	
	//Presenter functions will be called
	m_exitButton.events().click([]{
		//m_presenter->exit();
	});

	m_browseButton.events().click([&]{
		if ((*m_filebox)()){
			m_presenter->loadImage(m_filebox->file());
			//m_presenter->loadImage(); //
		}
	});
	
	
	m_image = new nana::drawing(m_imageLabel);
	m_heightMap = new nana::drawing(m_HMLabel);
	
}

void MainView::setImageHandle(nana::paint::image& img){

	m_image->clear();

	calculateLabelSize(img);

	m_image->draw([&](nana::paint::graphics& graph)
	{
		img.stretch(img.size(), graph, nana::rectangle(nana::size(imageWidth, imageHeight)));
	});
	
	m_image->draw([&](nana::paint::graphics& graph){
		m_presenter->drawSpline(graph);
	});
	
	m_image->update();
	m_imageLabel.events().mouse_down([&](nana::arg_mouse mouse){
		drag = true;
		m_presenter->activateDragCtrlPoint(mouse.pos.x, mouse.pos.y);
	});

	m_imageLabel.events().mouse_up([&](nana::arg_mouse mouse){
		drag = false;
		m_presenter->ctrlPointDragged = -1;
	});

	m_imageLabel.events().mouse_move([&](nana::arg_mouse mouse){
		if (drag){
			m_presenter->moveCtrlPoint(mouse.pos.x, mouse.pos.y);
			m_image->update();
		}
		
		std::cout << drag << " " << m_presenter->ctrlPointDragged << std::endl;
	});

	m_imageLabel.events().click([&](nana::arg_mouse mouse){
		m_presenter->imgLabelClicked(mouse.pos.x, mouse.pos.y);
		m_image->update();
		//if(m_model.getControlPoints().size() > 4 && !splineGenerated)
			//m_presenter->drawSpline();
		//else if(!splineGenerated)
			//m_presenter->drawControlPoint(click.x, click.y);
	});

	
}

void MainView::initializeHeightMap(){
	m_heightMap->clear();

	m_heightMap->draw([&](nana::paint::graphics& graph){
		graph.rectangle(nana::rectangle(nana::size(imageWidth, imageHeight)), true, nana::colors::black);
	});

	m_heightMap->update();
}

void MainView::setHeightMapHandle(){

}

void MainView::calculateLabelSize(nana::paint::image& img){
	double sizeFactor = 0.8;

	if (img.size().width > img.size().height){
		imageWidth = (int)IMG_HEIGHT*sizeFactor;
		imageHeight = (int)IMG_WIDTH*sizeFactor;
	}
	else if (img.size().width == img.size().height){
		imageWidth = (int)IMG_HEIGHT*sizeFactor;
		imageHeight = (int)IMG_HEIGHT*sizeFactor;
	}
	else{
		imageWidth = (int)IMG_WIDTH*sizeFactor;
		imageHeight = (int)IMG_HEIGHT*sizeFactor;
	}

	m_imageLabel.size(nana::size(imageWidth, imageHeight));
	m_HMLabel.size(nana::size(imageWidth, imageHeight));
}
