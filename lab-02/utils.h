#include <glm/glm.hpp>

#define nTriangles 15
#define PI 3.14159265358979323846

namespace colors {
    static const glm::vec4 top 				= {1.0,1.0,1.0, 0.8};
    static const glm::vec4 bianco 			= { 1.0,1.0,1.0, 1.0 };
    static const glm::vec4 nero 			= { 0.0,0.0,0.0, 1.0 };
    static const glm::vec4 maximum_green 	= { 83.0/255.0,141.0/255.0,34.0/255.0, 1.0 };
    static const glm::vec4 lincoln_green 	= { 36.0/255.0,85.0/255.0,1.0/255.0, 1.0 };
    static const glm::vec4 persian_plum 	= { 107.0/255.0,29.0/255.0,25.0/255.0, 1.0 };
    static const glm::vec4 cinnabar 		= { 219.0/255.0,78.0/255.0,70.0/255.0, 1.0 };
    static const glm::vec4 midnight_blue	= { 28.0/255.0,29.0/255.0,96.0/255.0, 1.0 };
    static const glm::vec4 top_Navicella 	= { 1.0,1.5,0.0,1.0 };
    static const glm::vec4 bot_Navicella 	= { 1.0,0.8,0.8,0.5 };
    static const glm::vec4 top_Corpo     	= { 0.0,0.5,0.8,1.0 };
    static const glm::vec4 bot_Corpo     	= { 0.0,0.2,0.5,1.0 };
    static const glm::vec4 top_Oblo      	= { 0.2,0.9,0.1,1.0 };
    static const glm::vec4 bot_Oblo      	= { 0.0,0.2,0.8,1.0 };
}

namespace math_utils {

    struct Point{ 
	float x, y, z, r, g, b, a; 
	
	Point() {}

	Point(float x, float y, float z, float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 0.0f) 
	: x(x), y(y), z(z), r(r), g(g), b(b), a(a) 
	{}  

    Point& operator =(const Point& p)
    {
        x = p.x;
        y = p.y;
        z = p.z;
        r = p.r;
        g = p.g;
        b = p.b;
        a = p.a;
        return *this;
    }
    };
	static float lerp(float a, float b, float t) {
		//Interpolazione lineare tra a e b secondo amount
		return (1 - t) * a + t * b;
	}

	static double  degtorad(double angle) {
		return angle * PI / 180;
	}

	static double  radtodeg(double angle) {
	return angle / (PI / 180);
	}

}


namespace draw_utils {

static void disegna_piano(float x, float y, float width, float height, glm::vec4 color_top, glm::vec4 color_bot, math_utils::Point* piano)
{
	piano[0].x = x;	piano[0].y = y; piano[0].z = 0;
	piano[0].r = color_bot.r; piano[0].g = color_bot.g; piano[0].b = color_bot.b; piano[0].a = color_bot.a;
	piano[1].x = x + width;	piano[1].y = y;	piano[1].z = 0;
	piano[1].r = color_top.r; piano[1].g = color_top.g; piano[1].b = color_top.b; piano[1].a = color_top.a;
	piano[2].x = x + width;	piano[2].y = y + height; piano[2].z = 0;
	piano[2].r = color_bot.r; piano[2].g = color_bot.g; piano[2].b = color_bot.b; piano[2].a = color_bot.a;

	piano[3].x = x + width;	piano[3].y = y + height; piano[3].z = 0;
	piano[3].r = color_bot.r; piano[3].g = color_bot.g; piano[3].b = color_bot.b; piano[3].a = color_bot.a;
	piano[4].x = x;	piano[4].y = y + height; piano[4].z = 0;
	piano[4].r = color_top.r; piano[4].g = color_top.g; piano[4].b = color_top.b; piano[4].a = color_top.a;
	piano[5].x = x;	piano[5].y = y; piano[5].z = 0;
	piano[5].r = color_bot.r; piano[5].g = color_bot.g; piano[5].b = color_bot.b; piano[5].a = color_bot.a;
}

static void disegna_cerchio(float cx, float cy, float raggiox, float raggioy, glm::vec4 color_top, glm::vec4 color_bot, math_utils::Point* Cerchio)
{
	int i;
	float stepA = (2 * PI) / nTriangles;

	int comp = 0;
	for (i = 0; i < nTriangles; i++)
	{
		Cerchio[comp].x = cx + cos((double)i * stepA) * raggiox;
		Cerchio[comp].y = cy + sin((double)i * stepA) * raggioy;
		Cerchio[comp].z = 0.0;
		Cerchio[comp].r = color_top.r; Cerchio[comp].g = color_top.g; Cerchio[comp].b = color_top.b; Cerchio[comp].a = color_top.a;

		Cerchio[comp + 1].x = cx + cos((double)(i + 1) * stepA) * raggiox;
		Cerchio[comp + 1].y = cy + sin((double)(i + 1) * stepA) * raggioy;
		Cerchio[comp + 1].z = 0.0;
		Cerchio[comp + 1].r = color_top.r; Cerchio[comp + 1].g = color_top.g; Cerchio[comp + 1].b = color_top.b; Cerchio[comp + 1].a = color_top.a;

		Cerchio[comp + 2].x = cx;
		Cerchio[comp + 2].y = cy;
		Cerchio[comp + 2].z = 0.0;
		Cerchio[comp + 2].r = color_bot.r; Cerchio[comp + 2].g = color_bot.g; Cerchio[comp + 2].b = color_bot.b; Cerchio[comp + 2].a = color_bot.a;

		comp += 3;
	}
}
}