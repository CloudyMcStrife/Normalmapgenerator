#pragma once

#include "Model.h"
#include <nana/gui/wvl.hpp>
#include <nana/gui/filebox.hpp>
#include <glm/glm.hpp>

class MainView;

class MainPresenter
{
public:
	MainPresenter();
	MainPresenter(Model*);
	~MainPresenter();

	void setView(MainView* view);

	void loadImage(nana::string);

	void saveGraphic(nana::paint::graphics&, const char*);

	void openFileBox();

	bool isSpriteSheet();

	void drawSpline(nana::paint::graphics&);
	void drawRect(nana::paint::graphics&);

	void calcTangents(glm::mat4, glm::mat4, glm::mat2x4, glm::mat2x4&);

	bool isEditing(){return m_edit;}

	void imgLabelClicked(int x, int y);

	void moveCtrlPoint(int, int);

	void activateDragCtrlPoint(int, int);

	int ctrlPointDragged = -1;

	bool pointInTriangle(glm::vec2, glm::vec2, glm::vec2, glm::vec2);

	glm::vec2 getSplineMiddlePoint(nana::paint::graphics&);

	int size();

private:
	MainView* m_view;
	Model* m_model;

	nana::paint::image img;
	bool m_edit;

};

