
#include "CoronaGame.h"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include "ShaderMaker.h"
/***************GAME UPDATES***************/
void updateNemici(int value)
{
	frame++;
	if (frame % 30 == 0)
	{
		dxnemici--;
		dynemici--;
	}
	glutTimerFunc(5, updateNemici, 0);
	glutPostRedisplay();
}


void update(int a)
{
	float timeValue = glutGet(GLUT_ELAPSED_TIME);
	t = abs(sin(timeValue));
	// printf("Valore di t %f \n", t);
	glutPostRedisplay();
	glutTimerFunc(50, update, 0);
}

void updateProiettile(int value)
{
	glutTimerFunc(5, updateProiettile, 0);

	if (pos_projectiles.size() == 0 && pos_strong_projectiles.size() == 0)
		return;

	std::vector<int> to_remove;
	for (int i = 0; i < pos_projectiles.size(); i++)
	{
		pos_projectiles.at(i).y++;
		if (pos_projectiles.at(i).y >= height)
			to_remove.push_back(i);
	}
	for (int idx : to_remove)
	{
		pos_projectiles.erase(pos_projectiles.begin()+idx);
	}

	to_remove.clear();
	for (int i = 0; i < pos_strong_projectiles.size(); i++)
	{
		pos_strong_projectiles.at(i).y+=2;
		if (pos_strong_projectiles.at(i).y > height)
			to_remove.push_back(i);
	}
	for (int idx : to_remove)
	{
		pos_strong_projectiles.erase(pos_strong_projectiles.begin()+idx);	
	}
	glutPostRedisplay();
}

void updateV(int a)
{
	bool moving = false;

	if (pressing_left)
	{
		VelocitaOrizzontale -= accelerazione;
		moving = true;
	}

	if (pressing_right)
	{
		VelocitaOrizzontale += accelerazione;
		moving = true;
	}

	if (float_yoffset >= 0) {
		frame_animazione += 6;
		if (frame_animazione >= 360) {
			frame_animazione -= 360;
		}
	}

	if (!moving) {   

		if (VelocitaOrizzontale > 0)
		{
			VelocitaOrizzontale -= 1;
			if (VelocitaOrizzontale < 0)
				VelocitaOrizzontale = 0;
		}

		if (VelocitaOrizzontale < 0)
		{
			VelocitaOrizzontale += 1;
			if (VelocitaOrizzontale > 0)
				VelocitaOrizzontale = 0;
		}
	}
	if (pressing_rotate_s)
	{
		angoloV++;
		moving = true;
	}
	if (pressing_rotate_d)
	{
		angoloV--;
		moving = true;
	}
	//Aggioramento della posizione in x della navicella, che regola il movimento orizzontale
	posx += VelocitaOrizzontale;
	// Gestione Bordi viewport:
	// Se la navicella assume una posizione in x minore di 0 o maggiore di width dello schermo
	// facciamo rimbalzare la navicella ai bordi dello schermo
	if (posx < 0) {
		posx = 0;
		VelocitaOrizzontale = -VelocitaOrizzontale * 0.8;
	}
	if (posx > width) {
		posx = width;
		VelocitaOrizzontale = -VelocitaOrizzontale * 0.8;
	}
	// calcolo y come somma dei seguenti contributi: pos. di equilibrio, oscillazione periodica
	posy = posizione_di_equilibrio + sin(math_utils::degtorad(frame_animazione)) * range_fluttuazione;
	angolo = cos(math_utils::degtorad(frame_animazione)) * angle_offset - VelocitaOrizzontale * 1.3;
	glutPostRedisplay();
	glutTimerFunc(15, updateV, 0);
}

void fire() {

	if (pressing_strong_attack && pos_strong_projectiles.size() < 3)
	{
		pos_strong_projectiles.push_back(math_utils::Point(posx, posy, 0.0f));
	}
	else if (pressing_attack && pos_projectiles.size() < 10)
	{
		pos_projectiles.push_back(math_utils::Point(posx, posy, 0.0f));
	}
}

void keyboardPressedEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case ' ':
		pressing_attack = true;
		fire();
		break;
	case 'a':
		pressing_left = true;
		break;
	case 'd':
		pressing_right = true;
		break;
	case 'r':
		pressing_rotate_s = true;
		break;
	case 'f':
		pressing_rotate_d = true;
		break;
	case 'q': 
		pressing_strong_attack = true;
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
		pressing_attack = false;
		break;
	case 'a':
		pressing_left = false;
		break;
	case 'd':
		pressing_right = false;
		break;
	case 'r':
		pressing_rotate_s = false;
		break;
	case 'f':
		pressing_rotate_d = false;
		break;
	case 'q':
		pressing_strong_attack = false;
		break;
	default:
		break;
	}
}

/**************DRAW SCENE OBJECTS**************/
void disegna_tentacoli(math_utils::Point* Punti, glm::vec4 color_top, glm::vec4 color_bot)
{
	float alfa = 2.0f, step=PI/4;
	math_utils::Point P0, PStart, PEnd, PSpike[8], *Spike;
	Spike = new math_utils::Point[nvSpikes];
	P0.x = 0.0;	P0.y = 0.0;  // centro circonferenza
	int cont = 0;
	for (int i = 0; i < 8 ;i++)//  per ciascuno degli 8 tentacoli
	{
		PStart.x = cos(i*step);  // punto sulla circonferenza 
		PStart.y = sin(i*step);
		PStart.z = 0.0;
		PStart.r = color_bot.r; PStart.g = color_bot.g; PStart.b = color_bot.b; PStart.a = color_bot.a;
		PEnd.x = math_utils::lerp(P0.x, PStart.x, alfa);  //punto fuori circonferenza
		PEnd.y = math_utils::lerp(P0.y, PStart.y, alfa);
		PEnd.z = 0.0;
		PEnd.r = color_top.r; PEnd.g = color_top.g; PEnd.b = color_top.b; PEnd.a = color_top.a;
		
		Punti[cont] = PStart;
		Punti[cont + 1] = PEnd;
		cont += 2;

		PSpike[i] = PEnd;
	}

	for (int i = 0; i < 8 ;i++)// for each tentacle draw the "pon pon"
	{
		//draw spikes
		draw_utils::disegna_cerchio(PSpike[i].x, PSpike[i].y, 0.25, 0.25, colors::lincoln_green, colors::maximum_green, Spike);

		for (int j = 0; j < 3 * nTriangles; j++)
		{
			Punti[cont] = Spike[j];
			cont++;
		}
	}
}

void Parte_superiore_navicella(float cx, float cy, float raggiox, float raggioy, glm::vec4 color_top, glm::vec4 color_bot, math_utils::Point* Cerchio) {
	int i;
	int comp = 0; 
	float A = PI / 4;
	float B = 3 / 4 * PI;
	// arco tra A e B dato da PI/2
	float stepA = (PI/2)  / nTriangles;

	for (i = 0; i < nTriangles ; i++)
		{
		Cerchio[comp].x = cx + cos(A+(double)i * stepA) * raggiox;
		Cerchio[comp].y = cy + sin(A+(double)i * stepA) * raggioy;
		Cerchio[comp].z = 0;
		Cerchio[comp].r = color_top.r; Cerchio[comp].g = color_top.g; Cerchio[comp].b = color_top.b; Cerchio[comp].a = color_top.a;
		Cerchio[comp + 1].x = cx + cos(A+(double)(i + 1) * stepA) * raggiox;
		Cerchio[comp + 1].y = cy + sin(A+(double)(i + 1) * stepA) * raggioy;
		Cerchio[comp + 1].z = 0;
		Cerchio[comp + 1].r = color_top.r; Cerchio[comp + 1].g = color_top.g; Cerchio[comp + 1].b = color_top.b; Cerchio[comp + 1].a = color_top.a;
		Cerchio[comp + 2].x =cx;
		Cerchio[comp + 2].y = cy;
		Cerchio[comp + 2].z = 0.0;
		Cerchio[comp + 2].r = color_bot.r; Cerchio[comp + 2].g = color_bot.g; Cerchio[comp + 2].b = color_bot.b; Cerchio[comp + 2].a = color_bot.a;
		comp += 3;
	}
	
}


