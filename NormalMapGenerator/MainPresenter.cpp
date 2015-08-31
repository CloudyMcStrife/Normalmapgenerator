#include "MainPresenter.h"
#include "MainView.h"

#include "atlstr.h"

/*
TODO: Checkboxes: invert color, ...
*/

#define CTRL_POINT_WIDTH 20
#define CTRL_POINT_HEIGHT 20

MainPresenter::MainPresenter(){

}

MainPresenter::MainPresenter(Model* model) : m_model(model), m_edit(true)
{
	img = nana::paint::image(STR("hallo.bmp"));
}

MainPresenter::~MainPresenter()
{
	delete m_model;
	delete m_view;
}

void MainPresenter::setView(MainView* view){
	m_view = view;
}

int MainPresenter::size(){
	return m_model->getCtrlSize();
}
#include <iostream>
void MainPresenter::loadImage(nana::string file){
	loadedNew = true;
	m_model->clearCtrlPoints();
	img = nana::paint::image(file);
	m_view->setImageHandle(img);

	nana::paint::graphics imgGraphics(img.size());

	img.paste(nana::rectangle(img.size()), imgGraphics, nana::point());
	if (img.alpha())
		std::cout << "Alpha daa" << std::endl;

	m_model->setImagePixelBuffer(imgGraphics.handle(), 400, 600);
	int a = 0;
}

void MainPresenter::showNormalmap(nana::paint::graphics& g){
	m_model->calculateNormalMapPixels(blur);
	nana::paint::pixel_buffer* normalBuffer = m_model->getNormalBuffer();
	normalBuffer->paste(nana::rectangle(normalBuffer->size()), g.handle(), nana::point());
}

void MainPresenter::saveNormalmap(nana::paint::graphics& g){
	nana::filebox fb(true);

	fb.add_filter(L"Image File", L"*.bmp;*.jpg");
	nana::string file;
	if (fb())	//When user clicked 'OK'
	{
		file = fb.file();
	}
	
	const wchar_t *orig = file.c_str();

	size_t origsize = wcslen(orig) + 1;
	size_t convertedChars = 0;

	char strConcat[] = "";
	size_t strConcatsize = (strlen(strConcat) + 1) * 2;

	const size_t newsize = origsize * 2;
	char *nstring = new char[newsize + strConcatsize];

	wcstombs_s(&convertedChars, nstring, newsize, orig, _TRUNCATE);
	_mbscat_s((unsigned char*)nstring, newsize + strConcatsize, (unsigned char*)strConcat);
	std::cout << nstring << std::endl;
	g.save_as_file(nstring);
	
}

