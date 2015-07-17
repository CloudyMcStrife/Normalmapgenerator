#include "MainPresenter.h"
#include "MainView.h"

#define CTRL_POINT_WIDTH 20
#define CTRL_POINT_HEIGHT 20

MainPresenter::MainPresenter(){

}

MainPresenter::MainPresenter(Model* model) : m_model(model), m_edit(true)
{
	img = nana::paint::image (STR("hallo.bmp"));
}

MainPresenter::~MainPresenter()
{
	delete m_model;
	delete m_view;
}

void MainPresenter::setView(MainView* view){
	m_view = view;
}

void MainPresenter::loadImage(nana::string file){
	img = nana::paint::image(file);
	m_view->setImageHandle(img);
}

#include <nana/gui.hpp>

void MainPresenter::openFileBox(){
	nana::filebox fb(true);

	fb.add_filter(L"Text File", L"*.text;*.doc");

	if (fb())	//When user clicked 'OK'
	{
		nana::string file = fb.file();
	}
}

void MainPresenter::imgLabelClicked(int x, int y){
	if (m_model->getCtrlSize() < 4)
		m_model->addControlPoint(x, y);
}

//Saving graphic into file (Bmp???)
void MainPresenter::saveGraphic(nana::paint::graphics& graphic, const char* filename){
	graphic.save_as_file("C:\\vcredist2.jpg");
}

bool MainPresenter::isSpriteSheet(){
	nana::paint::graphics g;
	img.paste(img.size(), g, nana::point());
	
	nana::paint::pixel_buffer pixels(g.handle(), img.size());
	return true;
}

void hermiteLeft(glm::mat4& mat){
	for (int i = 0; i < 4; ++i){
		mat[(i + 3) % 4][i] = 1;
		mat[i][i] = 4;
		mat[(i + 1) % 4][i] = 1;
	}
}

void hermiteRight(glm::mat4& mat){
	for (int i = 0; i < 4; ++i){
		mat[(i + 3) % 4][i] = -3;
		mat[(i + 1) % 4][i] = 3;
	}
}

#include <iostream>

void MainPresenter::calcTangents(glm::mat4 left, glm::mat4 right, glm::mat2x4 points, glm::mat2x4& tangents){
	glm::mat4 invLeft = glm::inverse(left);
	tangents = invLeft *right * points;
}

void MainPresenter::activateDragCtrlPoint(int x, int y){
	std::cout << x << " " << y << std::endl;
	for (int i = 0; i < m_model->getCtrlSize(); ++i){
		
		int ctrl_x = m_model->getCtrlPoint_X(i);
		int ctrl_y = m_model->getCtrlPoint_Y(i);
		std::cout << ctrl_x << " " << ctrl_y << std::endl;
		bool xrange = (x >= (ctrl_x - CTRL_POINT_WIDTH*0.5) && x <= (ctrl_x + CTRL_POINT_WIDTH*0.5));
		bool yrange = (y >= (ctrl_y - CTRL_POINT_HEIGHT*0.5) && y <= (ctrl_y + CTRL_POINT_HEIGHT*0.5));
		std::cout << xrange << " " << yrange << std::endl;
		if (xrange && yrange){
			ctrlPointDragged = i;
			return;
		}
	}
	ctrlPointDragged = -1;
}

void MainPresenter::moveCtrlPoint(int x, int y){
	if (ctrlPointDragged != -1){
		m_model->setCtrlPoint_X(ctrlPointDragged, x);
		m_model->setCtrlPoint_Y(ctrlPointDragged, y);
	}
}

bool MainPresenter::pointInTriangle(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p){
	glm::vec2 v0 = c - a;
	glm::vec2 v1 = b - a;
	glm::vec2 v2 = p - a;
	
	// Compute dot products
	float dot00 = glm::dot(v0, v0);
	float dot01 = glm::dot(v0, v1);
	float dot02 = glm::dot(v0, v2);
	float dot11 = glm::dot(v1, v1);
	float dot12 = glm::dot(v1, v2);
	
	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);
}

