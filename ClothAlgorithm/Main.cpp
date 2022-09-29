#include <math.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include "include/GL/glut.h"
#include "3DimensionVector.h"
#include "ClothCompose.h"


Cloth drap(15, 10, 55, 50); // 천
Vec3 ball_pos(7, -5, 0); // 공
Vec3 cube_pos(12, -5, 0); // 큐브
float ball_radius = 2;
float ball_time = 0; // 공의 z값 계산
float cube_size = 2.0;
int ball = 0;
float density = 0.03;

void drawRepere() {
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.2, 0.0, 0.0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.2, 0.0);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.2);
	glEnd();
	glPopMatrix();
}

void init(void) {
	glShadeModel(GL_SMOOTH);
	glClearColor(0.2f, 0.2f, 0.4f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_COLOR_MATERIAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat lightPos[4] = { -1.0,1.0,0.5,0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, (GLfloat*)&lightPos);

	glEnable(GL_LIGHT1);

	GLfloat lightAmbient1[4] = { 0.0,0.0,0.0,0.0 };
	GLfloat lightPos1[4] = { 1.0,0.0,-0.2,0.0 };
	GLfloat lightDiffuse1[4] = { 0.5,0.5,0.3,0.0 };

	glLightfv(GL_LIGHT1, GL_POSITION, (GLfloat*)&lightPos1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, (GLfloat*)&lightAmbient1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, (GLfloat*)&lightDiffuse1);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	/*안개*/
	GLfloat fogColor[4] = { 0.5, 0.5, 0.5, 1.0 };
	glEnable(GL_FOG);

	glFogi(GL_FOG_MODE, GL_EXP2);

	glFogfv(GL_FOG_COLOR, fogColor);

	glFogf(GL_FOG_DENSITY, density);

	glHint(GL_FOG_HINT, GL_NICEST);


}


float x = 0, y = 0, z = 0, r = 25;
float* px = &x, * py = &y, * pz = &z, * pr = &r;


void draw(void) {

	ball_time++;
	ball_pos.f[2] = cos(ball_time / 50.0) * 7;


	drap.addForce(Vec3(0, -0.2, 0) * TIME_STEPSIZE2); 
	drap.windForce(Vec3(0.5, 0, 0.2) * TIME_STEPSIZE2); //위는 중력 이거는 바람
	drap.timeStep();
	if (ball == 1) {
		drap.ballCollision(ball_pos, ball_radius);

	}

	drap.cubeCollision(cube_pos, cube_size, cube_pos);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-200.0f, -100.0f, -100.0f);
	glVertex3f(200.0f, -100.0f, -100.0f);
	glColor3f(0.4f, 0.4f, 0.8f);
	glVertex3f(200.0f, 100.0f, -100.0f);
	glVertex3f(-200.0f, 100.0f, -100.0f);
	glEnd();
	glEnable(GL_LIGHTING);

	glTranslatef(-6.5 + x, 6 + y, -11.0f + z); // 잘보이게하기 위해서
	glRotatef(r, 0, 1, 0);
	drap.drawShaded();

	glPushMatrix();
	glTranslatef(ball_pos.f[0], ball_pos.f[1], ball_pos.f[2]);
	glColor3f(0.6f, 0.19f, 0.8f);
	if (ball == 1) {
		glutSolidSphere(ball_radius - 0.1, 50, 50);
	}
	glPopMatrix();
	glPushMatrix();
	glTranslatef(cube_pos.f[0], cube_pos.f[1], cube_pos.f[2]);
	glColor3f(0.27f, 0.5f, 0.7f);
	glutSolidCube(1.7 * cube_size);
	glPopMatrix();
	glutSwapBuffers();
	glutPostRedisplay();
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h == 0)
		gluPerspective(80, (float)w, 1.0, 5000.0);
	else
		gluPerspective(80, (float)w / (float)h, 1.0, 5000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':    
		exit(0);
		break;
	case 'f':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
		glutPostRedisplay();
		break;
	case 'l':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
		glutPostRedisplay();
		break;
	case 'p':
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		glutPostRedisplay();
		break;
	case 'b':
		if (ball == 0) {
			ball = 1;
		}
		else {
			ball = 0;
		}
		glutPostRedisplay();
		break;
		/*밑에는 카메라 조정*/
	case 'x':
		*px = *px + 0.2;
		glutPostRedisplay();
		break;
	case 'X':
		*px = *px - 0.2;
		glutPostRedisplay();
		break;
	case 'y':
		*py = *py + 0.2;
		glutPostRedisplay();
		break;
	case 'Y':
		*py = *py - 0.2;
		glutPostRedisplay();
		break;
	case 'z':
		*pz = *pz + 0.2;
		glutPostRedisplay();
		break;
	case 'Z':
		*pz = *pz - 0.2;
		glutPostRedisplay();
		break;
	case 'r':
		*pr = *pr + 5;
		glutPostRedisplay();
		break;
	case 'R':
		*pr = *pr - 5;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void arrow_keys(int a_keys, int x, int y) {
	switch (a_keys) {
	case GLUT_KEY_UP:
		glutFullScreen();
		break;
	case GLUT_KEY_DOWN:
		glutReshapeWindow(1000, 700);
		break;
	default:
		break;
	}
}


int main() {
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1000, 700);

	glutCreateWindow("천 알고리즘");
	init();
	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(arrow_keys);

	glutMainLoop();
}