void MainPresenter::fillHM_Graph(nana::paint::graphics& g, int width, int height){
	int factor = 10;
	if (m_edit && m_model->getD_State() == states::BEZIER){
		nana::paint::pixel_buffer* buff = m_model->getHM_PixelBuffer();
		buff->paste(nana::rectangle(0, 0, 400, 600), g.handle(), nana::point());
		/*if (loadedNew){
		buff->paste(nana::rectangle(nana::size(400, 600)), g.handle(), nana::point());
		loadedNew = false;
		}
		else{
		}*/
		//buff->paste(nana::rectangle(nana::size(400, 600)), g.handle(), nana::point());
		if (m_model->getCtrlPoints().size() > 3){
			nana::paint::pixel_buffer gBuff = nana::paint::pixel_buffer(g.handle(), buff->size().width, buff->size().height);
			int sliderV = m_view->getSliderValue();
			std::vector<glm::vec2> vertices = m_model->getBezierVertices();
			for (int i = 0; i < m_model->getBezierSize() - 1; ++i){
				glm::vec2 normal = m_model->getBezierNormal(i);
				glm::vec2 vertex = vertices[i];
				for (float s = 0.0f; s < 1.0f; s += 0.01f){

					vertex = vertices[i] + s*(vertices[i + 1] - vertices[i]);

					normal = normal*(1 - s) + s*m_model->getBezierNormal(i + 1);
					normal = glm::normalize(normal);

					//Color to interpolate to
					glm::vec2 endPoint = vertex + normal * (float)sliderV;

					nana::color beginColorTMP(sliderV, sliderV, sliderV);
					nana::pixel_color_t beginColor = beginColorTMP.px_color();
					nana::pixel_color_t endColor = buff->pixel(endPoint.x, endPoint.y);
					for (float t = 0.0f; t < sliderV; t += 1){
						glm::vec2 normalRunner = vertex + normal * t;
						glm::vec2 normalRunnerM = vertex - normal * t;
						nana::pixel_color_t c = buff->pixel(normalRunner.x, normalRunner.y);
						nana::pixel_color_t cM = buff->pixel(normalRunnerM.x, normalRunnerM.y);

						glm::vec2 distanceVector = normalRunner - vertex;
						float length = glm::length(distanceVector);
						int grey = 255 - (((t * 255) / sliderV));
						int greyM = grey;

						nana::pixel_color_t b = buff->pixel(normalRunner.x, normalRunner.y);
						float rWeight = b.element.red / 255.0f + 0.1f;
						float gWeight = b.element.green / 255.0f + 0.1f;
						float bWeight = b.element.blue / 255.0f + 0.1f;
						float middleWeight = (rWeight + gWeight + bWeight) / 3;
						middleWeight = (middleWeight > 1) ? 1 : middleWeight;
						grey *= middleWeight;
						grey *= (float)(m_view->getBrightnessValue()  * factor);
						grey = (grey > 255) ? 255 : grey;
						grey = (m_view->invert()) ? 255 - grey : grey;

						nana::color color = nana::color(grey, grey, grey);

						nana::pixel_color_t bM = buff->pixel(normalRunnerM.x, normalRunnerM.y);
						float rWeightM = bM.element.red / 255.0f + 0.1f;
						float gWeightM = bM.element.green / 255.0f + 0.1f;
						float bWeightM = bM.element.blue / 255.0f + 0.1f;
						float middleWeightM = (rWeightM + gWeightM + bWeightM) / 3;
						middleWeightM = (middleWeightM > 1) ? 1 : middleWeightM;

						greyM *= middleWeightM;
						greyM *= m_view->getBrightnessValue() * factor;
						greyM = (greyM > 255) ? 255 : greyM;
						greyM = (m_view->invert()) ? 255 - greyM : greyM;
						nana::color colorM = nana::color(greyM, greyM, greyM);

						nana::pixel_color_t colGBuff = gBuff.pixel(normalRunner.x, normalRunner.y);
						nana::pixel_color_t colBuff = buff->pixel(normalRunner.x, normalRunner.y);

						nana::pixel_color_t colGBuffM = gBuff.pixel(normalRunnerM.x, normalRunnerM.y);
						nana::pixel_color_t colBuffM = buff->pixel(normalRunnerM.x, normalRunnerM.y);

						if (c.element.red == 255 && c.element.green == 254){
							nana::color white(255, 254, 255);
							g.set_pixel((int)normalRunner.x, (int)normalRunner.y, white);
							gBuff.pixel((int)normalRunner.x, (int)normalRunner.y, white.px_color());
						}
						else{
							g.set_pixel((int)normalRunner.x, (int)normalRunner.y, color);
							gBuff.pixel((int)normalRunner.x, (int)normalRunner.y, color.px_color());
						}

						if (cM.element.red == 255 && cM.element.green == 254){
							nana::color white(255, 254, 255);
							g.set_pixel((int)normalRunnerM.x, (int)normalRunnerM.y, white);
							gBuff.pixel((int)normalRunnerM.x, (int)normalRunnerM.y, white.px_color());
						}
						else{
							g.set_pixel((int)normalRunnerM.x, (int)normalRunnerM.y, colorM);
							gBuff.pixel((int)normalRunnerM.x, (int)normalRunnerM.y, colorM.px_color());
						}
					}
				}
			}
			vertices = m_model->getBezierVertices();
			for (int i = 0; i < 2; ++i){
				glm::vec2 normal = m_model->getBezierNormal(i*(vertices.size() - 1));
				glm::vec2 vertex = vertices[i*(vertices.size() - 1)];
				glm::vec2 helpVert = (i == 0) ? vertices[1] : vertices[vertices.size() - 2];
				glm::vec2 helpVec = glm::normalize(vertex - helpVert);
				glm::vec2 normalM = -normal;

				for (float s = 0.0f; s < 1.0f; s += 0.01f){
					normalM = normalM*(1 - s) + helpVec*s;
					normalM = glm::normalize(normalM);
					normal = normal*(1 - s) + s*helpVec;
					normal = glm::normalize(normal);
					//normalM = glm::vec2(normal.x, -normal.y);
					for (float t = 0.0f; t < sliderV; t += 1){

						glm::vec2 normalRunner = vertex + normal * t;
						glm::vec2 normalRunnerM = vertex + normalM * t;

						nana::pixel_color_t c = buff->pixel(normalRunner.x, normalRunner.y);
						nana::pixel_color_t cM = buff->pixel(normalRunnerM.x, normalRunnerM.y);

						glm::vec2 distanceVector = normalRunner - vertex;
						float length = glm::length(distanceVector);
						int grey;
						grey = 255 - (((t * 255) / sliderV));
						int greyM = grey;

						nana::pixel_color_t b = buff->pixel(normalRunner.x, normalRunner.y);
						float rWeight = b.element.red / 255.0f + 0.1f;
						float gWeight = b.element.green / 255.0f + 0.1f;
						float bWeight = b.element.blue / 255.0f + 0.1f;
						float middleWeight = (rWeight + gWeight + bWeight) / 3;
						middleWeight = (middleWeight > 1) ? 1 : middleWeight;
						grey *= middleWeight;
						grey *= (float)(m_view->getBrightnessValue()  * factor);
						grey = (grey > 255) ? 255 : grey;
						grey = (m_view->invert()) ? 255 - grey : grey;
						nana::color color = nana::color(grey, grey, grey);

						nana::pixel_color_t bM = buff->pixel(normalRunnerM.x, normalRunnerM.y);
						float rWeightM = bM.element.red / 255.0f + 0.1f;
						float gWeightM = bM.element.green / 255.0f + 0.1f;
						float bWeightM = bM.element.blue / 255.0f + 0.1f;
						float middleWeightM = (rWeightM + gWeightM + bWeightM) / 3;
						middleWeightM = (middleWeightM > 1) ? 1 : middleWeightM;
						nana::color a = nana::color(bM.element.red, bM.element.green, bM.element.blue);
						greyM *= middleWeightM;
						greyM *= m_view->getBrightnessValue() * factor;
						greyM = (greyM > 255) ? 255 : greyM;
						greyM = (m_view->invert()) ? 255 - greyM : greyM;
						nana::color colorM = nana::color(greyM, greyM, greyM, 1.0);

						if (c.element.red == 255 && c.element.green == 254){
							nana::color white(255, 254, 255);
							g.set_pixel((int)normalRunner.x, (int)normalRunner.y, white);
							gBuff.pixel((int)normalRunner.x, (int)normalRunner.y, white.px_color());
						}
						else{
							g.set_pixel((int)normalRunner.x, (int)normalRunner.y, color);
							gBuff.pixel((int)normalRunner.x, (int)normalRunner.y, color.px_color());
						}

						if (cM.element.red == 255 && cM.element.green == 254){
							nana::color white(255, 254, 255);
							g.set_pixel((int)normalRunnerM.x, (int)normalRunnerM.y, white);
							gBuff.pixel((int)normalRunnerM.x, (int)normalRunnerM.y, white.px_color());
						}
						else{
							g.set_pixel((int)normalRunnerM.x, (int)normalRunnerM.y, colorM);
							gBuff.pixel((int)normalRunnerM.x, (int)normalRunnerM.y, colorM.px_color());
						}

					}
				}
			}
		}
		if (apply){
			buff->open(g.handle());
			//m_model->clearCtrlPoints();
			apply = false;
		}
		g.blur(nana::rectangle(g.size()), blur);
	}
	else if (m_edit && m_model->getD_State() == states::CIRCLE){
		nana::paint::pixel_buffer* buff = m_model->getHM_PixelBuffer();
		buff->paste(nana::rectangle(0, 0, 400, 600), g.handle(), nana::point());
		glm::vec2 middlePoint = m_model->getCtrlPoints()[0];
		glm::vec2 sidePoint = m_model->getCtrlPoints()[1];
		float radius = glm::length(sidePoint - middlePoint);
		if (m_model->getCtrlPoints().size() > 1){
			int sliderV = m_view->getSliderValue();
			int width = g.size().width;
			int height = g.size().height;
			for (int x = 0; x < width; ++x){
				for (int y = 0; y < height; ++y){
					glm::vec2 point(x, y);
					float distance = glm::length(point - middlePoint);
					if (distance <= radius){
						int grey;
						grey = 255 * (1 - (distance / radius));
						nana::pixel_color_t b = buff->pixel(x, y);
						float rWeight = b.element.red / 255.0f + 0.1f;
						float gWeight = b.element.green / 255.0f + 0.1f;
						float bWeight = b.element.blue / 255.0f + 0.1f;
						float middleWeight = (rWeight + gWeight + bWeight) / 3;
						middleWeight = (middleWeight > 1) ? 1 : middleWeight;
						grey *= middleWeight;
						grey *= (float)(m_view->getBrightnessValue()  * factor);
						grey = (grey > 255) ? 255 : grey;
						grey = (m_view->invert()) ? 255 - grey : grey;
						nana::color greyC(grey, grey, grey);
						nana::pixel_color_t c = buff->pixel(x, y);
						if (c.element.red != 255 && c.element.green != 254){
							g.set_pixel(x, y, greyC);
						}
					}
				}
			}
		}
		if (apply){
			buff->open(g.handle());
			//m_model->clearCtrlPoints();
			apply = false;
		}
		g.blur(nana::rectangle(g.size()), blur);
	}
}

