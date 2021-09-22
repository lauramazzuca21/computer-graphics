#include <glm/glm.hpp>
#include <vector>

#define nTriangles 30
#define PI 3.14159265358979323846

namespace colors {
    static const glm::vec4 white 			= { 1.0,1.0,1.0, 1.0 };
    static const glm::vec4 black 			= { 0.0,0.0,0.0, 1.0 };
    static const glm::vec4 darkest_gray		= {34.0/255.0,34.0/255.0,34.0/255.0, 1.0 };
    static const glm::vec4 dark_gray		= {51.0/255.0,51.0/255.0,51.0/255.0, 1.0 };
    static const glm::vec4 light_gray		= {85.0/255.0,85.0/255.0,85.0/255.0, 1.0 };
    static const glm::vec4 lighter_gray		= {118.0/255.0,118.0/255.0,118.0/255.0, 1.0 };
    static const glm::vec4 lightest_gray	= {169.0/255.0,169.0/255.0,169.0/255.0, 1.0 };
    static const glm::vec4 copper			= {184.0/255.0,115.0/255.0,51.0/255.0, 1.0 };
    static const glm::vec4 maximum_green 	= { 83.0/255.0,141.0/255.0,34.0/255.0, 1.0 };
    static const glm::vec4 lincoln_green 	= { 36.0/255.0,85.0/255.0,1.0/255.0, 1.0 };
    static const glm::vec4 persian_plum 	= { 107.0/255.0,29.0/255.0,25.0/255.0, 1.0 };
    static const glm::vec4 red			 	= { 1.0,0.0,0.0, 1.0 };
    static const glm::vec4 orange		 	= { 1.0,90.0/255.0,0.0, 1.0 };
    static const glm::vec4 orange_peel	 	= { 1.0,154.0/255.0,0.0, 1.0 };
    static const glm::vec4 vivid_orange	 	= { 1.0,232.0/255.0,8.0/255.0, 1.0 };
    static const glm::vec4 lemon		 	= { 1.0,232.0/255.0,8.0/255.0, 1.0 };
    static const glm::vec4 cinnabar 		= { 219.0/255.0,78.0/255.0,70.0/255.0, 1.0 };
    static const glm::vec4 midnight_blue	= { 28.0/255.0,29.0/255.0,96.0/255.0, 1.0 };
    static const glm::vec4 blue_flame		= { 94.0/255.0,186.0/255.0,201.0/255.0, 1.0 };
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

static void disegna_cerchio(float cx, float cy, float raggiox, float raggioy, glm::vec4 color_top, glm::vec4 color_bot, math_utils::Point* Cerchio, int triangles = nTriangles)
{
	int i;
	float stepA = (2 * PI) / triangles;

	int comp = 0;
	for (i = 0; i < triangles; i++)
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

//Game objects

namespace objects {
class Stars {
    public:
    static const int timeout = 500;
    int timeout_stars = -1;
    int nStars = 50;
    std::vector<math_utils::Point> pos_stars;

    void update(int w, int h) {
        if (timeout_stars < 0) {
            update_stars_positions(w, h);
            timeout_stars = timeout;
        }
        else
            --timeout_stars;
    }

    private:
    void update_stars_positions(int w, int h) {
        pos_stars.clear();
        for (int i = 0; i < nStars; i++) {
            pos_stars.push_back(math_utils::Point(rand()%w, rand()%h, 0.0f));
        }
    }

};


class Sparks {
typedef struct { 
	float alpha; 
	float xFactor; 
	float yFactor; 
	float drag; 
	math_utils::Point p;
} Particle;

private:
int w, h;
std::vector<glm::vec4> flameColors = {colors::lemon, colors::orange, colors::orange_peel, colors::red, colors::vivid_orange, colors::blue_flame};
public:
	std::vector<Particle> particles;

	Sparks(int w, int h) : w(w), h(h) {}

	void emit(float x, float y, int n_particles) {
	for (int i = 0; i < n_particles; i++) {
		Particle p;
		p.p.x = x;
		p.p.y = y;
		p.alpha = 1.0;
		p.drag = 1.05;
		p.xFactor = (rand() % 1000 + 1) / 300 * (rand() % 2 == 0 ? -1 : 1);
		p.yFactor = (rand() % 1000 + 1) / 300 * -1;
		particles.push_back(p);
	}
	}

	void update() {
		for (int i = 0; i < particles.size(); i++) {
			particles.at(i).xFactor /= particles.at(i).drag;
			particles.at(i).yFactor /= particles.at(i).drag;

			particles.at(i).p.x += particles.at(i).xFactor;
			particles.at(i).p.y += particles.at(i).yFactor;

			particles.at(i).alpha -= 0.05;

			if (particles.at(i).alpha <= 0.0) {
				particles.erase(particles.begin() + i);
			}
		}
	}
};

class Ship {
    public:
    float posx = 0.0f; //coordinate sul piano della posizione iniziale della navicella
    float posy = 0.0f;
	float angle = 0.0f;
	static constexpr float gravity = 0.005f;
	float velx = 0.0f, vely = 0.0f;

	bool left = false;
	bool right = false;
	bool center = false;

	bool last_hit_left = false;

	Sparks sparks;
	float fuel = fuel_full;
	bool filling = false;
	static constexpr float thrust = 0.02f;
	static constexpr float fuel_inc = 0.0075f;
	static constexpr float fuel_full = 1.0f;

	Ship(float w, float h) : posx(w/2.0f), posy(h*0.9f), sparks(w, h) {}

	void update() {
 
		posx += velx;
    	posy += vely;
		angle += (last_hit_left ? -1.0f : 1.0f) * PI / (3 * 180.0f);

		if (right) { angle +=  PI / 180.0f; sparks.emit(posx+30.0f, posy, 1); last_hit_left = false;}
		if (left) { angle -=  PI / 180.0f; sparks.emit(posx-30.0f, posy, 1); last_hit_left = true;}

		if (center && fuel > 0.0f && !filling) {
			fuel -= thrust;
			filling = fuel <= 0.0f ? true : false;
			velx -= thrust * sin(angle);
			vely += thrust * cos(angle);
			sparks.emit(posx, posy-30.0f, 10);
		}

		else if (fuel < fuel_full && filling)
		{
			fuel += fuel_inc;
			filling = fuel >= fuel_full ? false : true;
		}

		vely -= gravity;
	}

};

}