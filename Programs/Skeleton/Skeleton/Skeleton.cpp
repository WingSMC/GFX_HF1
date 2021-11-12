//=============================================================================================
// Mintaprogram: Z�ld h�romsz�g. Ervenyes 2019. osztol.
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
// Nev    : Drem�k Gergely
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
//=============================================================================================
#include "framework.h"

#pragma region

const double PI = 3.141592653;

const char* const vertexSource = R"(
#version 330
precision highp float;

uniform mat4 MVP;
layout(location = 0) in vec2 vp;

void main() {
	gl_Position = vec4(vp.x, vp.y, 0, 1) * MVP;		// transform vp from modeling space to normalized device space
}
)";

const char* const fragmentSource = R"(
#version 330
precision highp float;

uniform vec3 color;
out vec4 outColor;

void main() {
	outColor = vec4(color, 1);
}
)";
#pragma endregion

GPUProgram gpuProgram; // vertex and fragment shaders
unsigned int vao;	   // virtual world on the GPU

void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);

	glGenVertexArrays(1, &vao);   // get 1 vao id
	glBindVertexArray(vao);       // make it active

	unsigned int vbo;             // vertex buffer object
	glGenBuffers(1, &vbo);        // Generate 1 buffer
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	float vertices[] = {
		-0.5f, -0.5f,
		-0.0f, +1.0f,
		+1.0f, -1.0f
	};
	glBufferData(
		GL_ARRAY_BUFFER,           // Copy to GPU target
		sizeof(vertices),
		vertices,
		GL_STATIC_DRAW             // Won't change later
	);

	glEnableVertexAttribArray(0);  // AttribArray 0
	glVertexAttribPointer(
		0,                         // vbo -> AttribArray 0
		2, GL_FLOAT, GL_FALSE,     // two floats/attrib, not fixed-point
		0, NULL                    // stride, offset: tightly packed
	);

	gpuProgram.create(vertexSource, fragmentSource, "outColor");
}

void onDisplay() {
	glClearColor(0, 0, 0, 0);     // background color
	glClear(GL_COLOR_BUFFER_BIT); // clear frame buffer

	gpuProgram.setUniform(vec3(0.0f, 1.0f, 0.0f), "color"); // 3 floats
	gpuProgram.setUniform(RotationMatrix(PI / 2.0, vec3(0, 0, 1)), "MVP");

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glutSwapBuffers();
}

void onKeyboard(unsigned char key, int pX, int pY) {
	if (key == 'd') glutPostRedisplay();
}

void onMouseMotion(int pX, int pY) {
	float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;
}

void onMouse(int button, int state, int pX, int pY) {
	float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;

	char* buttonStat;
	switch (state) {
	case GLUT_DOWN:
		break;
	case GLUT_UP:
		break;
	}

	switch (button) {
	case GLUT_LEFT_BUTTON:
		break;
	case GLUT_MIDDLE_BUTTON:
		break;
	case GLUT_RIGHT_BUTTON:
		break;
	}
}

void onIdle() {
	// long time = glutGet(GLUT_ELAPSED_TIME);
}

void onKeyboardUp(unsigned char key, int pX, int pY) {}
