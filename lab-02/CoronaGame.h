// Include GLM; libreria matematica per le opengl
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "utils.h"

static unsigned int programId, programId_1;

unsigned int VAO, VAO_CIELO, VAO_NEMICO;
unsigned int VBO, VBO_C, VBO_N, loc, MatProj, MatModel, MatProj1, MatModel1;

int NumeroColpiti = 0;
glm::mat4 Projection;  //Matrice di proiezione
glm::mat4 Model; //Matrice per il cambiamento di sistema di riferimento: da Sistema diriferimento dell'oggetto a sistema di riferimento nel Mondo

float dxnemici = 0;
float dynemici = 0;
float posxN, posyN;
int nemici_per_riga = 5;
int numero_di_righe = 3;

int nVertices_Navicella = 12 * nTriangles + 1;

int nvBocca = 5;
int nvTentacoli = 16;
int nvSpikes = 8 * 3 * nTriangles; // adds vertexes of spikes
int nVertices_Nemico = 3 * nTriangles + 2* 3 * nTriangles + nvBocca + nvTentacoli + nvSpikes;

math_utils::Point* Navicella = new math_utils::Point[nVertices_Navicella];
math_utils::Point* Nemico = new math_utils::Point[nVertices_Nemico];

int vertices_cielo = 6;
math_utils::Point* Cielo = new math_utils::Point[vertices_cielo];

// Viewport size
int width = 1280;
int height = 720;

float t;
float posx_Proiettile = 0, posy_Proiettile = 0;

//ANIMARE V
double VelocitaOrizzontale = 0; //velocita orizzontale (pixel per frame)
int scuoti = 0;
double accelerazione = 1; // forza di accelerazione data dalla tastiera
float posx = width / 2; //coordinate sul piano della posizione iniziale della navicella
float posy = height * 0.2;
float posizione_di_equilibrio = posy;
float angolo = 0;

bool pressing_left = false;
bool pressing_right = false;
bool pressing_attack = false;
bool pressing_rotate_s = false;
bool pressing_rotate_d = false;

bool** colpito;
float angoloV = 0;
double range_fluttuazione = 15; // fluttuazione su-gi 
double angle = 0; // angolo di fluttuazione
double angle_offset = 10; // quanto   accentuata l'oscillazione angolare
double float_yoffset = 0; // distacco dalla posizione d'equilibrio 
int frame_animazione = 0; // usato per animare la fluttuazione
int frame = 0;
bool start = 0;

