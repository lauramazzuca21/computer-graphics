
// Include GLM; libreria matematica per le opengl
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "lunar-lander.h"
#include "ShaderMaker.h"

void reset() {
    ship = objects::Ship(width, height);
    game_end = false;
    game_won = false;
    reset_timeout = RESET;
}

void restart(int i) {
    reset_timeout-=i;
    if (reset_timeout <= 0)
    {
        reset();
        glutPostRedisplay();
        glutTimerFunc(5, update, 0);
    }
    else {
        float timeValue = glutGet(GLUT_ELAPSED_TIME);
        float t = abs(sin(timeValue));
        glUniform1f(loc, t);
        glutPostRedisplay();
        glutTimerFunc(5, restart, 5);
    }
}

void update(int a)
{
    if (game_end)
	{
        auto c = game_won ? colors::maximum_green : colors::red;
        glClearColor(c.r, c.g, c.b, 1.0);
        glutPostRedisplay();
        glutTimerFunc(5, restart, 5);
		return;
	}

    ship.update();
    ship.sparks.update();

	if (ship.posy <= flagY) {
        game_end = true;
        if (ship.posx < flagX && ship.posx > width - flagX  
        && ship.vely > -1.0f && ship.angle <= PI/4.0f && ship.angle >= -PI/4.0f)
            game_won = true;
        printf("game %d ship vely %f angle %f\n", game_won ? 1 : 0, ship.vely, ship.angle);
    }
    glutPostRedisplay();
	glutTimerFunc(5, update, 0);
}