void disegna_Navicella(glm::vec4 color_top_Navicella, glm::vec4 color_bot_Navicella, glm::vec4 color_top_corpo, glm::vec4 color_bot_corpo, glm::vec4 color_top_Oblo, glm::vec4 color_bot_Oblo, math_utils::Point* Navicella)
{
	int cont, i, v_Oblo;
	math_utils::Point* Corpo;
	int v_Corpo =3 * nTriangles;
	Corpo = new math_utils::Point[v_Corpo];

	Parte_superiore_navicella(0.0, -0.1, 1.0, 1.0, color_top_Navicella, color_bot_Navicella, Navicella);

	//Costruisci Corpo   cerchio + cintura + 3 bottoni 
	draw_utils::disegna_cerchio(0.0, -1.0, 1.0, 1.0, color_top_corpo, color_bot_corpo, Corpo);
	cont = 3 * nTriangles;

	for (i = 0; i < 3 * nTriangles; i++)
	{
		Navicella[i + cont] = Corpo[i];
	}
	//Costruisci Corpo 2
	draw_utils::disegna_cerchio(1.0, -1.0, 0.5, 0.5, color_top_Oblo, color_bot_Oblo, Corpo);
	cont = 6 * nTriangles;
	for (i = 0; i < 3 * nTriangles; i++)
	{
		Navicella[i + cont] = Corpo[i];
	}

	//Costruisci Corpo 3
	draw_utils::disegna_cerchio(-1.0, -1.0, 0.5, 0.5, color_top_Oblo, color_bot_Oblo, Corpo);

	cont = 9 * nTriangles;
	for (i = 0; i < 3 * nTriangles; i++)
	{
		Navicella[i + cont] = Corpo[i];
	}
}

void draw_projectile() {
	Projectiles[0] = math_utils::Point(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);
}

void draw_strong_projectile() {
	math_utils::Point StrongProjectile[6];
	draw_utils::disegna_piano(0.0f, 0.0f, 8.0f, 32.0f, colors::persian_plum, colors::cinnabar, StrongProjectile);
	for (int i = 0; i < 6; i++)
		Projectiles[1+i] = StrongProjectile[i];
}

void disegna_nemico(glm::vec4 color_top_Nemico, glm::vec4 color_bot_Nemico, glm::vec4 color_top_Occhio, glm::vec4 color_bot_Occhio, math_utils::Point* Nemico)
{
	int i, cont;
	int v_faccia = 3 * nTriangles;
	int v_tentacoli_spike = nvTentacoli + nvSpikes;
	math_utils::Point* Occhio = new math_utils::Point[v_faccia];
	math_utils::Point* Tentacoli = new math_utils::Point[nvTentacoli];

	// Disegna faccia del Nemico
	draw_utils::disegna_cerchio(0.0, 0.0, 1.0, 1.0, color_top_Nemico, color_bot_Nemico, Nemico);
	
	// Disegna i due occhi
	draw_utils::disegna_cerchio(-0.5, 0.5, 0.1, 0.1, color_top_Occhio, color_bot_Occhio, Occhio);
	cont = 3 * nTriangles;
	for (i = 0; i < v_faccia; i++)
		Nemico[i + cont] = Occhio[i];
	draw_utils::disegna_cerchio(0.5, 0.5, 0.1, 0.1, color_top_Occhio, color_bot_Occhio, Occhio);
		cont = cont + 3 * nTriangles;
	for (i = 0; i < v_faccia; i++)
		Nemico[i + cont] = Occhio[i];

	cont = cont + 3 * nTriangles;

	//Aggiungo bocca
	Nemico[cont].x = -0.5;	Nemico[cont].y = -0.5;	Nemico[cont].z = 0.0;
	Nemico[cont].r = colors::nero.r;Nemico[cont].g = colors::nero.g;Nemico[cont].b = colors::nero.b;Nemico[cont].a = colors::nero.a;

	Nemico[cont + 1].x = -0.25;	Nemico[cont + 1].y = -0.25;	Nemico[cont + 1].z = 0.0;
	Nemico[cont + 1].r = colors::nero.r; Nemico[cont + 1].g = colors::nero.g; Nemico[cont + 1].b = colors::nero.b; Nemico[cont + 1].a = colors::nero.a;

	Nemico[cont + 2].x = 0.0;	Nemico[cont + 2].y = -0.5;	Nemico[cont + 2].z = 0.0;
	Nemico[cont + 2].r = colors::nero.r; Nemico[cont + 2].g = colors::nero.g; Nemico[cont + 2].b = colors::nero.b; Nemico[cont + 2].a = colors::nero.a;

	Nemico[cont + 3].x = 0.25;	Nemico[cont + 3].y = -0.25;	Nemico[cont + 3].z = 0.0;
	Nemico[cont + 3].r = colors::nero.r; Nemico[cont + 3].g = colors::nero.g; Nemico[cont + 3].b = colors::nero.b; Nemico[cont + 3].a = colors::nero.a;

	Nemico[cont + 4].x = 0.5;Nemico[cont + 4].y = -0.5;	Nemico[cont + 4].z = 0.0;
	Nemico[cont + 4].r = colors::nero.r; Nemico[cont + 4].g = colors::nero.g; Nemico[cont + 4].b = colors::nero.b; Nemico[cont + 4].a = colors::nero.a;

	cont = cont + 5;
	
	disegna_tentacoli(Tentacoli, colors::maximum_green, colors::lincoln_green);
	for (i = 0; i < v_tentacoli_spike; i++)
		Nemico[cont + i] = Tentacoli[i];
}