#include <nana/gui.hpp>

void MainPresenter::openFileBox(){
	nana::filebox fb(true);

	fb.add_filter(L"Image File", L"*.bmp;*.jpg");

	if (fb())	//When user clicked 'OK'
	{
		nana::string file = fb.file();
	}
}

void MainPresenter::imgLabelClicked(int x, int y){
	if (m_model->getCtrlSize() < 4)
		m_model->addControlPoint(x, y);
}

bool MainPresenter::isSpriteSheet(){
	nana::paint::graphics g;
	img.paste(nana::rectangle(img.size()), g, nana::point());

	nana::paint::pixel_buffer pixels(g.handle(), img.size().width, img.size().height);
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
	for (int i = 0; i < m_model->getCtrlSize(); ++i){

		int ctrl_x = m_model->getCtrlPoint_X(i);
		int ctrl_y = m_model->getCtrlPoint_Y(i);
		bool xrange = (x >= (ctrl_x - CTRL_POINT_WIDTH*0.5) && x <= (ctrl_x + CTRL_POINT_WIDTH*0.5));
		bool yrange = (y >= (ctrl_y - CTRL_POINT_HEIGHT*0.5) && y <= (ctrl_y + CTRL_POINT_HEIGHT*0.5));
		if (xrange && yrange){
			normalPointDragged = -1;
			ctrlPointDragged = i;
			return;
		}
	}
	ctrlPointDragged = -1;
}

