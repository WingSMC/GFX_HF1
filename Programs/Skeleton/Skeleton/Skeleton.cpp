//=============================================================================================
// Mintaprogram: Zöld háromszög. Ervenyes 2019. osztol.
//
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat, BOM kihuzando.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kiveve
// - Mashonnan atvett programresszleteket forrasmegjeloles nelkul felhasznalni es
// - felesleges programsorokat a beadott programban hagyni!!!!!!! 
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak 
// A keretben nem szereplo GLUT fuggvenyek tiltottak.
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Dremák Gergely
// Neptun : KSHSLY
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
// 
// 
// Felhasznált segédanyagok: Program: Deriválás duális számokkal
// 
// Elvileg a metszéspontok kiszámításán kívül minden működik.
//=============================================================================================

#include "framework.h"

const float g_maxSelectionDistance = 0.02f;
const float PI = 3.141592653f;
const float TAU = 2*PI;
GPUProgram g_gpuProgram;

const char* const vertexSource = R"(
#version 330
precision highp float;
layout(location = 0) in vec2 vp;
void main() {
	gl_PointSize = 7.0;
	gl_Position = vec4(vp, 0, 1);
}
)";

const char* const fragmentSource = R"(
#version 330
precision highp float;
uniform vec4 color;
out vec4 outColor;
void main() {
	outColor = color;
}
)";

class Drawable {
	vec4 color;
	vec4 defColor;
	int nPoints;
	unsigned int vao;
public:
	Drawable(const std::vector<vec2>& points, const vec4& color) {
		this->defColor = this->color = color;
		this->nPoints = points.size();

		glGenVertexArrays(1, &vao);         // create 1 vertex array object
		glBindVertexArray(vao);             // make it active

		unsigned int vbo;                   // vertex buffer objects
		glGenBuffers(1, &vbo);              // Generate 1 vertex buffer objects
		glBindBuffer(GL_ARRAY_BUFFER, vbo); // make this vbo active

		glBufferData(
			GL_ARRAY_BUFFER,
			this->nPoints * sizeof(vec2),
			&points[0],
			GL_STATIC_DRAW
		);

		glEnableVertexAttribArray(0);  // AttribArray 0
		glVertexAttribPointer(
			0,                         // vbo -> AttribArray 0
			2, GL_FLOAT, GL_FALSE,     // two floats/attrib, not fixed-point
			0, NULL                    // stride, offset: tightly packed
		);
	}

	// virtual std::vector<vec2> Intersect(const Line& l) const = 0;
	// virtual std::vector<vec2> Intersect(const Circle& l) const = 0;

	virtual float Dist(const vec2& cursor) const = 0;
	virtual void Draw() const = 0;
	void Draw(const int type) const {
		g_gpuProgram.setUniform(this->color, "color");
		glBindVertexArray(vao);
		glDrawArrays(type, 0, this->nPoints);
	}


	inline void Select() {
		this->color = vec4(1,1,1,1);
	}
	inline void DeSelect() {
		this->color = this->defColor;
	}
};

class Point: public Drawable {
	vec2 location;
public:
	Point(const vec2& location): Drawable(std::vector<vec2>{location}, vec4(1, 1, 0, 1)) {
		this->location = location;
	}

	void Draw() const override {
		Drawable::Draw(GL_POINTS);
	}

	float Dist(const vec2& cursor) const override {
		return length(this->location - cursor);
	}

	inline vec2 getLocation() const { return location; }
	inline float X() const { return location.x; }
	inline float Y() const { return location.y; }
};

class Line: public Drawable {
	vec2 normal;
	float distFromO;
	static std::vector<vec2> calculateDetails(const Point p1, const Point p2) {
		vec2 pv1 = p1.getLocation();
		vec2 pv2 = p2.getLocation();
		vec2 lineDirection = normalize(pv1 - pv2);

		return std::vector<vec2> { pv1 + (2.9f * lineDirection), pv1 - (2.9f * lineDirection) };
	}
public:
	Line(Point p1, Point p2):
		Drawable(calculateDetails(p1, p2), vec4(1, 0, 0, 1)) {
		vec2 pv1 = p1.getLocation();
		vec2 pv2 = p2.getLocation();
		vec2 lineDirection = normalize(pv1 - pv2);
		this->normal = vec2(-lineDirection.y, lineDirection.x);
		this->distFromO = dot(this->normal, pv1);
	}

	float Dist(const vec2& cursor) const override {
		return abs(distFromO - dot(cursor, normal));
	}

	void Draw() const override { Drawable::Draw(GL_LINE_STRIP); }
};