void drawScene(void)
{
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(game_end ? programId_1 : programId);  // attiva fragment shader basic per tutta la scena meno i nemici 

    // Disegna cielo
	glBindVertexArray(VAO_SKY);
	Model = glm::mat4(1.0);
	Model = glm::scale(Model, glm::vec3(float(width), float(height), 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glDrawArrays(GL_TRIANGLES, 0, vertices_sky);
	glBindVertexArray(0);

	stars.update(width, height);
	
	glBindVertexArray(VAO_STAR);
	for (int i = 0; i < stars.nStars; i++)
	{
		Model = glm::mat4(1.0);
		Model = glm::translate(Model, glm::vec3(stars.pos_stars.at(i).x, stars.pos_stars.at(i).y, 1.0));
		Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, 1.0));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		glDrawArrays(GL_TRIANGLES, 0, nVertices_star);
	}
	glBindVertexArray(0);

	glBindVertexArray(VAO_MOON);
	Model = glm::mat4(1.0);
	Model = glm::scale(Model, glm::vec3(float(width), float(height), 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glDrawArrays(GL_TRIANGLES, 0, nVertices_moon);
	glBindVertexArray(0);

    glBindVertexArray(VAO_FLAG);
	Model = glm::mat4(1.0);
	Model = glm::translate(Model, glm::vec3(flagX, flagY, 0.0));
	Model = scale(Model, glm::vec3(15.0, 15.0, 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glLineWidth(8.0);  // legs
	glDrawArrays(GL_LINES, 0, 2); //stick
	glDrawArrays(GL_TRIANGLES, 2, 3); //flag
    Model = glm::mat4(1.0);
	Model = glm::translate(Model, glm::vec3(float(width) - flagX, flagY, 0.0));
	Model = scale(Model, glm::vec3(15.0, 15.0, 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glLineWidth(8.0);  // legs
	glDrawArrays(GL_LINES, 0, 2); //stick
	glDrawArrays(GL_TRIANGLES, 2, 3); //flag

	glBindVertexArray(0);

    glBindVertexArray(VAO_SPARKS);
    int cont = 0;
	for (const auto& p : ship.sparks.particles)
	{
		Model = glm::mat4(1.0);
		Model = glm::translate(Model, glm::vec3(p.p.x, p.p.y, 1.0));
		Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, 1.0));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
        glPointSize(3.0); 
        glDrawArrays(GL_POINTS, rand()%6, 1);
	}
	glBindVertexArray(0);

	//Disegno Navicella
	glBindVertexArray(VAO);
	Model = glm::mat4(1.0);
	Model = glm::translate(Model, glm::vec3(ship.posx, ship.posy, 0.0));
	Model = scale(Model, glm::vec3(15.0, 15.0, 1.0));
	Model = glm::rotate(Model, ship.angle, glm::vec3(0.0, 0.0, 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glDrawArrays(GL_TRIANGLES, 0, nVertices_ship - nVertices_ship_legs);
	glLineWidth(8.0);  // legs
	glDrawArrays(GL_LINES, nVertices_ship - nVertices_ship_legs, nVertices_ship_legs);

	glBindVertexArray(0);

    glBindVertexArray(VAO_TANK);
    Model = glm::mat4(1.0);
    Model = glm::translate(Model, glm::vec3(width*0.8f, height*0.9f, 1.0));
    Model = glm::scale(Model, glm::vec3(250.0f, 250.0f, 1.0));
    glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glLineWidth(30.0);
    int points = std::floor((ship.fuel_full/ship.thrust)*ship.fuel);
	glDrawArrays(GL_LINE_STRIP, 0, points);
	glBindVertexArray(0);

	glutSwapBuffers();
}

void draw_stars(math_utils::Point* stars) {
	int cont=0;
	math_utils::Point* tmp;
	tmp = new math_utils::Point[nVertices_star];
	draw_utils::disegna_cerchio(0.0f, 0.0f, 1.0f, 1.0f, colors::white, colors::white, tmp);
	for (int j = 0; j < 3 * nTriangles; j++)
	{
		stars[cont].x = tmp[j].x;
		stars[cont].y = tmp[j].y;
		stars[cont].z = tmp[j].z;
		stars[cont].r = tmp[j].r;stars[cont].g = tmp[j].g;stars[cont].b = tmp[j].b;stars[cont].a = tmp[j].a;
		cont++;
	}
}

void draw_spark(math_utils::Point* spark) {
    spark[0].x = 0.0f;
    spark[0].y = 0.0f;
    spark[0].z = 0.0f;
    spark[0].r = colors::red.r;spark[0].g = colors::red.g;spark[0].b = colors::red.b;spark[0].a = colors::red.a;
    spark[1].x = 0.0f;
    spark[1].y = 0.0f;
    spark[1].z = 0.0f;
    spark[1].r = colors::orange.r;spark[1].g = colors::orange.g;spark[1].b = colors::orange.b;spark[1].a = colors::orange.a;
    spark[2].x = 0.0f;
    spark[2].y = 0.0f;
    spark[2].z = 0.0f;
    spark[2].r = colors::red.r;spark[2].g = colors::red.g;spark[2].b = colors::red.b;spark[2].a = colors::red.a;
    spark[3].x = 0.0f;
    spark[3].y = 0.0f;
    spark[3].z = 0.0f;
    spark[3].r = colors::vivid_orange.r;spark[3].g = colors::vivid_orange.g;spark[3].b = colors::vivid_orange.b;spark[3].a = colors::vivid_orange.a;
    spark[4].x = 0.0f;
    spark[4].y = 0.0f;
    spark[4].z = 0.0f;
    spark[4].r = colors::lemon.r;spark[4].g = colors::lemon.g;spark[4].b = colors::lemon.b;spark[4].a = colors::lemon.a;
    spark[5].x = 0.0f;
    spark[5].y = 0.0f;
    spark[5].z = 0.0f;
    spark[5].r = colors::blue_flame.r;spark[5].g = colors::blue_flame.g;spark[5].b = colors::blue_flame.b;spark[5].a = colors::blue_flame.a;
}

void draw_moon(math_utils::Point* stars) {
	int cont=0;
	math_utils::Point* tmp;
	tmp = new math_utils::Point[nVertices_star];
	draw_utils::disegna_cerchio(0.5f, -0.5f, 1.0f, 1.0f, colors::light_gray, colors::white, tmp);
	for (int j = 0; j < 3 * nTriangles; j++)
	{
		stars[cont].x = tmp[j].x;
		stars[cont].y = tmp[j].y;
		stars[cont].z = tmp[j].z;
		stars[cont].r = tmp[j].r;stars[cont].g = tmp[j].g;stars[cont].b = tmp[j].b;stars[cont].a = tmp[j].a;
		cont++;
	}
}

void draw_flag(math_utils::Point* flag) {
	int cont=0;
    //stick
	math_utils::Point PStart, PEnd;
    PStart.x = 0.0f;   
    PStart.y = 2.0f;
    PStart.z = 0.0f;
    PStart.r = colors::lincoln_green.r; PStart.g = colors::lincoln_green.g; PStart.b = colors::lincoln_green.b; PStart.a = colors::lincoln_green.a;
    PEnd.x = 0.0f;
    PEnd.y = -2.0f;
    PEnd.z = 0.0f;
    PEnd.r = colors::darkest_gray.r; PEnd.g = colors::darkest_gray.g; PEnd.b = colors::darkest_gray.b; PEnd.a = colors::darkest_gray.a;
    flag[cont] = PStart;
    flag[cont + 1] = PEnd;
    cont += 2;
    //flag
    flag[cont].x = 0.0f;	flag[cont].y = 0.0f; flag[cont].z = 0.0f;
	flag[cont].r = colors::lincoln_green.r; flag[cont].g = colors::lincoln_green.g; flag[cont].b = colors::lincoln_green.b; flag[cont].a = colors::lincoln_green.a;
	flag[cont+1].x = 0.0f;	flag[cont+1].y = 2.0f;	flag[cont+1].z = 0.0f;
	flag[cont+1].r = colors::lincoln_green.r; flag[cont+1].g = colors::lincoln_green.g; flag[cont+1].b = colors::lincoln_green.b; flag[cont+1].a = colors::lincoln_green.a;
	flag[cont+2].x = 3.0f;	flag[cont+2].y = 1.0f; flag[cont+2].z = 0.0f;
	flag[cont+2].r = colors::maximum_green.r; flag[cont+2].g = colors::maximum_green.g; flag[cont+2].b = colors::maximum_green.b; flag[cont+2].a = colors::maximum_green.a;

}

int draw_fuel_tank(math_utils::Point* fuel) {
	int cont=0;
    for (float i = 0; i < ship.fuel_full; i+=0.015f) {
        fuel[cont].x = i;   
        fuel[cont].y = 0.0f;
        fuel[cont].z = 0.0f;
        fuel[cont].r = colors::orange.r; fuel[cont].g = colors::orange.g; fuel[cont].b = colors::orange.b; fuel[cont].a = colors::orange.a;
        cont++;
    }
    return cont;
}

void draw_ship(math_utils::Point* ship)
{
	int cont = 0, i;
	math_utils::Point* circle, plane[6];
	int v_circle =3 * nTriangles;
	circle = new math_utils::Point[v_circle];
    
    //low-poli circle
	draw_utils::disegna_cerchio(0.0f, 2.0f, 2.0f, 2.0f, colors::white, colors::dark_gray, circle, 7);
    for (i = 0; i < nVertices_ship_top; i++)
	{
		ship[cont] = circle[i];
        cont++;
	}
	//copper rectangle
    draw_utils::disegna_piano(-2.5f, -1.25f, 5.0f, 2.5f, colors::dark_gray, colors::copper, plane);
    for (i = 0; i < 6; i++)
	{
		ship[cont] = plane[i];
        cont++;
	}

    //legs
    math_utils::Point PStart, PEnd;
    PStart.x = -2.4f;   
    PStart.y = -1.25f;
    PStart.z = 0.0;
    PStart.r = colors::copper.r; PStart.g = colors::copper.g; PStart.b = colors::copper.b; PStart.a = colors::copper.a;
    PEnd.x = -4.0f;
    PEnd.y = -3.05f;
    PEnd.z = 0.0;
    PEnd.r = colors::dark_gray.r; PEnd.g = colors::dark_gray.g; PEnd.b = colors::dark_gray.b; PEnd.a = colors::dark_gray.a;
    ship[cont] = PStart;
    ship[cont + 1] = PEnd;
    cont += 2;
    PStart.x = 2.4f;   
    PStart.y = -1.25f;
    PStart.z = 0.0;
    PStart.r = colors::copper.r; PStart.g = colors::copper.g; PStart.b = colors::copper.b; PStart.a = colors::copper.a;
    PEnd.x = 4.0f;
    PEnd.y = -3.05f;
    PEnd.z = 0.0;
    PEnd.r = colors::dark_gray.r; PEnd.g = colors::dark_gray.g; PEnd.b = colors::dark_gray.b; PEnd.a = colors::dark_gray.a;
    ship[cont] = PStart;
    ship[cont + 1] = PEnd;
    cont += 2;
}

void init(void)
{
	//Disegno SPAZIO/CIELO
	draw_utils::disegna_piano(0.0, 0.0, 1.0, 1.0f, colors::midnight_blue, colors::black, Sky);
	//Genero un VAO
	glGenVertexArrays(1, &VAO_SKY);
	//Ne faccio il bind (lo collego, lo attivo)
	glBindVertexArray(VAO_SKY);
	//AL suo interno genero un VBO
	glGenBuffers(1, &VBO_SKY);
	//Ne faccio il bind (lo collego, lo attivo, assegnandogli il tipo GL_ARRAY_BUFFER)
	glBindBuffer(GL_ARRAY_BUFFER, VBO_SKY);
	//Carico i dati vertices sulla GPU
	glBufferData(GL_ARRAY_BUFFER, vertices_sky * sizeof(math_utils::Point), &Sky[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	
	//draw stars
	//Costruzione geometria e colori delle stars
	//Genero il VAO delle STARS
    draw_stars(Stars);
	glGenVertexArrays(1, &VAO_STAR);
	glBindVertexArray(VAO_STAR);
	glGenBuffers(1, &VBO_S);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_S);
	glBufferData(GL_ARRAY_BUFFER, nVertices_star * sizeof(math_utils::Point), &Stars[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Scollego il VAO
	glBindVertexArray(0);

    draw_moon(Moon);
	glGenVertexArrays(1, &VAO_MOON);
	glBindVertexArray(VAO_MOON);
	glGenBuffers(1, &VBO_M);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_M);
	glBufferData(GL_ARRAY_BUFFER, nVertices_moon * sizeof(math_utils::Point), &Moon[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Scollego il VAO
	glBindVertexArray(0);

    draw_flag(Flag);
	glGenVertexArrays(1, &VAO_FLAG);
	glBindVertexArray(VAO_FLAG);
	glGenBuffers(1, &VBO_F);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_F);
	glBufferData(GL_ARRAY_BUFFER, nVertices_flag * sizeof(math_utils::Point), &Flag[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Scollego il VAO
	glBindVertexArray(0);

    draw_spark(Sparks);
    glGenVertexArrays(1, &VAO_SPARKS);
	glBindVertexArray(VAO_SPARKS);
	glGenBuffers(1, &VBO_SPARKS);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_SPARKS);
	glBufferData(GL_ARRAY_BUFFER, nPoint* sizeof(math_utils::Point), &Sparks[0], GL_STATIC_DRAW);
	// Configura l'attributo posizione
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Configura l'attributo Colore
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);	

    glBindVertexArray(0);

	//Disegno NAVICELLA	
	draw_ship(Ship);
	//Genero un VAO navicella
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices_ship * sizeof(math_utils::Point), &Ship[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Scollego il VAO
	glBindVertexArray(0);
	
    int fuel_increments = draw_fuel_tank(Tank);
    glGenVertexArrays(1, &VAO_TANK);
	glBindVertexArray(VAO_TANK);
	glGenBuffers(1, &VBO_TANK);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_TANK);
	glBufferData(GL_ARRAY_BUFFER, fuel_increments * sizeof(math_utils::Point), &Tank[0], GL_STATIC_DRAW);
	// Configura l'attributo posizione
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Configura l'attributo Colore
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);	
    glBindVertexArray(0);

	Projection = glm::ortho(0.0f, float(width), 0.0f, float(height));
	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");
    MatProj1 = glGetUniformLocation(programId_1, "Projection");
	MatModel1 = glGetUniformLocation(programId_1, "Model");
	loc = glGetUniformLocation(programId_1, "t");
	color = glGetUniformLocation(programId_1, "aColor");
}

void initShader(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_C_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_C_M.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);

    char* vertexShader1 = (char*)"vertexShader_C_M.glsl";
	char* fragmentShader1 = (char*)"fragmentShader_C_M_1.glsl";

	programId_1 = ShaderMaker::createProgram(vertexShader1, fragmentShader1);

	glUseProgram(programId);
}


void keyboardPressedEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		ship.center = true;
		break;
	case 'a':
		ship.left = true;
		break;
	case 'd':
		ship.right = true;
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
	}
}

void keyboardReleasedEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		ship.center = false;
		break;
	case 'a':
		ship.left = false;
		break;
	case 'd':
		ship.right = false;
		break;
	default:
		break;
	}
}


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	srand (time(NULL));

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("2D Lunar Lander");
	glutDisplayFunc(drawScene);
	//Evento tastiera tasi premuti
	glutKeyboardFunc(keyboardPressedEvent);
	// //Evento tastiera tasto rilasciato

	glutKeyboardUpFunc(keyboardReleasedEvent);
	glutTimerFunc(5, update, 0);
	// glutTimerFunc(500, updateV, 0);
	glewExperimental = GL_TRUE;
	glewInit();

	initShader();
	init();

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutMainLoop();
}