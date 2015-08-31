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

#define BUTTON_WIDTH 150
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
	m_window->show();
	
}



MainView::~MainView()
{
	if (m_presenter)
		delete m_presenter;
	delete m_image;
	delete m_filebox;
	delete m_heightMap;
	delete m_filebox;
	delete m_window;
}

bool save;

void MainView::initializeView(){
	save = false;
	m_window->caption("Normalmap-Generator");
	window::addWidget(*m_window, m_browseButton, L"Browse", 0, 0, 100, 60);
	window::addWidget(*m_window, m_exitButton, L"Exit", 0, 120, 100, 60);
	
	window::addWidget(*m_window, m_apply,	L"Apply Height",	BUTTON_GROUP_POSITION_X, BUTTON_GROUP_POSITION_Y,							BUTTON_WIDTH, BUTTON_HEIGHT);
	window::addWidget(*m_window, m_blurUp,	L"Blur up",	BUTTON_GROUP_POSITION_X, (BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET),	BUTTON_WIDTH, BUTTON_HEIGHT);
	window::addWidget(*m_window, m_blurDown,L"Blur down",		BUTTON_GROUP_POSITION_X, (BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET*2), BUTTON_WIDTH, BUTTON_HEIGHT);
	window::addWidget(*m_window, m_genNMap,	L"Generate Normalmap",	BUTTON_GROUP_POSITION_X, (BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET*3), BUTTON_WIDTH, BUTTON_HEIGHT);
	window::addWidget(*m_window, m_saveNMap,L"Save Normalmap",	BUTTON_GROUP_POSITION_X, (BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET*4), BUTTON_WIDTH, BUTTON_HEIGHT);
	//window::addWidget(*m_window, m_bottom,	L"Bottom",	BUTTON_GROUP_POSITION_X, (BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET*5), BUTTON_WIDTH, BUTTON_HEIGHT);

	window::addWidget(*m_window, m_bezier, L"Bezier", BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET, BUTTON_GROUP_POSITION_X, BUTTON_WIDTH, BUTTON_HEIGHT);
	window::addWidget(*m_window, m_spline, L"Circle", BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET*3, BUTTON_GROUP_POSITION_X, BUTTON_WIDTH, BUTTON_HEIGHT);
	window::addWidget(*m_window, m_rectangle, L"Rectangle", BUTTON_GROUP_POSITION_Y + BUTTON_GROUP_OFFSET*5, BUTTON_GROUP_POSITION_X, BUTTON_WIDTH, BUTTON_HEIGHT);
	
	window::addWidget(*m_window, m_invCheck, L"Invert", 400, IMG_POS_Y + IMG_HEIGHT - 30 - 50, 100, 20);
	m_slider.create(*m_window, true);
	m_slider.move(nana::rectangle(IMG_POS_X + IMG_WIDTH / 2, IMG_POS_Y + IMG_HEIGHT - 50, IMG_WIDTH, 10));
	m_slider.vmax(20);

	m_genNMap.events().click([&]{
		m_presenter->normal = !m_presenter->normal;
		m_heightMap->update();
	});

	m_saveNMap.events().click([&]{
		save = true;
		m_heightMap->update();
	});

	m_invCheck.events().click([&]{
		m_heightMap->update();
	});

	m_blurDown.events().click([&]{
		m_presenter->blur = (m_presenter->blur < 2) ? 1 : m_presenter->blur-1;
		m_heightMap->update();
	});

	m_blurUp.events().click([&]{
		m_presenter->blur++;
		m_heightMap->update();
	});

	m_apply.events().click([&]{
		m_presenter->apply = true;
		m_heightMap->update();
	});

	m_slider.events().value_changed([&]{
		m_presenter->m_normalsInitialized = true;
		m_image->update();
	});

	m_brightnessSlider.create(*m_window, true);
	m_brightnessSlider.move(nana::rectangle(IMG_POS_X + IMG_WIDTH / 2, IMG_POS_Y + IMG_HEIGHT + 20 - 50, IMG_WIDTH, 10));
	m_brightnessSlider.vmax(100);
	
	m_brightnessSlider.events().value_changed([&]{
		m_image->update();
	});
	//m_slider.events().
	window::addWidget(*m_window, m_imageLabel, L"Sprite", IMG_POS_X, IMG_POS_Y, IMG_WIDTH, IMG_HEIGHT);
	window::addWidget(*m_window, m_HMLabel, L"Height-Map", HM_POS_X, HM_POS_Y, IMG_WIDTH, IMG_HEIGHT);
	nana::screen screen;
	int screenWidth = screen.desktop_size().width;
	int screenHeight = screen.desktop_size().height;
	m_window->move(screenWidth / 2 - WINDOW_WIDTH / 2, screenHeight / 2 - WINDOW_HEIGHT / 2 - 40);

	m_imageLabel.bgcolor(nana::colors::white);
	m_HMLabel.bgcolor(nana::colors::white);
	

	m_filebox = new nana::filebox(*m_window, false);
	
	//Presenter functions will be called
	m_exitButton.events().click([]{
		//m_presenter->exit();
	});

	m_browseButton.events().click([&]{
		if ((*m_filebox)()){
			m_presenter->loadImage(m_filebox->file());
			setHeightMapHandle();
		}
	});

	m_bezier.events().click([&]{
		m_presenter->changeD_State(states::D_State::BEZIER);
		m_image->update();
	});

	m_rectangle.events().click([&]{
		m_presenter->changeD_State(states::D_State::RECT);
		m_image->update();
	});

	m_spline.events().click([&]{
		m_presenter->changeD_State(states::D_State::CIRCLE);
		m_image->update();
	});
	
	
	m_image = new nana::drawing(m_imageLabel);
	m_heightMap = new nana::drawing(m_HMLabel);

	m_imageLabel.events().mouse_down([&](nana::arg_mouse mouse){
		drag = true;
		m_presenter->activateDragCtrlPoint(mouse.pos.x, mouse.pos.y);
		std::cout << "UPDATE BITCH" << std::endl;
		m_image->update();
	});

	m_imageLabel.events().mouse_up([&](nana::arg_mouse mouse){
		drag = false;
	});

	m_imageLabel.events().mouse_move([&](nana::arg_mouse mouse){
		if (drag){
			m_presenter->moveCtrlPoint(mouse.pos.x, mouse.pos.y);
			m_image->update();
			std::cout << "Drag" << std::endl;
		}
		lastMousePos = glm::vec2(mouse.pos.x, mouse.pos.y);
	});

	m_imageLabel.events().click([&](nana::arg_click mouse){
		m_presenter->imgLabelClicked(lastMousePos.x, lastMousePos.y);
		m_image->update();
	});
	
}

