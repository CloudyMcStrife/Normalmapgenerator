#include "Model.h"

Model::Model()
{
	hm_state = states::PLANE;
	d_state = states::CIRCLE;
	m_controlPoints = std::vector<glm::vec2>();
	m_bezierNormals = std::vector<glm::vec2>();
	m_bezierVertices = std::vector<glm::vec2>();
	
}


Model::~Model()
{
}

void Model::setImagePixelBuffer(nana::paint::drawable_type t, int width, int height){
	m_imagePixels = new nana::paint::pixel_buffer(t, nana::rectangle(nana::size(width, height)));
	clearHM_Buffer(width, height);
}

std::vector<glm::vec2> Model::getImagePixelBounds(int width, int height){
	bool isStart = false;
	bool isEnd = false;
	m_imagePixels->alpha_channel(true);
	int x = 0;
	//std::vector<glm::vec2> greyLine;
	for (int x = 0; x < width; ++x){
		glm::vec2 boundStart;
		glm::vec2 boundEnd;
		for (int y = 0; y < height; ++y){
			nana::pixel_argb_t c = m_imagePixels->pixel(x, y);
			
			
			int r = c.element.red;
			int g = c.element.green;
			int b = c.element.blue;
			int a = c.element.alpha_channel;
			
			if (a == 0){
				nana::color white(255, 254, 255);
				m_heightMapPixels->pixel(x, y, white.px_color());
			}
		}
	}
	return std::vector<glm::vec2>();
}

void Model::clearHM_Buffer(int width, int height)
{
	m_heightMapPixels = new nana::paint::pixel_buffer(width, height);
	nana::rectangle rect(0, 0, width, height);
	m_heightMapPixels->rectangle(rect, nana::colors::black, 0, true);
	std::vector<glm::vec2> bounds = getImagePixelBounds(width, height);
}

void Model::setHM_PixelAt(int x, int y, nana::color color){
	m_heightMapPixels->pixel(x, y, color.px_color());
}

void Model::addControlPoint(int x, int y){
	m_controlPoints.push_back(glm::vec2(x, y));
}

int Model::getCtrlPoint_X(int index){
	return m_controlPoints[index].x;
}

int Model::getCtrlPoint_Y(int index){
	return m_controlPoints[index].y;
}

void Model::setCtrlPoint(int index, glm::vec2 ctrlPoint){
	m_controlPoints[index] = ctrlPoint;
}

void Model::setMiddlePoint(glm::vec2 middlePoint){
	//m_controlPoints[index] = middlePoint;
}


void Model::addBezierData(glm::vec2 point, glm::vec2 normal){
	addBezierVertex(point);
	addBezierNormal(normal);
}

void Model::addBezierVertex(glm::vec2 bezierVertex){
	m_bezierVertices.push_back(bezierVertex);
}

glm::vec2 Model::getBezierVertex(int index){
	return m_bezierVertices[index];
}

void Model::setBezierNormal(int index, glm::vec2 mouse){
	glm::vec2 bezierPoint = getBezierVertex(index);
	glm::vec2 newNormal = glm::normalize(mouse - bezierPoint);
	
	m_bezierNormals[index] = newNormal;
}

int Model::getCtrlSize(){
	return m_controlPoints.size();
}

void Model::clearCtrlPoints(){
	m_controlPoints.clear();
}

nana::paint::pixel_buffer* Model::getHM_PixelBuffer(){
	return m_heightMapPixels;
}

void Model::calculateAndAddNormal(int index){
	if (index == (m_numberOfVertices-1)){
		glm::vec2 lastVertex = m_bezierVertices[index - 2];
		glm::vec2 vertex = m_bezierVertices[index-1];

		glm::vec2 directionVec = vertex - lastVertex;

		glm::vec3 up(0, 0, 1);

		glm::vec3 normal = glm::cross(glm::vec3(directionVec, 0), up);
		
		addBezierNormal(glm::normalize(glm::vec2(normal.x, normal.y)));
	}else if (index > 1){
		glm::vec2 lastVertex = m_bezierVertices[index - 2];
		glm::vec2 vertex = m_bezierVertices[index - 1];
		glm::vec2 nextVertex = m_bezierVertices[index];

		glm::vec2 directionVec1 = vertex - lastVertex;
		glm::vec2 directionVec2 = nextVertex - vertex;
		
		glm::vec3 up(0, 0, 1);

		glm::vec3 normal1 = glm::normalize(glm::cross(glm::vec3(directionVec1, 0), up));
		glm::vec3 normal2 = glm::normalize(glm::cross(glm::vec3(directionVec2, 0), up));

		glm::vec3 normal = normal1 + normal2;
		addBezierNormal(glm::normalize(glm::vec2(normal.x, normal.y)));
	}else if (index == 1){
		glm::vec2 vertex = m_bezierVertices[index - 1];
		glm::vec2 nextVertex = m_bezierVertices[index];

		glm::vec2 directionVec = nextVertex - vertex;

		glm::vec3 up(0, 0, 1);

		glm::vec3 normal = glm::cross(glm::vec3(directionVec, 0), up);

		addBezierNormal(glm::normalize(glm::vec2(normal.x, normal.y)));
	}

	
}

