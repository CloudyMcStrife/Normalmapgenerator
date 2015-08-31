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

	void openFileBox();

	bool isSpriteSheet();

	void draw(nana::paint::graphics&);
	void drawCircle(nana::paint::graphics&);
	void drawSpline(nana::paint::graphics&);
	void drawRect(nana::paint::graphics&);
	void drawBezier(nana::paint::graphics&);
	void drawBezierNormals(nana::paint::graphics&);
	glm::vec2 getBezierNormal(glm::vec2, glm::vec2);

	void changeD_State(states::D_State);

	void calcTangents(glm::mat4, glm::mat4, glm::mat2x4, glm::mat2x4&);

	bool isEditing(){return m_edit;}

	void imgLabelClicked(int x, int y);

	void moveCtrlPoint(int, int);
	void moveBezierPoint(int, int);
	void moveMiddlePoint(int, int);

	void activateDragCtrlPoint(int, int);
	void activateDragNormalPoint(int, int);

	void fillHM_Graph(nana::paint::graphics&, int, int);
	void showNormalmap(nana::paint::graphics&);
	void saveNormalmap(nana::paint::graphics&);

	int ctrlPointDragged = -1;
	int normalPointDragged = -1;
	bool m_normalsInitialized = false;

	bool pointInTriangle(glm::vec2, glm::vec2, glm::vec2, glm::vec2);

	glm::vec2 getSplineMiddlePoint(nana::paint::graphics&);

	int size();

	bool apply = false;
	bool m_edit = false;
	bool loadedNew = false;
	bool normal = false;
	int blur = 1;

private:
	MainView* m_view;
	Model* m_model;

	nana::paint::image img;
		

};