void MainPresenter::activateDragNormalPoint(int x, int y){
	for (int i = 0; i < m_model->getBezierSize(); ++i){

		glm::vec2 bezierPoint = m_model->getBezierVertex(i);
		glm::vec2 normal = m_model->getBezierNormal(i);

		glm::vec2 point = bezierPoint + 10.0f * normal;
		bool xrange = (x >= (point.x - CTRL_POINT_WIDTH*0.5) && x <= (point.x + CTRL_POINT_WIDTH*0.5));
		bool yrange = (y >= (point.y - CTRL_POINT_HEIGHT*0.5) && y <= (point.y + CTRL_POINT_HEIGHT*0.5));
		if (xrange && yrange){
			ctrlPointDragged = -1;
			normalPointDragged = i;
			return;
		}
	}
	normalPointDragged = -1;
}

void MainPresenter::moveCtrlPoint(int x, int y){
	if (ctrlPointDragged != -1){
		m_model->setCtrlPoint(ctrlPointDragged, glm::vec2(x, y));
	}
	std::cout << ctrlPointDragged << std::endl;
}

void MainPresenter::moveBezierPoint(int x, int y){
	if (normalPointDragged != -1){
		m_model->setBezierNormal(normalPointDragged, glm::vec2(x, y));
	}
}

