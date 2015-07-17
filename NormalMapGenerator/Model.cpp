#include "Model.h"

Model::Model()
{
	hm_state = states::PLANE;
	m_controlPoints = std::vector<control_point>();
}


Model::~Model()
{
}

void Model::setImagePixelBuffer(nana::paint::drawable_type t, nana::size size){
	if (m_imagePixels == nullptr)
		m_imagePixels = new nana::paint::pixel_buffer(t, size);
	else
		m_imagePixels->attach(t, size);
	clearHM_Buffer();
}

void Model::clearHM_Buffer()
{
	int width = m_imagePixels->size().width;
	int height = m_imagePixels->size().height;
	m_heightMapPixels = new nana::paint::pixel_buffer(width, height);
	m_heightMapPixels->rectangle(nana::size(width, height), nana::colors::black, 0, false);
}

void Model::setHM_PixelAt(int x, int y, nana::color color){
	m_heightMapPixels->pixel(x, y, color.px_color());
}

void Model::addControlPoint(int x, int y){
	m_controlPoints.push_back(control_point(x, y, nana::colors::red));
}

int Model::getCtrlPoint_X(int index){
	return m_controlPoints[index].getX();
}

int Model::getCtrlPoint_Y(int index){
	return m_controlPoints[index].getY();
}

void Model::setCtrlPoint_X(int index, int x){
	m_controlPoints[index].setX(x);
}

void Model::setCtrlPoint_Y(int index, int y){
	m_controlPoints[index].setY(y);
}

int Model::getCtrlSize(){
	return m_controlPoints.size();
}

void Model::clearCtrlPoints(){
	m_controlPoints.clear();
}

nana::paint::pixel_buffer Model::getHM_PixelBuffer(){
	return *m_heightMapPixels;
}