void Model::calculateNormalMapPixels(int blur){
	//if (m_normalMapPixels != nullptr)
		//delete m_normalMapPixels;
	int countNormal = 0;
	int countSides = 0;
	int countCorners = 0;
	int count = 0;
	//m_heightMapPixels->blur(nana::rectangle(m_heightMapPixels->size()), blur);
	m_normalMapPixels = new nana::paint::pixel_buffer(m_heightMapPixels->size().width, m_heightMapPixels->size().height);
	m_normalMapPixels->rectangle(nana::rectangle(m_normalMapPixels->size()), nana::color(0, 0, 255), 0, true);
	std::cout << "Normalmap Calculation..." << std::endl;
	for (int x = 0; x < m_heightMapPixels->size().width; ++x){
		std::cout << x << std::endl;
		for (int y = 0; y < m_heightMapPixels->size().height; ++y){
			glm::vec3 normal;
			if (x > 0 && x < (m_heightMapPixels->size().width - 1) && y > 0 && y < (m_heightMapPixels->size().height - 1)){
				nana::pixel_color_t pxColorLeft = m_heightMapPixels->pixel(x - 1, y);
				nana::pixel_color_t pxColorTop = m_heightMapPixels->pixel(x, y + 1);
				nana::pixel_color_t pxColorRight = m_heightMapPixels->pixel(x + 1, y);
				nana::pixel_color_t pxColorBottom = m_heightMapPixels->pixel(x, y - 1);

				float value = 32.f;
				float zLeft = (float(pxColorLeft.element.red + pxColorLeft.element.green + pxColorLeft.element.blue) / 3 / value);
				float zTop = (float(pxColorTop.element.red + pxColorTop.element.green + pxColorTop.element.blue) / 3 / value);
				float zRight = (float(pxColorRight.element.red) / value);
				float zBottom = (float(pxColorBottom.element.red) / value);

				glm::vec3 left(x - 1, y, zLeft);
				glm::vec3 top(x, y + 1, zTop);
				glm::vec3 right(x + 1, y, zRight);
				glm::vec3 bottom(x, y - 1, zBottom);

				glm::vec3 directionLR = right - left;
				glm::vec3 directionTB = bottom - top;

				normal = glm::cross(directionLR, directionTB);
				if (normal.z < 0)
					normal *= -1;

				normal = glm::normalize(normal);

				countNormal++;
			}
			else if ((x == 0 && y == 0) || (x == (m_heightMapPixels->size().width - 1) && y == (m_heightMapPixels->size().height - 1))){
				nana::pixel_color_t pxColor = m_heightMapPixels->pixel(x, y);
				nana::pixel_color_t pxColorLR;
				nana::pixel_color_t pxColorTB;

				if (x == 0 && y == 0){
					pxColorLR = m_heightMapPixels->pixel(x + 1, y);
					pxColorTB = m_heightMapPixels->pixel(x, y + 1);
				}
				else{
					pxColorLR = m_heightMapPixels->pixel(x - 1, y);
					pxColorTB = m_heightMapPixels->pixel(x, y - 1);
				}

				float z = (float(pxColor.element.red) / 255.f);
				float zLR = (float(pxColorLR.element.red) / 255.f);
				float zTB = (float(pxColorTB.element.red) / 255.f);

				glm::vec3 v(x, y, z);
				glm::vec3 lr(x - 1, y, zLR);
				glm::vec3 tb(x, y + 1, zTB);

				glm::vec3 directionLR = lr - v;
				glm::vec3 directionTB = tb - v;

				normal = glm::cross(directionLR, directionTB);
				if (normal.z < 0)
					normal.z *= -1;

				normal = glm::normalize(normal);


				countCorners++;
			}
			else if ((x == 0 && y == m_heightMapPixels->size().height - 1) || (x == (m_heightMapPixels->size().width - 1) && y == 0)){
				nana::pixel_color_t pxColor = m_heightMapPixels->pixel(x, y);
				nana::pixel_color_t pxColorLR;
				nana::pixel_color_t pxColorTB;

				if (x == 0 && y == m_heightMapPixels->size().height - 1){
					pxColorLR = m_heightMapPixels->pixel(x + 1, y);
					pxColorTB = m_heightMapPixels->pixel(x, y - 1);
				}
				else{
					pxColorLR = m_heightMapPixels->pixel(x - 1, y);
					pxColorTB = m_heightMapPixels->pixel(x, y + 1);
				}

				float z = (float(pxColor.element.red) / 255.f);
				float zLR = (float(pxColorLR.element.red) / 255.f);
				float zTB = (float(pxColorTB.element.red) / 255.f);

				glm::vec3 v(x, y, z);
				glm::vec3 lr(x - 1, y, zLR);
				glm::vec3 tb(x, y + 1, zTB);

				glm::vec3 directionLR = lr - v;
				glm::vec3 directionTB = tb - v;

				normal = glm::cross(directionLR, directionTB);
				if (normal.z < 0)
					normal.z *= -1;

				normal = glm::normalize(normal);
				countCorners++;
			}
			else if (x == 0 || x == (m_heightMapPixels->size().width - 1)){
				nana::pixel_color_t pxColor = m_heightMapPixels->pixel(x, y);
				nana::pixel_color_t pxColorLR;
				nana::pixel_color_t pxColorTop;
				nana::pixel_color_t pxColorBottom;

				if (x == 0){
					pxColorLR = m_heightMapPixels->pixel(x + 1, y);
					pxColorTop = m_heightMapPixels->pixel(x, y + 1);
					pxColorBottom = m_heightMapPixels->pixel(x, y - 1);
				}
				else{
					pxColorLR = m_heightMapPixels->pixel(x - 1, y);
					pxColorTop = m_heightMapPixels->pixel(x, y + 1);
					pxColorBottom = m_heightMapPixels->pixel(x, y - 1);
				}

				float z = (float(pxColor.element.red) / 255.f);
				float zLR = (float(pxColorLR.element.red) / 255.f);
				float zTop = (float(pxColorTop.element.red) / 255.f);
				float zBottom = (float(pxColorBottom.element.red) / 255.f);

				glm::vec3 v(x, y, z);
				glm::vec3 lr(x - 1, y, zLR);
				glm::vec3 top(x, y + 1, zTop);
				glm::vec3 bottom(x, y - 1, zBottom);

				glm::vec3 directionLR = lr - v;
				glm::vec3 directionTB = top - bottom;

				normal = glm::cross(directionLR, directionTB);
				if (normal.z < 0)
					normal.z *= -1;

				normal = glm::normalize(normal);

				countSides++;
			}
			else if (y == 0 || y == (m_heightMapPixels->size().height - 1)){
				nana::pixel_color_t pxColor = m_heightMapPixels->pixel(x, y);
				nana::pixel_color_t pxColorTB;
				nana::pixel_color_t pxColorLeft = m_heightMapPixels->pixel(x - 1, y);
				nana::pixel_color_t pxColorRight = m_heightMapPixels->pixel(x + 1, y);

				if (y == 0){
					pxColorTB = m_heightMapPixels->pixel(x, y + 1);
				}
				else{
					pxColorTB = m_heightMapPixels->pixel(x, y + 1);
				}

				float z = (float(pxColor.element.red) / 255.0f);
				float zLeft = (float(pxColorLeft.element.red) / 255.0f);
				float zRight = (float(pxColorRight.element.red) / 255.0f);
				float zTB = (float(pxColorTB.element.red) / 255.0f);

				glm::vec3 v(x, y, z);
				glm::vec3 left(x - 1, y, zLeft);
				glm::vec3 right(x, y + 1, zRight);
				glm::vec3 tb(x, y - 1, zTB);

				glm::vec3 directionLR = left - right;
				glm::vec3 directionTB = tb - v;

				normal = glm::cross(directionLR, directionTB);
				if (normal.z < 0)
					normal.z *= -1;

				normal = glm::normalize(normal);
				countSides++;
			}
			else{
				normal = glm::vec3(0, 0, 0);
			}

			count++;
			int r = (normal.x*0.5f + 0.5f) * 255;
			int g = (normal.y*0.5f + 0.5f) * 255;
			int b = (normal.z*0.5f + 0.5f) * 255;
			m_normalMapPixels->pixel(x, y, nana::color(r, g, b).px_color());
		}
	}
	m_normalMapPixels->blur(nana::rectangle(m_normalMapPixels->size()), blur);
}