void MainPresenter::moveMiddlePoint(int x, int y){
	if (ctrlPointDragged != -1){
		m_model->setMiddlePoint(glm::vec2(x, y));
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

//Da es im Moment nur 4 Punkte gibt, ist diese Methode ok
glm::vec2 MainPresenter::getSplineMiddlePoint(nana::paint::graphics& g){
	std::vector<glm::vec2> directionVecs;

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

	int width = CTRL_POINT_WIDTH / 2;
	int height = CTRL_POINT_HEIGHT / 2;
	for (int i = 0; i < 3; ++i){
		t1MiddlePoints[i] = triangle1[i] + (triangle1[(i + 1) % 3] - triangle1[i]) * 0.5f;
		t2MiddlePoints[i] = triangle2[i] + (triangle2[(i + 1) % 3] - triangle2[i]) * 0.5f;
		g.round_rectangle(nana::rectangle(t1MiddlePoints[i].x - width / 2, t1MiddlePoints[i].y - height / 2, width, height), width, height, nana::colors::black, true, nana::colors::red);
		g.round_rectangle(nana::rectangle(t2MiddlePoints[i].x - width / 2, t2MiddlePoints[i].y - height / 2, width, height), width, height, nana::colors::black, true, nana::colors::blue_violet);
	}

	glm::vec2 dirT1[3];
	glm::vec2 dirT2[3];

	for (int i = 0; i < 3; ++i){
		dirT1[i] = t1MiddlePoints[(i + 1) % 3] - triangle1[i];
		g.line(nana::point(triangle1[i].x, triangle1[i].y), nana::point(t1MiddlePoints[(i + 1) % 3].x, t1MiddlePoints[(i + 1) % 3].y));
		dirT2[i] = t2MiddlePoints[(i + 1) % 3] - triangle2[i];
		g.line(nana::point(triangle2[i].x, triangle2[i].y), nana::point(t2MiddlePoints[(i + 1) % 3].x, t2MiddlePoints[(i + 1) % 3].y));
	}

	glm::vec2 t1MiddlePoint;
	glm::vec2 t2MiddlePoint;

	t1MiddlePoint.x = (1.0f / 3.0f) * (triangle1[0].x + triangle1[1].x + triangle1[2].x);
	t1MiddlePoint.y = (1.0f / 3.0f) * (triangle1[0].y + triangle1[1].y + triangle1[2].y);

	t2MiddlePoint.x = (1.0f / 3.0f) * (triangle2[0].x + triangle2[1].x + triangle2[2].x);
	t2MiddlePoint.y = (1.0f / 3.0f) * (triangle2[0].y + triangle2[1].y + triangle2[2].y);

	g.round_rectangle(nana::rectangle(t1MiddlePoint.x - width / 2, t1MiddlePoint.y - height / 2, width, height), width, height, nana::colors::black, false, nana::colors::black);
	g.round_rectangle(nana::rectangle(t2MiddlePoint.x - width / 2, t2MiddlePoint.y - height / 2, width, height), width, height, nana::colors::black, false, nana::colors::black);

	middlePoint = t1MiddlePoint + 0.5f*(t2MiddlePoint - t1MiddlePoint);
	return middlePoint;

}

void MainPresenter::changeD_State(states::D_State state){
	m_model->setD_State(state);
}

glm::vec2 MainPresenter::getBezierNormal(glm::vec2 point, glm::vec2 middle)
{
	//glm::vec2 n = m_model->getBezierNormal(index);
	glm::vec2 direction = point - middle;
	glm::vec2 normal = glm::normalize(direction);
	return normal;
}

void MainPresenter::draw(nana::paint::graphics& g){
	states::D_State drawState = m_model->getD_State();
	int ctrlPointsDraw = 0;
	switch (drawState){
	case (states::RECT) :
		drawRect(g);
		ctrlPointsDraw = 4;
		//m_normalsInitialized = false;
		break;
	case(states::D_State::BEZIER) :
		ctrlPointsDraw = 4;
		drawBezier(g);
		drawBezierNormals(g);
		break;
	case(states::D_State::CIRCLE) :
		ctrlPointsDraw = 2;
		drawCircle(g);
		break;
	case(states::D_State::SPLINE) :
		//m_normalsInitialized = false;
		ctrlPointsDraw = 4;
		drawSpline(g);
		break;
	}

	for (int i = 0; i < m_model->getCtrlSize(); ++i){
		int x = m_model->getCtrlPoint_X(i) - CTRL_POINT_WIDTH*0.5;
		int y = m_model->getCtrlPoint_Y(i) - CTRL_POINT_HEIGHT*0.5;
		g.round_rectangle(nana::rectangle(x, y, CTRL_POINT_WIDTH, CTRL_POINT_HEIGHT), CTRL_POINT_WIDTH, CTRL_POINT_HEIGHT, nana::colors::black, false, nana::colors::red);
		std::cout << ctrlPointsDraw << " " << i - 1 << std::endl;
		if (ctrlPointsDraw == (i + 1)){
			break;
		}
	}
}

void MainPresenter::drawSpline(nana::paint::graphics& g){
	if (m_model->getCtrlSize() >= 4){
		m_edit = true;
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
			g.round_rectangle(nana::rectangle(middle.x - 5, middle.y - 5, 10, 10), 10, 10, nana::colors::black, true, nana::colors::black);
		}
	}
	else{
		m_edit = false;
	}
}

void MainPresenter::drawRect(nana::paint::graphics& g){
	if (m_model->getCtrlSize() >= 4){
		m_edit = true;
		for (int i = 0; i < 4; ++i){

			glm::vec2 point = glm::vec2(m_model->getCtrlPoint_X(i), m_model->getCtrlPoint_Y(i));
			glm::vec2 oldPoint = point;
			glm::vec2 endPoint = glm::vec2(m_model->getCtrlPoint_X((i + 1) % 4), m_model->getCtrlPoint_Y((i + 1) % 4));

			glm::vec2 dir = endPoint - point;
			bool drawLine = true;

			float step = 1.0f / 32.0f;

			for (float t = 0.0f; t < 1.0f; t += step){
				glm::vec2 newPoint = point + t*dir;
				if (drawLine)
					g.line(nana::point(oldPoint.x, oldPoint.y), nana::point(newPoint.x, newPoint.y));

				oldPoint = newPoint;
				drawLine = !drawLine;
			}
		}
		glm::vec2 middle = getSplineMiddlePoint(g);
		g.round_rectangle(nana::rectangle(middle.x - 5, middle.y - 5, 10, 10), 10, 10, nana::colors::blanched_almond, true, nana::colors::alice_blue);
	}
	else{
		m_edit = false;
	}
}

void MainPresenter::drawCircle(nana::paint::graphics& g){
	if (m_model->getCtrlSize() >= 2){
		g.set_color(nana::colors::black);
		m_edit = true;
		glm::vec2 point = glm::vec2(m_model->getCtrlPoint_X(0), m_model->getCtrlPoint_Y(0));
		glm::vec2 oldPoint = point;
		glm::vec2 endPoint = glm::vec2(m_model->getCtrlPoint_X(1), m_model->getCtrlPoint_Y(1));
		g.line(nana::point(oldPoint.x, oldPoint.y), nana::point(endPoint.x, endPoint.y));
		float radius = glm::length(endPoint - point);
		bool drawLine = false;
		for (float theta = 0.f; theta < 360.f; ++theta){
			float x = point.x + radius * glm::cos(glm::radians(theta));
			float y = point.y + radius * glm::sin(glm::radians(theta));
			if (drawLine)
				g.line(nana::point(oldPoint.x, oldPoint.y), nana::point(x,y));

			oldPoint = glm::vec2(x,y);
			drawLine = !drawLine;
		}
		//g.round_rectangle(nana::rectangle(middle.x - 5, middle.y - 5, 10, 10), 10, 10, nana::colors::blanched_almond, true, nana::colors::alice_blue);
	}
	else{
		m_edit = false;
	}
}

void MainPresenter::drawBezier(nana::paint::graphics& g){
	m_model->clearBezierVertices();
	m_model->clearBezierNormals();

	g.set_color(nana::colors::black);
	int numPoints = m_model->getCtrlSize();

	if (numPoints >= 4){
		m_edit = true;
		//bool normalCalculationActivated = false;
		int index = 0;
		std::vector<glm::vec2> ctrlPoints = m_model->getCtrlPoints();
		glm::vec2 oldPoint = ctrlPoints[0];

		m_model->addBezierVertex(oldPoint);
		m_model->calculateAndAddNormal(index++);

		glm::vec2 lastPoint = ctrlPoints[numPoints - 1];
		glm::vec2 newPoint;

		float distance = glm::length(lastPoint - oldPoint);
		if (distance > 16.0f)
			distance = 16.0f;
		int stepDenominator = 16.0f;// m_view->getSliderValue();
		m_model->setNumberOfVertices(stepDenominator + 2);
		float stepSize = 1.0f / stepDenominator;
		float normalStep = stepDenominator / 10;
		float t = stepSize;
		while (t <= 1.0){
			ctrlPoints = m_model->getCtrlPoints();
			int size = numPoints;
			while (size > 1){
				std::vector<glm::vec2> halfPoints;
				for (int i = 0; i < ctrlPoints.size() - 1; ++i){
					glm::vec2 pointI = ctrlPoints[i];
					glm::vec2 pointIP1 = ctrlPoints[i + 1];

					glm::vec2 addPoint = pointI*(1 - t) + pointIP1*t;
					halfPoints.push_back(addPoint);
				}
				ctrlPoints = halfPoints;
				size--;
			}
			newPoint = ctrlPoints[0];
			m_model->addBezierVertex(newPoint);
			m_model->calculateAndAddNormal(index++);

			g.line(nana::point(oldPoint.x, oldPoint.y), nana::point(newPoint.x, newPoint.y));
			oldPoint = newPoint;

			t += stepSize;
		}
		g.line(nana::point(oldPoint.x, oldPoint.y), nana::point(lastPoint.x, lastPoint.y));
		//m_model->addBezierVertex(lastPoint);
		m_model->calculateAndAddNormal(index);
	}
	else{
		m_edit = false;
	}
	m_normalsInitialized = true;
}

void MainPresenter::drawBezierNormals(nana::paint::graphics& g){
	int size = m_model->getBezierSize();
	for (int i = 0; i < size; ++i){

		if (i % 2 == 0){
			glm::vec2 normal = m_model->getBezierNormal(i);
			glm::vec2 pointPosition = m_model->getBezierVertex(i);
			glm::vec2 drawPosition = pointPosition + normal*10.0f;

			nana::point p(pointPosition.x, pointPosition.y);
			nana::point d(drawPosition.x, drawPosition.y);
			g.set_color(nana::colors::green);
			g.line(p, d);

			int width = CTRL_POINT_WIDTH*0.2f;
			int height = CTRL_POINT_HEIGHT*0.2f;

			g.round_rectangle(nana::rectangle(drawPosition.x - width*0.5, drawPosition.y - height*0.5, width, height), width, height, nana::colors::red, true, nana::colors::red);
		}
	}
	g.set_color(nana::colors::black);
}