glm::vec2 getIntersectionPoint(glm::vec2 point1, glm::vec2 direction1, glm::vec2 point2, glm::vec2 direction2){
	
	glm::vec2 res, v_min, v_mul;
	v_min = point2 - point1;
	v_mul = (point2+direction2) * -1.0f;
	double D_s = v_min.x * v_mul.y - v_min.y * v_mul.x;
	glm::vec2 m_v2 = (point1 + direction1);
	float D = m_v2.x * v_mul.y - m_v2.y * v_mul.x;
	float s = D_s / D;
	//Ich weiß, dass ich hier noch prüfen müsste ob die beiden parallel liegen, weil sonst eine Division durch 0 auftritt.
	res = point1 + (m_v2 * s);
	return res;
	
}

//Da es im Moment nur 4 Punkte gibt, ist diese Methode ok
glm::vec2 MainPresenter::getSplineMiddlePoint(nana::paint::graphics& g){
	std::vector<glm::vec2> directionVecs;
	/*for (int i = 0; i < m_model->getCtrlSize()-1; ++i){
		for (int j = (i+1); j < m_model->getCtrlSize(); ++j){
			glm::vec2 pointi(m_model->getCtrlPoint_X(i), m_model->getCtrlPoint_Y(i));
			glm::vec2 pointj(m_model->getCtrlPoint_X(j), m_model->getCtrlPoint_Y(j));
			glm::vec2 directionVec = pointj - pointi;
			directionVecs.push_back(directionVec);
		}
	}*/
	/*
	for (int i = 0; i < 4; ++i){
		glm::vec2 point1(m_model->getCtrlPoint_X(i), m_model->getCtrlPoint_Y(i));
		glm::vec2 point2(m_model->getCtrlPoint_X((i+1)%4), m_model->getCtrlPoint_Y((i+1)%4));
		glm::vec2 dir(0);
		if ((i % 2) == 0){
			dir = point2 - point1;
		}else{
			dir = -(point2 - point1);
		}
		directionVecs.push_back(dir);
	}

	glm::vec2 middlePoint(m_model->getCtrlPoint_X(0), m_model->getCtrlPoint_Y(0));
	std::cout << "Middle: " << middlePoint.x << " " << middlePoint.y << std::endl;
	//int size = directionVecs.size();
	for (int k = 0; k < directionVecs.size(); ++k){
		middlePoint += ((directionVecs[k]) * 0.25f);
		std::cout << k << ": " << directionVecs[k].x << " " << directionVecs[k].y << std::endl;
		std::cout << k << ": " << directionVecs[k].x*0.25f << " " << directionVecs[k].y*0.25f << std::endl;
	}*/

	glm::vec2 middlePoint;

	glm::vec2 recPoints[4];
	glm::vec2 triangle1[3];
	glm::vec2 triangle2[3];

	for (int i = 0; i < 4; ++i){
		glm::vec2 point1(m_model->getCtrlPoint_X(i), m_model->getCtrlPoint_Y(i));
		recPoints[i] = point1;
	}
	triangle1[0] = recPoints[0];
	triangle1[1] = recPoints[1];
	triangle1[2] = recPoints[2];

	triangle2[0] = recPoints[0];
	triangle2[1] = recPoints[2];
	triangle2[2] = recPoints[3];

	glm::vec2 centroidT1;
	glm::vec2 centroidT2;

	glm::vec2 t1MiddlePoints[3];
	glm::vec2 t2MiddlePoints[3];

	for (int i = 0; i < 3; ++i){
		t1MiddlePoints[i] = triangle1[i] + (triangle1[i] - triangle1[(i + 1) % 3]) * 0.5f;
		t2MiddlePoints[i] = triangle2[i] + (triangle2[i] - triangle2[(i + 1) % 3]) * 0.5f;
		g.round_rectangle(nana::rectangle(t1MiddlePoints[i].x, t1MiddlePoints[i].y, CTRL_POINT_WIDTH, CTRL_POINT_HEIGHT), CTRL_POINT_WIDTH, CTRL_POINT_HEIGHT, nana::colors::black, false, nana::colors::red);
		g.round_rectangle(nana::rectangle(t2MiddlePoints[i].x, t2MiddlePoints[i].y, CTRL_POINT_WIDTH, CTRL_POINT_HEIGHT), CTRL_POINT_WIDTH, CTRL_POINT_HEIGHT, nana::colors::black, false, nana::colors::red);
	}
	
	glm::vec2 dirT1[3];
	glm::vec2 dirT2[3];
	
	for (int i = 0; i < 3; ++i){
		dirT1[i] = t1MiddlePoints[(i + 1) % 3] - triangle1[i];
		dirT2[i] = t2MiddlePoints[(i + 1) % 3] - triangle2[i];
	}
	
	glm::vec2 intersectionPoint = getIntersectionPoint(triangle1[0], dirT1[0], triangle1[1], dirT1[1]);
	g.round_rectangle(nana::rectangle(intersectionPoint.x, intersectionPoint.y, CTRL_POINT_WIDTH, CTRL_POINT_HEIGHT), CTRL_POINT_WIDTH, CTRL_POINT_HEIGHT, nana::colors::black, false, nana::colors::red);

	
	return middlePoint;

}