class Circle: public Drawable {
	vec2 center;
	float radius;
	static std::vector<vec2> calculateDetails(const vec2 c, const float radius) {
		std::vector<vec2> points;

#define N_SEG 50

		const float arcSeg = TAU / N_SEG;
		for(size_t i = 0; i < N_SEG; ++i) {
			float x = radius * cosf(i * arcSeg);
			float y = radius * sinf(i * arcSeg);
			points.push_back(c + vec2(x,y));
		}

		return points;
	}
public:
	Circle(const Point center, const float radius):
		Drawable(calculateDetails(center.getLocation(), radius), vec4(0, 1, 1, 1)) {
		this->center = center.getLocation();
		this->radius = radius;
	}

	float Dist(const vec2& cursor) const override {
		return abs(length(this->center - cursor) - this->radius);
	}

	void Draw() const override { Drawable::Draw(GL_LINE_LOOP); }
};

float g_radius = 0.2f;
char g_state = '\0';
std::vector<Point> g_points;
std::vector<Drawable*> g_other_drawables{};
std::vector<Drawable*> g_selected;

void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0, 0, 0, 0);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glLineWidth(3.0);

	g_points.push_back(Point(vec2(0.0f, 0.0f)));
	g_points.push_back(Point(vec2(0.2f, 0.0f)));
	g_other_drawables.push_back(new Line(g_points[0], g_points[1]));

	g_gpuProgram.create(vertexSource, fragmentSource, "outColor");
}
void onDisplay() {
	glClear(GL_COLOR_BUFFER_BIT);
	size_t len = g_other_drawables.size();
	for(size_t i = 0; i < len; ++i) {
		g_other_drawables[i]->Draw();
	}

	len = g_points.size();
	for(size_t i = 0; i < len; ++i) {
		g_points[i].Draw();
	}

	glutSwapBuffers();
}

Point* findClosestPointToCursor(const vec2& cursor) {
	size_t len = g_points.size();
	for(size_t i = 0; i < len; ++i) {
		float dist = g_points[i].Dist(cursor);
		if(dist <= g_maxSelectionDistance) {
			return &g_points[i];
		}
	}
	return NULL;
}
Drawable* findClosestShapeToCursor(const vec2& cursor) {
	size_t len = g_other_drawables.size();
	for(size_t i = 0; i < len; ++i) {
		float dist = g_other_drawables[i]->Dist(cursor);
		if(dist <= g_maxSelectionDistance) {
			return g_other_drawables[i];
		}
	}
	return NULL;
}

void onKeyboard(const unsigned char key, const int pX, const int pY) {
	if(!(
		g_state == '\0' &&
		(key == 'i' || key == 's' || key == 'c' || key == 'l')
	)) return;

	printf("SET STATE   | [%c] -> [%c]\n", g_state, key);
	g_state = key;
}

void resetState() {
	printf("RESET STATE | [%c] -> [ ]\n", g_state);
	size_t len = g_selected.size();
	for(size_t i = 0; i < len; ++i) {
		g_selected[i]->DeSelect();
	}
	g_selected.clear();
	g_state = '\0';
	glutPostRedisplay();
}

void onKeyboardUp(const unsigned char key, const int pX, const int pY) {
	if(g_state != key) return;
	return resetState();
}

void onMouse(const int button, const int state, const int pX, const int pY) {
	float cX = 2.0f * pX / windowWidth - 1;	// flip y
	float cY = 1.0f - 2.0f * pY / windowHeight;
	vec2 cursor(cX, cY);

	if(button == GLUT_LEFT_BUTTON) {
		if(state == GLUT_DOWN) {
			switch(g_state) {
				case 's': {
					Point* p = findClosestPointToCursor(cursor);
					if(p == NULL) return;

					if(g_selected.size() == 0) {
						p->Select();
						g_selected.push_back(p);
						break;
					}

					if(g_selected[0] == p) return;

					vec2 p1 = ((Point*) g_selected[0])->getLocation();
					vec2 p2 = p->getLocation();
					printf("Radius changed from [%f] to", g_radius);
					g_radius = length(p1 - p2);
					printf("[%f]\n", g_radius);
					return resetState();
				}
				case 'c': {
					Point* p = findClosestPointToCursor(cursor);
					if(p == NULL) return;
					g_other_drawables.push_back(new Circle(*p, g_radius));
					return resetState();
				}
				case 'l': {
					Point* p = findClosestPointToCursor(cursor);
					if(p == NULL) return;

					if(g_selected.size() == 0) {
						p->Select();
						g_selected.push_back(p);
						break;
					}

					if(g_selected[0] == p) return;

					g_other_drawables.push_back(new Line(*((Point*) g_selected[0]), *p));
					return resetState();
				}
				case 'i': {
					Drawable* d = findClosestShapeToCursor(cursor);
					if(d == NULL) return;

					if(g_selected.size() == 0) {
						d->Select();
						g_selected.push_back(d);
						break;
					}

					if(g_selected[0] == d) return;
					resetState();
					break;
				}
			}
		}
	}
}

void onMouseMotion(int pX, int pY) {}
void onIdle() {}
