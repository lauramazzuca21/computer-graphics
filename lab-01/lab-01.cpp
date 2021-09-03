/*
 * Lab-01_students.c
 *
 *     This program draws straight lines connecting dots placed with mouse clicks.
 *
 * Usage:
 *   Left click to place a control point.
 *		Maximum number of control points allowed is currently set at 64.
 *	 Press "f" to remove the first control point
 *	 Press "l" to remove the last control point.
 *	 Press escape to exit.
 */


#include <iostream>
#include "ShaderMaker.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <utility>
#include <numeric>

 // Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

static unsigned int programId;

unsigned int VAO;
unsigned int VBO;

unsigned int VAO_curve;
unsigned int VBO_curve;

using namespace glm;

#define DE_CASTELJAU 			0
#define ADAPTIVE_SUBDIVISION	1 			

#define PointRadius		8.0
#define SegmentRadius	2.5
#define CurveRadius		1.0
#define CurvePointRadius		2.0
#define Tolerance		0.00075f

#define MaxDeg 			2
#define MaxNumCurvePts 	1000 //increased from 100 to 1000 to support chosen tolerance's production of curve points
#define MaxNumPts 		64
float PointArray[MaxNumPts][3];
float CurvePointArray[MaxNumCurvePts][3];
int NumPts = 0;
int CurveNumPts = 0;
int SelectedCP = -1;
int method = 	1;
// Window size in pixels
int		width = 500;
int		height = 500;

/* Prototypes */
void addNewPoint(float x, float y);
void removeFirstPoint();
void removeLastPoint();


void myKeyboardFunc(unsigned char key, int x, int y)
{
	switch (key) {
	case 'f':
		removeFirstPoint();
		glutPostRedisplay();
		break;
	case 'l':
		removeLastPoint();
		glutPostRedisplay();
		break;
	case 27:			// Escape key
		exit(0);
		break;
	}
}

