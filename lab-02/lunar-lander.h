#include <GL/glew.h>
#include <GL/glut.h>
#include "utils.h"
#include <cstdlib>
#include <vector>


/**************RENDERING***************/
static unsigned int programId, programId_1;

unsigned int VAO, VAO_STAR, VAO_SKY, VAO_MOON, VAO_FLAG, VAO_SPARKS, VAO_TANK;
unsigned int VBO, VBO_S, VBO_SKY, VBO_M, VBO_F, VBO_SPARKS, VBO_TANK, loc, MatProj, MatModel, MatProj1, MatModel1;
GLuint color;
// Viewport size
int width = 1280;
int height = 720;
glm::mat4 Projection;  //Matrice di proiezione
glm::mat4 Model; //Matrice per il cambiamento di sistema di riferimento: da Sistema diriferimento dell'oggetto a sistema di riferimento nel Mondo

int nVertices_star = 3 * nTriangles;
math_utils::Point* Stars = new math_utils::Point[nVertices_star];
int nVertices_moon = 3 * nTriangles;
math_utils::Point* Moon = new math_utils::Point[nVertices_moon];
int nVertices_flag = 5;
math_utils::Point* Flag = new math_utils::Point[nVertices_flag];
int vertices_sky = 6;
math_utils::Point* Sky = new math_utils::Point[vertices_sky];
int nVertices_ship_legs = 4;
int nVertices_ship_top = 3 * 7;
int nVertices_ship = 6 + nVertices_ship_top + nVertices_ship_legs;
math_utils::Point* Ship = new math_utils::Point[nVertices_ship];
int nPoint = 6;
math_utils::Point* Sparks = new math_utils::Point[nPoint];
int nTank = 1000;
math_utils::Point* Tank = new math_utils::Point[nTank];

/**************GAME***************/

#define RESET 100

objects::Stars stars = objects::Stars();
objects::Ship ship = objects::Ship(width, height);

bool game_end = false, game_won = false;
int reset_timeout = RESET;

float flagX = width*3.0f/5.0f, flagY = height * 0.2f;

void restart(int i);
void update(int a);