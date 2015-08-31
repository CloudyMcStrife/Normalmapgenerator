#pragma once

#include <vector>
#include <nana/paint/pixel_buffer.hpp>
#include "state.h"
#include <glm\glm.hpp>
#include <iostream>

class Model
{
public:
	Model();
	~Model();

	void setImagePixelBuffer(nana::paint::drawable_type, int, int);
	void clearHM_Buffer(int, int);

	void setHM_PixelAt(int, int, nana::color);
	nana::paint::pixel_buffer* getHM_PixelBuffer();
	nana::paint::pixel_buffer* getImgBuffer(){return m_imagePixels;}
	nana::paint::pixel_buffer* getNormalBuffer(){ return m_normalMapPixels; }

	std::vector<glm::vec2> getImagePixelBounds(int, int);

	states::HM_State getHM_State(){ return hm_state; }
	void setHM_State(states::HM_State state){ hm_state = state; }

	states::D_State getD_State(){ return d_state; }
	void setD_State(states::D_State state){ d_state = state; }

	void addControlPoint(int, int);
	
	int getCtrlPoint_X(int);
	int getCtrlPoint_Y(int);

	void addBezierVertex(glm::vec2);
	glm::vec2 getBezierVertex(int);
	void setBezierNormal(int, glm::vec2);
	void addBezierData(glm::vec2, glm::vec2);

	int getBezierSize(){return m_bezierNormals.size(); }
	std::vector<glm::vec2> getBezierVertices(){ return m_bezierVertices; }

	glm::vec2 getBezierNormal(int i){ return m_bezierNormals[i]; }
	void addBezierNormal(glm::vec2 normal){ m_bezierNormals.push_back(normal); }
	void clearBezierNormals(){ m_bezierNormals.clear(); }
	void clearBezierVertices(){ m_bezierVertices.clear(); }

	std::vector<glm::vec2> getCtrlPoints(){ return m_controlPoints; }

	void setCtrlPoint(int, glm::vec2);

	void setMiddlePoint(glm::vec2);

	int getCtrlSize();

	void clearCtrlPoints();

	void setNumberOfVertices(int number){ m_numberOfVertices = number; }

	void calculateAndAddNormal(int);

	void calculateNormalMapPixels(int);

private:
	//PixelBuffers
	nana::paint::pixel_buffer* m_imagePixels;
	nana::paint::pixel_buffer* m_heightMapPixels;
	nana::paint::pixel_buffer* m_normalMapPixels;

	std::vector<nana::pixel_color_t> t;
	
	//States
	states::HM_State hm_state;
	states::D_State d_state;

	//Changeable Points
	//ControlPoints of the drawn beziercurve, hermitespline or rectangle
	std::vector<glm::vec2> m_controlPoints;
	//All Vertices of the beziercurve
	std::vector<glm::vec2> m_bezierVertices;
	std::vector<glm::vec2> m_bezierNormals;

	int m_numberOfVertices = 0;
};

