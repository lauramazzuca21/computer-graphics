#include <stdio.h>
#include <math.h>
#include <vector>
#include <string>
#include <ctime>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "HUD_Logger.h"
#include "ShaderMaker.h"

#define SHIFT_WHEEL_UP 11
#define SHIFT_WHEEL_DOWN 12
#define CTRL_WHEEL_UP 19
#define CTRL_WHEEL_DOWN 20

#define NUM_SHADERS 9

using namespace std;

// Viewport size
static int WindowWidth = 1120; //1366;
static int WindowHeight = 630; // 768;
GLfloat aspect_ratio = 16.0f / 9.0f;

typedef struct {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;
	GLuint vertexArrayObjID;
	GLuint vertexBufferObjID;
	GLuint normalBufferObjID;
	GLuint uvBufferObjID;
} Mesh;

typedef enum {
	RED_PLASTIC,
	EMERALD,
	BRASS,
	SLATE,
	NO_MATERIAL
} MaterialType;

typedef struct {
	std::string name;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;
} Material;

constexpr float WAVE_FREQUENCY = 0.00001f;
constexpr float WAVE_HEIGHT_SCALE = 0.1f;

typedef enum { // used also as index, don't modify order
	GOURAUD,
	PHONG,
	BLINN,
	TOON,
	PASS_THROUGH,
	WAVE
} ShadingType;

typedef struct {
	Mesh mesh;
	MaterialType material;
	ShadingType shading;
	glm::mat4 M;
	string name;
} Object;

typedef struct {
	GLuint light_position_pointer;
	GLuint light_color_pointer;
	GLuint light_power_pointer;
	GLuint material_diffuse;
	GLuint material_ambient;
	GLuint material_specular;
	GLuint material_shininess;
} LightShaderUniform;

typedef struct {
	GLuint P_Matrix_pointer;
	GLuint V_Matrix_pointer;
	GLuint M_Matrix_pointer;
	GLuint MVP_Matrix_Pointer;
	GLfloat time_delta_pointer;
	GLfloat frequency_pointer;
	GLfloat height_scale_pointer;
} BaseShaderUniform;

const string MeshDir = "Mesh/";
const string ShaderDir = "Shaders/";
static Object Axis, Grid;
static vector<Object> objects;
static vector<Material> materials;
static int selected_obj = 0;

// Materiali disponibili
glm::vec3 red_plastic_ambient = { 0.1, 0.0, 0.0 }, red_plastic_diffuse = { 0.6, 0.1, 0.1 }, red_plastic_specular = { 0.7, 0.6, 0.6 }; GLfloat red_plastic_shininess = 32.0f;
glm::vec3 brass_ambient = { 0.1, 0.06, 0.015 }, brass_diffuse = { 0.78, 0.57, 0.11 }, brass_specular = { 0.99, 0.91, 0.81 }; GLfloat brass_shininess = 27.8f;
glm::vec3 emerald_ambient = { 0.0215, 0.04745, 0.0215 }, emerald_diffuse = { 0.07568, 0.61424, 0.07568 }, emerald_specular = { 0.633, 0.727811, 0.633 }; GLfloat emerald_shininess = 78.8f;
glm::vec3 slate_ambient = { 0.02, 0.02, 0.02 }, slate_diffuse = { 0.1, 0.1, 0.1 }, slate_specular{ 0.4, 0.4, 0.4 }; GLfloat slate_shininess = 1.78125f;

typedef struct {
	glm::vec3 position;
	glm::vec3 color;
	GLfloat power;
} point_light;

static point_light light;

/*camera structures*/
constexpr float CAMERA_ZOOM_SPEED = 0.1f;
constexpr float CAMERA_TRASLATION_SPEED_V = 0.01f;
constexpr float CAMERA_TRASLATION_SPEED_H = 0.1f;

struct {
	glm::vec4 position = glm::vec4(10.0, 10.0, 10.0, 0.0);
	glm::vec4 target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	glm::vec4 upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);
	glm::vec4 rightVector = glm::vec4(1.0, 0.0, 0.0, 0.0);
} ViewSetup;

struct {
	float fovY, aspect, near_plane, far_plane;
} PerspectiveSetup;

typedef enum {
	WIRE_FRAME,
	FACE_FILL,
	CULLING_ON,
	CULLING_OFF,
	CHANGE_TO_WCS,
	CHANGE_TO_OCS
} MenuOption;

enum {
	NAVIGATION,
	CAMERA_MOVING,
	TRASLATING,
	ROTATING,
	SCALING
} OperationMode;

enum {
	X,
	Y,
	Z
} WorkingAxis;

enum {
	OCS, // Object Coordinate System
	WCS // World Coordinate System
} TransformMode;

static bool moving_trackball = 0;
static int last_mouse_pos_Y;
static int last_mouse_pos_X;

//Shaders Uniforms 
static vector<LightShaderUniform> light_uniforms; // for shaders with light
static vector<BaseShaderUniform> base_uniforms; // for ALL shaders
static vector<GLuint> shaders_IDs; //Pointers to the shader programs
// Main initialization funtion
void init();
// Display Funtion
void display();
// Reshape Function
void resize(int w, int h);
// Calls glutPostRedisplay each millis milliseconds
void refresh_monitor(int millis);
// Mouse Function
void mouseClick(int button, int state, int x, int y);
// Keyboard:  g traslate r rotate s scale x,y,z axis esc 
void keyboardDown(unsigned char key, int x, int y);
// Special key arrow: select active object (arrows left,right)
void special(int key, int x, int y);
// gestione delle voci principali del menu
void main_menu_func(int option);
// gestione delle voci principali del sub menu per i matriali
void material_menu_function(int option);
// costruisce i menu openGL
void buildOpenGLMenu();
// Trackball: Converte un punto 2D sullo schermo in un punto 3D sulla trackball
glm::vec3 getTrackBallPoint(float x, float y);
// Trackball: Effettua la rotazione del vettore posizione sulla trackball
void mouseActiveMotion(int x, int y);
void moveCameraForeward();
void moveCameraBack();
void moveCameraLeft();
void moveCameraRight();
void moveCameraUp();
void moveCameraDown();
//	Crea ed applica la matrice di trasformazione alla matrice dell'oggeto discriminando tra WCS e OCS.
//	La funzione � gia invocata con un input corretto, � sufficiente concludere la sua implementazione.
void modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor);
/* Mesh Functions*/
// Genera i buffer per la mesh in input e ne salva i puntatori di openGL
void generate_and_load_buffers(bool generate, Mesh* mesh);
// legge un file obj ed inizializza i vector della mesh in input
void loadObjFile(string file_path, Mesh* mesh);
// disegna l'origine del assi
void drawAxisAndGrid();
// 2D fixed pipeline Font rendering on screen
void printToScreen();