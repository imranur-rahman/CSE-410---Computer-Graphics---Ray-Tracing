#include<bits/stdc++.h>
#include <windows.h>
#include <glut.h>

using namespace std;

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

struct point
{
	double x,y,z;
};


void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

double a = 120;
point pos;
point v;
double f1 = 10.0;
double f2 = 0.05;
point perp;
double ang = 10.0;
double f3 = 3.0;


void drawSS()
{
    glBegin(GL_LINES);
    {
        glVertex3f(a,a,0);
        glVertex3f(a,-a,0);
    }
    glEnd();

    glBegin(GL_LINES);
    {
        glVertex3f(a,a,0);
        glVertex3f(-a,a,0);
    }
    glEnd();

    glBegin(GL_LINES);
    {
        glVertex3f(-a,a,0);
        glVertex3f(-a,-a,0);
    }
    glEnd();

    glBegin(GL_LINES);
    {
        glVertex3f(-a,-a,0);
        glVertex3f(a,-a,0);
    }
    glEnd();

    point end = {pos.x + v.x * f1, pos.y + v.y * f1, pos.z + v.z * f1};

    glBegin(GL_LINES);
    {
        glVertex3f(pos.x, pos.y, pos.z);
        glVertex3f(end.x, end.y, end.z);
    }
    glEnd();

    point perpVector = {-v.y, v.x, v.z};

    glBegin(GL_TRIANGLES);
    {
        //glColor3f(1,0,0);
        glVertex3f(end.x, end.y, end.z);
        glVertex3f(end.x - v.x * f3 + perpVector.x * f3, end.y - v.y * f3 + perpVector.y * f3, end.z - v.z * f3 + perpVector.z * f3);
        glVertex3f(end.x - v.x * f3 - perpVector.x * f3, end.y - v.y * f3 - perpVector.y * f3, end.z - v.z * f3 - perpVector.z * f3);
    }
    glEnd();
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			drawgrid=1-drawgrid;
			break;

		default:
			break;
	}
}

double toRad(double f)
{
    return 2.0 * pi * f;
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 3.0;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;

            v.x = v.x * cos(pi/60.0) - v.y * sin(pi/60.0);
            v.y = v.y * cos(pi/60.0) + v.x * sin(pi/60.0);

			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;

            v.x = v.x * cos(-pi/60.0) - v.y * sin(-pi/60.0);
            v.y = v.y * cos(-pi/60.0) + v.x * sin(-pi/60.0);

			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
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

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	pos.x += v.x * f2;
	pos.y += v.y * f2;
	pos.z += v.z * f2;

	if(pos.x >= a  ||  pos.x <= -a)
    {
        v.x = -v.x;
    }

    if(pos.y >= a  ||  pos.y <= -a)
    {
        v.y = -v.y;
    }

	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=0;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance

	pos.x = 0.0;
    pos.y = 0.0;
    pos.z = 0.0;

    v.x = 1.0/sqrt(2.0);
    v.y = 1.0/sqrt(2.0);
    v.z = 0.0;

}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

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
