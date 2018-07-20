#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#define pi (2*acos(0.0))

#define sqrt2 (sqrt(2))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
int arm, rad;

typedef struct point
{
	double x,y,z;
}Point;

Point pos, u, r, l;

void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 0, 0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);
		}glEnd();
		glColor3f(0, 1.0, 0);
		glBegin(GL_LINES);{
			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);
        }glEnd();
        glColor3f(0, 0, 1.0);
        glBegin(GL_LINES);{
			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}

void drawHalfHemiSphere(double radius,int slices,int stacks)
{
    Point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2) );
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices - 0.5)*(pi/2));
			points[i][j].y=r*sin(((double)j/(double)slices - 0.5)*(pi/2));
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	glColor3f(1, 0, 0);
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}glEnd();
		}
	}
}


Point f3(Point vect, Point perp, int dir)
{
    double c = cos(pi/180);
    double s = dir * sin(pi/180);
    Point point;
    point.x = c * vect.x + s * perp.x;
    point.y = c * vect.y + s * perp.y;
    point.z = c * vect.z + s * perp.z;
    c = sqrt(point.x*point.x + point.y*point.y + point.z*point.z);
    point.x /= c;
    point.y /= c;
    point.z /= c;
    return point;
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1': {
            Point l1 = f3(l, r, -1);
            r = f3(r, l, 1);
            l = l1;
            break;
		}
		case '2': {
            Point l1 = f3(l, r, 1);
            r = f3(r, l, -1);
            l = l1;
            break;
		}

		case '3': {
            Point u1 = f3(u, l, -1);
            l = f3(l, u, 1);
            u = u1;
            break;
        }
        case '4': {
            Point u1 = f3(u, l, 1);
            l = f3(l, u, -1);
            u = u1;
            break;
        }
        case '5': {
            Point r1 = f3(r, u, -1);
            u = f3(u, r, 1);
            r = r1;
            break;
        }
        case '6':{
            Point r1 = f3(r, u, 1);
            u = f3(u, r, -1);
            r = r1;
            break;
        }
		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN: {
            pos.x -= l.x*3;
            pos.y -= l.y*3;
            pos.z -= l.z*3;
            break;
		}
		case GLUT_KEY_UP: {
            pos.x += l.x*3;
            pos.y += l.y*3;
            pos.z += l.z*3;
            break;
		}
		case GLUT_KEY_RIGHT: {
            pos.x += r.x*3;
            pos.y += r.y*3;
            pos.z += r.z*3;
            break;
		}
		case GLUT_KEY_LEFT: {
            pos.x -= r.x*3;
            pos.y -= r.y*3;
            pos.z -= r.z*3;
            break;
		}
		case GLUT_KEY_PAGE_UP: {
            pos.x += u.x*3;
            pos.y += u.y*3;
            pos.z += u.z*3;
            break;
		}
		case GLUT_KEY_PAGE_DOWN: {
            pos.x -= u.x*3;
            pos.y -= u.y*3;
            pos.z -= u.z*3;
            break;
		}
		case GLUT_KEY_HOME:
		    if(arm > 0)
            {
                arm -= 5;
                rad += 5;
            }
            break;
        case GLUT_KEY_END:
            if(rad > 0)
            {
                rad -= 5;
                arm += 5;
            }
            break;
		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;
		default:
			break;
	}
}

void drawQuarterCylinder(int slices)
{
    Point points[100];
    int i;
    for (i=0;i<=slices;i++)
    {
        points[i].x=rad*cos(((double)i/(double)slices - 0.5)*(pi/2));
        points[i].y=rad*sin(((double)i/(double)slices - 0.5)*(pi/2));
    }

    glColor3f(0, 1, 0);
    for(i=0; i<slices; i++)
    {
        //glColor3f((double)i/(double)slices,(double)i/(double)slices,(double)i/(double)slices);
        glBegin(GL_QUADS);{
            glVertex3f(points[i].x, points[i].y, arm);
            glVertex3f(points[i+1].x, points[i+1].y, arm);
            glVertex3f(points[i+1].x, points[i+1].y, -arm);
            glVertex3f(points[i].x, points[i].y, -arm);
        }glEnd();
    }
}

void f2()
{
    double d = arm * sqrt2;
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);{
        glVertex3f(d, 0, rad);
        glVertex3f(0, d, rad);
        glVertex3f(-d, 0, rad);
        glVertex3f(0, -d, rad);
    }glEnd();
}

void f()
{
    double d = arm * sqrt2;
    //glPushMatrix();
    glTranslatef(0, 0, arm);
    f2();
    //glPopMatrix();
    int i;
    glPushMatrix();
    glRotatef(45, 0, 0, 1);
    for(i=0; i<3; i++)
    {
        glRotatef(90, 0, 0, 1);
        glPushMatrix();
        glTranslatef(arm, 0, 0);
        glRotatef(-45, 0, 1, 0);
        glRotatef(90, 1, 0, 0);
        drawQuarterCylinder(30);
        glPopMatrix();
    }
    glRotatef(90, 0, 0, 1);
    glTranslatef(arm, 0, 0);
    glRotatef(-45, 0, 1, 0);
    glRotatef(90, 1, 0, 0);
    drawQuarterCylinder(30);
    glPopMatrix();
    for(i=0; i<3; i++)
    {
        glRotatef(90, 0, 0, 1);
        glPushMatrix();
        glTranslatef(d, 0, 0);
        drawHalfHemiSphere(rad, 30, 25);
        glPopMatrix();
    }
    glRotatef(90, 0, 0, 1);
    glTranslatef(d, 0, 0);
    drawHalfHemiSphere(rad, 30, 25);
}

void drawRoundCube()
{
    double d = arm * sqrt2;
    int i;
    glPushMatrix();
    for(i=0; i<3; i++)
    {
        glRotatef(90, 0, 0, 1);
        glPushMatrix();
        glTranslatef(d, 0, 0);
        drawQuarterCylinder(30);
        glPopMatrix();
    }
    glRotatef(90, 0, 0, 1);
    glTranslatef(d, 0, 0);
    drawQuarterCylinder(30);
    glPopMatrix();
    glPushMatrix();
    glRotatef(45, 0, 0, 1);
    for(i=0; i<3; i++)
    {
        glRotatef(90, 0, 0, 1);
        glPushMatrix();
        glTranslatef(arm, 0, 0);
        glRotatef(45, 1, 0, 0);
        glRotatef(90, 0, 1, 0);
        f2();
        glPopMatrix();
    }
        glRotatef(90, 0, 0, 1);
        glTranslatef(arm, 0, 0);
        glRotatef(45, 1, 0, 0);
        glRotatef(90, 0, 1, 0);
        f2();
    glPopMatrix();
    glPushMatrix();
    f();
    glPopMatrix();
    glRotatef(180, 1, 0, 0);
    f();
}

void display(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

    gluLookAt(pos.x, pos.y, pos.z, pos.x+l.x, pos.y+l.y, pos.z+l.z, u.x, u.y, u.z);

	glMatrixMode(GL_MODELVIEW);

	drawAxes();
    drawRoundCube();

	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	glutPostRedisplay();
}

void init(){
	arm = 30;
	rad = 20;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;

	glClearColor(0,0,0,0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
    gluPerspective(80,	1,	1,	1000.0);
}

int main(int argc, char **argv){
    double d = 1 / sqrt2;
    pos = {100, 100, 100};
    u = {0, 0, 1};
    r = {-d, d, 0};
    l = {-d, -d, 0};
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Rounded Cube");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