void MainView::setImageHandle(nana::paint::image& img){
	m_image->clear();

	calculateLabelSize(img);
	
	m_image->draw([&](nana::paint::graphics& graph){
		std::cout << "Update" << std::endl;
		nana::rectangle rect(img.size());
		//img.stretch(rect, graph, nana::rectangle(nana::size(imageWidth, imageHeight)));
		img.paste(rect, graph, nana::point());
		m_presenter->draw(graph);
		m_heightMap->update();
	});
	
	m_image->update();

	initializeHeightMap();
}

void MainView::initializeHeightMap(){
	m_heightMap->clear();
	
	m_heightMap->draw([&](nana::paint::graphics& graph){
		m_presenter->fillHM_Graph(graph, 400, 600);
	});

	m_heightMap->update();
}

int MainView::getSliderValue(){
	return m_slider.value();
}

float MainView::getBrightnessValue(){
	return m_brightnessSlider.value() / 100.0f;
}

void MainView::setHeightMapHandle(){
	m_heightMap->clear();
	
	m_heightMap->draw([&](nana::paint::graphics& graph){
		if (!m_presenter->normal)
			m_presenter->fillHM_Graph(graph, 400, 600);
		else
			m_presenter->showNormalmap(graph);

		if (save){
			m_presenter->saveNormalmap(graph);
			save = false;
		}
	});

	m_heightMap->update();
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
