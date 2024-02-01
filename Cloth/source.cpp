#include <iostream>
#include "include/GL/glut.h"
#include <ctime>
#include "PBD_Cloth.h"
#include "time.h"

int width = 600;
int height = 600;
float zoom = 15.0f;
float rotx = 0;
float roty = 0.001f;
float tx = 0;
float ty = 0;
int lastx = 0;
int lasty = 0;
unsigned char Buttons[3] = { 0 };
bool simulation = false;

PBD_Cloth* _pbd;



void Init(void)
{
	glEnable(GL_DEPTH_TEST);
	_pbd = new PBD_Cloth(64, 64);
}

void Darw(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	_pbd->draw();
	glDisable(GL_LIGHTING);
}

void Update(void)
{
	if (simulation) {
		_pbd->simulation(0.01);
	}
	::glutPostRedisplay();
}


void Display(void)
{
	glClearColor(0.8980392156862745f, 0.9490196078431373f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	zoom = 5.699999;
	tx = 0.350000;
	ty = 1.500000;
	rotx = 15.500000;
	roty = -45.499001;

	glTranslatef(0, 0, -zoom);
	glTranslatef(tx, ty, 0);
	glRotatef(rotx, 1, 0, 0);
	glRotatef(roty, 0, 1, 0);

	//printf("%f, %f, %f, %f, %f\n", -zoom, tx, ty, rotx, roty);

	Darw();

	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	if (w == 0) {
		h = 1;
	}
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w / h, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Motion(int x, int y)
{
	int diffx = x - lastx;
	int diffy = y - lasty;
	lastx = x;
	lasty = y;

	if (Buttons[2]) {
		zoom -= (float) 0.05f * diffx;
	}
	else if (Buttons[0]) {
		rotx += (float) 0.5f * diffy;
		roty += (float) 0.5f * diffx;
	}
	else if (Buttons[1]) {
		tx += (float) 0.05f * diffx;
		ty -= (float) 0.05f * diffy;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	lastx = x;
	lasty = y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		Buttons[0] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_MIDDLE_BUTTON:
		Buttons[1] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_RIGHT_BUTTON:
		Buttons[2] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void SpecialInput(int key, int x, int y)
{
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
	case 'Q':
		exit(0);
	case 'w':
	case 'W':
		_pbd->wind(0.03);
		break;
	case 'r':
	case 'R':
		_pbd->init();
		break;
	case ' ':
		simulation = !simulation;
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Adv. Computer Graphics : Position Based Dynamics");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Update);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialInput);
	Init();
	glutMainLoop();
}