void draw_stars(math_utils::Point* stars) {
	int cont=0;
	math_utils::Point* tmp;
	tmp = new math_utils::Point[nVertices_star];
	draw_utils::disegna_cerchio(0.0f, 0.0f, 1.0f, 1.0f, colors::bianco, colors::bianco, tmp);
	for (int j = 0; j < 3 * nTriangles; j++)
	{
		stars[cont].x = tmp[j].x;
		stars[cont].y = tmp[j].y;
		stars[cont].z = tmp[j].z;
		stars[cont].r = tmp[j].r;stars[cont].g = tmp[j].g;stars[cont].b = tmp[j].b;stars[cont].a = tmp[j].a;
		cont++;
	}
}

void initShader(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_C_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_C_M.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	char* vertexShader1 = (char*)"vertexShader_C_M.glsl";
	char* fragmentShader1 = (char*)"fragmentShader_C_M_1.glsl";

	programId_1 = ShaderMaker::createProgram(vertexShader1, fragmentShader1);
}

void update_stars_positions() {
	pos_stars.clear();
	for (int i = 0; i < 50; i++) {
		pos_stars.push_back(math_utils::Point(rand()%width, rand()%height, 0.0f));
	}
}

void init(void)
{
	//Disegno SPAZIO/CIELO
	draw_utils::disegna_piano(0.0, 0.0, 1.0, 1.0, colors::midnight_blue, colors::nero, Cielo);
	//Genero un VAO
	glGenVertexArrays(1, &VAO_CIELO);
	//Ne faccio il bind (lo collego, lo attivo)
	glBindVertexArray(VAO_CIELO);
	//AL suo interno genero un VBO
	glGenBuffers(1, &VBO_C);
	//Ne faccio il bind (lo collego, lo attivo, assegnandogli il tipo GL_ARRAY_BUFFER)
	glBindBuffer(GL_ARRAY_BUFFER, VBO_C);
	//Carico i dati vertices sulla GPU
	glBufferData(GL_ARRAY_BUFFER, vertices_cielo * sizeof(math_utils::Point), &Cielo[0], GL_STATIC_DRAW);
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

	//Disegno NAVICELLA	
	disegna_Navicella(colors::top_Navicella, colors::bot_Navicella, colors::top_Corpo, colors::bot_Corpo, colors::top_Oblo, colors::bot_Oblo, Navicella);
	//Genero un VAO navicella
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices_Navicella * sizeof(math_utils::Point), &Navicella[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Scollego il VAO
	glBindVertexArray(0);
	
	draw_projectile();
	draw_strong_projectile();
	//VAO for projectiles
	glGenVertexArrays(1, &VAO_PROJ);
	glBindVertexArray(VAO_PROJ);
	glGenBuffers(1, &VBO_P);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_P);
	glBufferData(GL_ARRAY_BUFFER, 7*sizeof(math_utils::Point), &Projectiles[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//Scollego il VAO
	glBindVertexArray(0);

	//Disegna nemici
	disegna_nemico(colors::lincoln_green, colors::bianco, colors::nero, colors::bianco, Nemico);
	//Genero un VAO

	glGenVertexArrays(1, &VAO_NEMICO);
	glBindVertexArray(VAO_NEMICO);
	glGenBuffers(1, &VBO_N);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_N);
	glBufferData(GL_ARRAY_BUFFER, nVertices_Nemico * sizeof(math_utils::Point), &Nemico[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//Scollego il VAO
	glBindVertexArray(0);
	
	colpito = new bool* [numero_di_righe];
	for (int i = 0; i < numero_di_righe; i++)
		colpito[i] = new bool[nemici_per_riga];
	for (int i = 0; i < numero_di_righe; i++)
		for (int j = 0; j < nemici_per_riga; j++)
		{
			colpito[i][j] = false;
			printf("%s", colpito[i][j] ? "true \n" : "false \n");
		}
	//Definisco il colore che verr� assegnato allo schermo
	glClearColor(1.0, 0.5, 0.0, 1.0);

	Projection = glm::ortho(0.0f, float(width), 0.0f, float(height));
	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");
    MatProj1 = glGetUniformLocation(programId_1, "Projection");
	MatModel1 = glGetUniformLocation(programId_1, "Model");
	loc = glGetUniformLocation(programId_1, "t");
}

bool deleteHitRow(int idx) {
	int colpitoRiga = 0;
	for(int i = 0; i < nemici_per_riga; i++)
	{
		 if (colpito[idx][i]) {
				colpitoRiga++;
			}
	}

	return colpitoRiga == nemici_per_riga;
}

void checkHitEnemy() {
			// calcolo virus colpiti
	for (int i = 0; i < numero_di_righe; i++)
	{
		posyN = height - i * passo_righe - 20 + rand() % 20;
		for (int j = 0; j < nemici_per_riga; j++)
		{			
			posxN = j * (passo_Nemici)+passo_Nemici / 2 + rand() % 40;
			std::vector<int> to_remove;
			for (int k =0; k < pos_projectiles.size() ; k++)
			{
				if ( k < pos_projectiles.size()
				&&((pos_projectiles.at(k).x >= posxN - dxnemici) && (pos_projectiles.at(k).x <= posxN + dxnemici)) 
				&& ((pos_projectiles.at(k).y >= posyN - dynemici) && (pos_projectiles.at(k).y <= posyN + dynemici))
				&& !colpito[i][j])
				{
						NumeroColpiti++;
						colpito[i][j] = true;
						to_remove.push_back(k);
						printf("Colpiti: %d", NumeroColpiti);
				}
			}

			for ( int idx : to_remove)
			{
				pos_projectiles.erase(pos_projectiles.begin()+idx);
			}

			for (int sk = 0; sk < pos_strong_projectiles.size(); sk++)
			{
				if ( sk < pos_strong_projectiles.size()
				&&	((pos_strong_projectiles.at(sk).x >= posxN - dxnemici) && (pos_strong_projectiles.at(sk).x <= posxN + dxnemici)) 
				&& ((pos_strong_projectiles.at(sk).y >= posyN - dynemici) && (pos_strong_projectiles.at(sk).y <= posyN + dynemici))
				&& !colpito[i][j])
				{
						NumeroColpiti++;
						colpito[i][j] = true;
						printf("Colpiti: %d", NumeroColpiti);
				}
			}
		}
	}
}

void updateDrawEnemy() {
	Model = glm::mat4(1.0);
	Model = glm::translate(Model, glm::vec3(posxN + dxnemici, posyN + dynemici, 0));
	Model = scale(Model, glm::vec3(30.0, 30.0, 1.0));
	Model = glm::rotate(Model, glm::radians(angolo), glm::vec3(0.0, 0.0, 1.0));
	glUniformMatrix4fv(MatModel1, 1, GL_FALSE, value_ptr(Model));
	// faccia e occhi
	glDrawArrays(GL_TRIANGLES, 0, nVertices_Nemico - nvBocca - nvTentacoli - nvSpikes);
	glLineWidth(3.0);  // bocca
	glDrawArrays(GL_LINE_STRIP, nVertices_Nemico - nvBocca - nvTentacoli - nvSpikes, nvBocca);
	glLineWidth(8.0); // tentacoli
	glDrawArrays(GL_LINES, nVertices_Nemico - nvTentacoli - nvSpikes, nvTentacoli);
	glDrawArrays(GL_TRIANGLES, nVertices_Nemico - nvSpikes, nvSpikes);
}

void drawScene(void)
{
	if (gameEnd)
	{
		return;
	}

	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(programId);  // attiva fragment shader basic per tutta la scena meno i nemici 

	// Disegna cielo
	glBindVertexArray(VAO_CIELO);
	Model = glm::mat4(1.0);
	Model = glm::scale(Model, glm::vec3(float(width), float(height), 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glDrawArrays(GL_TRIANGLES, 0, vertices_cielo);
	glBindVertexArray(0);

	if (timeoutStars < 0) {
		update_stars_positions();
			timeoutStars = TIMEOUT_STARS;
	}
	else
		--timeoutStars;
	
	glBindVertexArray(VAO_STAR);
	for (int i = 0; i < 50; i++)
	{
		Model = glm::mat4(1.0);
		Model = glm::translate(Model, glm::vec3(pos_stars.at(i).x, pos_stars.at(i).y, 1.0));
		Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, 1.0));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		glDrawArrays(GL_TRIANGLES, 0, nVertices_star);
	}
	glBindVertexArray(0);

	//Disegno Navicella
	glBindVertexArray(VAO);
	Model = glm::mat4(1.0);
	Model = glm::translate(Model, glm::vec3(posx, posy, 0.0));
	Model = scale(Model, glm::vec3(80.0, 40.0, 1.0));
	Model = glm::rotate(Model, glm::radians(angolo), glm::vec3(0.0, 0.0, 1.0));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glDrawArrays(GL_TRIANGLES, 0, nVertices_Navicella);
	glBindVertexArray(0);
	
	checkHitEnemy();
	int eliminaRighe = 0;
	for (int i = 0; i < numero_di_righe; i++)
	{
		if (deleteHitRow(i))
			eliminaRighe++;
	}
	numero_di_righe -= eliminaRighe;
	// Disegna nemici VIRUS
	glBindVertexArray(VAO_NEMICO);
	for (int i = 0; i < numero_di_righe; i++)
	{
		posyN = height - i * passo_righe - 20;
		for (int j = 0; j < nemici_per_riga; j++)
		{
			posxN = j * (passo_Nemici) + passo_Nemici / 2;
			if (!colpito[i][j]) {
				updateDrawEnemy();
			}
		}
	}

	glBindVertexArray(0);

		//Disegno il proiettile
	glBindVertexArray(VAO_PROJ);
	for (auto p : pos_projectiles)
	{
		glPointSize(8.0);
		Model = glm::mat4(1.0);
		Model = glm::translate(Model, glm::vec3(p.x, p.y, 0));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		glDrawArrays(GL_POINTS, 0, 1);
	}

	for (auto p : pos_strong_projectiles)
	{
		Model = glm::mat4(1.0);
		Model = glm::translate(Model, glm::vec3(p.x, p.y, 0));
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
		glDrawArrays(GL_TRIANGLES, 1, 6);
	}
	glBindVertexArray(0);

	if (numero_di_righe == 0 || posyN + passo_Nemici <= posy + 40.0f)
		gameEnd = true;
	glutSwapBuffers();
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
	glutCreateWindow("2D Game COV-19");
	glutDisplayFunc(drawScene);
	//Evento tastiera tasi premuti
	glutKeyboardFunc(keyboardPressedEvent);
	//Evento tastiera tasto rilasciato

	glutKeyboardUpFunc(keyboardReleasedEvent);
	glutTimerFunc(500, update, 0);
	glutTimerFunc(500, updateV, 0);
	glutTimerFunc(5, updateProiettile, 0);
	glutTimerFunc(500, updateNemici, 0);
	glewExperimental = GL_TRUE;
	glewInit();

	initShader();
	init();

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutMainLoop();
}