void MainPresenter::drawSpline(nana::paint::graphics& g){
	if (m_model->getCtrlSize() >= 4){

		glm::mat2x4 points(0);
		for (int i = 0; i < 4; ++i){
			points[0][i] = m_model->getCtrlPoint_X(i);
			points[1][i] = m_model->getCtrlPoint_Y(i);
		}

		glm::mat4 _hermiteLeft(0);
		hermiteLeft(_hermiteLeft);
		glm::mat4 _hermiteRight(0);
		hermiteRight(_hermiteRight);
		
		double step = 1.0 / 32.0;

		glm::mat2x4 tangents(0);

		calcTangents(_hermiteLeft, _hermiteRight, points, tangents);

		for (int i = 0; i < 4; ++i){

			glm::mat2x4 pt;
			pt[0][0] = points[0][i];
			pt[1][0] = points[1][i];
			pt[0][1] = points[0][(i + 1) % 4];
			pt[1][1] = points[1][(i + 1) % 4];
			pt[0][2] = tangents[0][i];
			pt[1][2] = tangents[1][i];
			pt[0][3] = tangents[0][(i + 1) % 4];
			pt[1][3] = tangents[1][(i + 1) % 4];

			glm::vec2 oldPoint = glm::vec2(points[0][i], points[1][i]);

			bool drawLine = true;

			for (double t = 0.0; t < 1.0; t += step){

				double h0 = 2 * t*t*t - 3 * t*t + 1;
				double h1 = -2 * t*t*t + 3 * t*t;
				double h2 = t*t*t - 2 * t*t + t;
				double h3 = t*t*t - t*t;

				glm::vec4 bindFunction(h0, h1, h2, h3);

				glm::vec2 newPoint = bindFunction * pt;

				if (drawLine)
					g.line(nana::point(oldPoint.x, oldPoint.y), nana::point(newPoint.x, newPoint.y), nana::colors::green);

				oldPoint = newPoint;
				drawLine = !drawLine;
			}

			g.line(nana::point(oldPoint.x, oldPoint.y), nana::point(points[0][(i + 1) % 4], points[1][(i + 1) % 4]), nana::colors::green);
			glm::vec2 middle = getSplineMiddlePoint(g);
			g.round_rectangle(nana::rectangle(middle.x, middle.y, 10, 10), 10, 10, nana::colors::blanched_almond, true, nana::colors::alice_blue);
			std::cout << middle.x << " " << middle.y << std::endl;
		}															
	}

	for (int i = 0; i < m_model->getCtrlSize(); ++i){
		int x = m_model->getCtrlPoint_X(i) - CTRL_POINT_WIDTH*0.5;
		int y = m_model->getCtrlPoint_Y(i) - CTRL_POINT_HEIGHT*0.5;
		g.round_rectangle(nana::rectangle(x, y, CTRL_POINT_WIDTH, CTRL_POINT_HEIGHT), CTRL_POINT_WIDTH, CTRL_POINT_HEIGHT, nana::colors::black, false, nana::colors::red);
	}
}