void removeFirstPoint() {
	int i;
	if (NumPts > 0) {
		// Remove the first point, slide the rest down
		NumPts--;
		for (i = 0; i < NumPts; i++) {
			PointArray[i][0] = PointArray[i + 1][0];
			PointArray[i][1] = PointArray[i + 1][1];
			PointArray[i][2] = 0;
		}
	}
}
void resizeWindow(int w, int h)
{
	height = (h > 1) ? h : 2;
	width = (w > 1) ? w : 2;
	gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

// (x,y) viewport(0,width)x(0,height)   -->   (xPos,yPos) window(-1,1)x(-1,1)
void viewportToWindow(int x, int y, float *xPos, float *yPos)
{
	(*xPos) = -1.0f + ((float)x)*2 / ((float)(width));
	(*yPos) = -1.0f + ((float)(height - y))*2 / ((float)(height));
}

int isControlPoint(float xPos, float yPos)
{
	float dx = PointRadius/(width*1.0f);
	float dy = PointRadius/(height*1.0f);

	for (int i = 0; i < NumPts; i++)
	{
		if (xPos <= (PointArray[i][0] + dx)  && xPos >= (PointArray[i][0] - dx)
			&& yPos <= (PointArray[i][1] + dy) && yPos >= (PointArray[i][1] - dy) )
			{
				return i;
			} 
	}
	return -1;
}

// Left button pressed -> place a new control point.
void myMouseFunc(int button, int state, int x, int y) {
	
	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
	{
		SelectedCP = -1;
		return;
	}

	float xPos, yPos;
	viewportToWindow(x, y, &xPos, &yPos);	
	printf("pressed point Xpos %f Ypos %f \n", xPos, yPos);
	printf("pressed point in pixel x %d y %d \n", x,y);

	SelectedCP = isControlPoint(xPos, yPos);
	if (SelectedCP < 0)	addNewPoint(xPos, yPos);

	glutPostRedisplay();
}

//left click e tengo premuto per muovere il control point
void myMotionFunc(int x, int y) {
	if (SelectedCP < 0)
		return;
	
	float xPos, yPos ;
	
	viewportToWindow(x, y, &xPos, &yPos);	
	
	PointArray[SelectedCP][0] = xPos;
	PointArray[SelectedCP][1] = yPos;

	glutPostRedisplay();
}

// Add a new point to the end of the list.  
// Remove the first point in the list if too many points.
void removeLastPoint() {
	if (NumPts > 0) {
		NumPts--;
	}
}

// Add a new point to the end of the list.  
// Remove the first point in the list if too many points.
void addNewPoint(float x, float y) {
	if (NumPts >= MaxNumPts) {
		removeFirstPoint();
	}
	PointArray[NumPts][0] = x;
	PointArray[NumPts][1] = y;
	PointArray[NumPts][2] = 0;
	NumPts++;
}

float lerp(float v0, float v1, float t) {
  return (1 - t) * v0 + t * v1;
}

glm::vec3 sub(glm::vec3 a, glm::vec3 b)
{
	glm::vec3 result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	result.z = a.z - b.z;
	return result;
}

float magnitude(glm::vec3 a)
{
	return sqrt(powf(a.x, 2) + powf(a.y, 2) + powf(a.z, 2));
}

float distPointLine(glm::vec3 point, glm::vec3 lineStart, glm::vec3 lineEnd)
{
	glm:: vec3 SE, EP;
	float area;
	SE = sub(lineEnd, lineStart);
	EP = sub(point, lineStart);
	area = magnitude(glm::cross(SE, EP));

	return fabsf(area / glm::distance(lineEnd, lineStart));
}

void deCasteljau(float t, float inArray[MaxNumPts][3], float outArray[3], int size) {

	float TempPointArray[MaxNumPts][3];

	for (int i = 0; i <= size; i++)
	{
		TempPointArray[i][0] = inArray[i][0];
		TempPointArray[i][1] = inArray[i][1];
		TempPointArray[i][2] = inArray[i][2];
	}

	for (int deg = 1; deg < size; deg++)
	{
		for (int i = 0; i < size - deg; i++)
		{
			TempPointArray[i][0] = lerp (TempPointArray[i][0], TempPointArray[i+1][0], t);
			TempPointArray[i][1] = lerp (TempPointArray[i][1], TempPointArray[i+1][1], t); 
			TempPointArray[i][2] = 0.0f;
		}
	}
		
	outArray[0] = TempPointArray[0][0];
	outArray[1] = TempPointArray[0][1];
	outArray[2] = TempPointArray[0][2];
} 

void deCasteljauASCP(float t, float inArray[MaxNumPts][3], float outArray1[MaxNumPts][3], float outArray2[MaxNumPts][3], int size) {

	float TempPointArray[MaxNumPts][3];

	for (int i = 0; i <= size; i++)
	{
		TempPointArray[i][0] = inArray[i][0];
		TempPointArray[i][1] = inArray[i][1];
		TempPointArray[i][2] = inArray[i][2];
	}

	//copy the first point in outArray containing first half of the new CPs
	outArray1[0][0] = TempPointArray[0][0];
	outArray1[0][1] = TempPointArray[0][1];
	outArray1[0][2] = TempPointArray[0][2];
	//copy the last point in outArray containing second half of the new CPs
	outArray2[size -1][0] = TempPointArray[size -1][0];
	outArray2[size -1][1] = TempPointArray[size -1][1];
	outArray2[size -1][2] = TempPointArray[size -1][2];


	for (int deg = 1; deg < size; deg++)
	{
		for (int i = 0; i < size - deg; i++)
		{
			TempPointArray[i][0] = lerp (TempPointArray[i][0], TempPointArray[i+1][0], t);
			TempPointArray[i][1] = lerp (TempPointArray[i][1], TempPointArray[i+1][1], t); 
			TempPointArray[i][2] = 0.0f;
		}
		//copy the intermediate CP results of de Casteljau algorithm to get the first half new CPs
		//the ones that go into the first outArray increase in index with deg (current index of degree analyzed)
		outArray1[deg][0] = TempPointArray[0][0];
		outArray1[deg][1] = TempPointArray[0][1];
		outArray1[deg][2] = TempPointArray[0][2];
		
		//the ones that go into the second outArray decrese in index with deg (current index of degree analyzed)
		outArray2[size-deg-1][0] = TempPointArray[size-deg-1][0];
		outArray2[size-deg-1][1] = TempPointArray[size-deg-1][1];
		outArray2[size-deg-1][2] = TempPointArray[size-deg-1][2];
	}
} 

bool flatTest( float tempArray[MaxNumPts][3], int size) {

	glm::vec3 start, end;
	glm:: vec3 currPoint;
	float pointLineDistance;

	start.x = tempArray[0][0];
	start.y = tempArray[0][1];
	start.z = tempArray[0][2];

	end.x = tempArray[size-1][0];
	end.y = tempArray[size-1][1];
	end.z = tempArray[size-1][2];

	for (int i = 1; i < size-1; i++)
	{
		currPoint.x = tempArray[i][0];
		currPoint.y = tempArray[i][1];
		currPoint.z = tempArray[i][2];

		pointLineDistance = distPointLine(currPoint, start, end);

		if (pointLineDistance > Tolerance)
			return false;
	}

	return true;
}

void adaptiveSubdivision(float tempArray[MaxNumPts][3], int size) {

	if (flatTest(tempArray, size))
	{
		CurvePointArray[CurveNumPts][0] = tempArray[0][0];
		CurvePointArray[CurveNumPts][1] = tempArray[0][1];
		CurvePointArray[CurveNumPts][2] = tempArray[0][2];
		CurveNumPts++;	
		CurvePointArray[CurveNumPts][0] = tempArray[size-1][0];
		CurvePointArray[CurveNumPts][1] = tempArray[size-1][1];
		CurvePointArray[CurveNumPts][2] = tempArray[size-1][2];
		CurveNumPts++;	
	}
	else
	{
		float CP1[MaxNumPts][3], CP2[MaxNumPts][3], t;

//apply deCasteljau in t=0.5 and save new split CPs
		deCasteljauASCP(0.5f, tempArray, CP1, CP2, size);

		adaptiveSubdivision(CP1, size);
		adaptiveSubdivision(CP2, size);
	}
}

void generateBezierPoints() {
	CurveNumPts = 0;
	switch(method) {
		case DE_CASTELJAU:
		{
			printf("Using DE_CASTELJAU\n");
			float t;
			for (int j = 0; j < MaxNumCurvePts; j++)
			{
				t = (GLfloat)j/MaxNumCurvePts;
				deCasteljau(t, PointArray, CurvePointArray[j], NumPts);
			}
		}
		break;
		case ADAPTIVE_SUBDIVISION:
		{	
			printf("Using ADAPTIVE_SUBDIVISION\n");
			adaptiveSubdivision(PointArray, NumPts); 
		}
			break;
		default:
		break;
	}

}

void initShader(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader.glsl";
	char* fragmentShader = (char*)"fragmentShader.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

}

void init(void)
{
	//control points
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//bezier curve
	glGenVertexArrays(1, &VAO_curve);
	glBindVertexArray(VAO_curve);
	glGenBuffers(1, &VBO_curve);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_curve);

	// Background color
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glViewport(0, 0, width, height);

}

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(PointArray), &PointArray[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	// Draw the curve points -- used as double check
	// glPointSize(PointRadius); //in pixels
	// glDrawArrays(GL_POINTS, 0, NumPts);

	// Draw the segments
	glLineWidth(SegmentRadius); //in pixels
	glDrawArrays(GL_LINE_STRIP, 0, NumPts);

	//unbind first VAO
	glBindVertexArray(0);

	//Draw bezier curve
	if (NumPts > 1) {
		generateBezierPoints();
	}

	glBindVertexArray(VAO_curve);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_curve);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CurvePointArray), &CurvePointArray[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

		// Draw the line points
	glPointSize(CurvePointRadius); //in pixels
	glDrawArrays(GL_POINTS, 0, CurveNumPts);

	glLineWidth(CurveRadius);
	glDrawArrays(GL_LINE_STRIP, 0, CurveNumPts);
	glBindVertexArray(0);

	glutSwapBuffers();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Draw curves 2D");
	
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resizeWindow);
	glutKeyboardFunc(myKeyboardFunc);
	glutMouseFunc(myMouseFunc);
	glutMotionFunc(myMotionFunc);


	glewExperimental = GL_TRUE;
	glewInit();

	initShader();
	init();

	glutMainLoop();
}
