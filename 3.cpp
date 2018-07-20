#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

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
			glVertex3f( 150,0,0);
			glVertex3f(-150,0,0);

			glVertex3f(0,-150,0);
			glVertex3f(0, 150,0);

			glVertex3f(0,0, 150);
			glVertex3f(0,0,-150);
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

double r1 = 8;
double r2 = 8;
double r3 = 10;

double r4 = 5;

double s1 = 5;
double s2 = 3;
double s3 = 2;

double rot1 = 0.0;
double rot2 = 0.0;
double rot3 = 0.0;
double rot4 = 0.0;
double rot5 = 0.0;
double rot6 = 0.0;

void drawSS()
{
//    glColor3f(1,0,0);
//    drawSquare(20);
//
//    glRotatef(angle,0,0,1);
//    glTranslatef(110,0,0);
//    glRotatef(2*angle,0,0,1);
//    glColor3f(0,1,0);
//    drawSquare(15);
//
//    glPushMatrix();
//    {
//        glRotatef(angle,0,0,1);
//        glTranslatef(60,0,0);
//        glRotatef(2*angle,0,0,1);
//        glColor3f(0,0,1);
//        drawSquare(10);
//    }
//    glPopMatrix();
//
//    glRotatef(3*angle,0,0,1);
//    glTranslatef(40,0,0);
//    glRotatef(4*angle,0,0,1);
//    glColor3f(1,1,0);
//    drawSquare(5);


    //arekta rotate
    glRotatef(rot2,1,0,0);
    glRotatef(rot1,0,1,0);
    glTranslatef(0,0,-s1*r1);
    glScalef(1,1,s1);
    glutWireSphere(r1,12,10);
    glScalef(1,1,1.0/s1);

    glTranslatef(0,0,-s1*r1);
    glRotatef(rot3,0,1,0);
    glTranslatef(0,0,-s2*r2);

    glScalef(1,1,s2);
    glutWireSphere(r2,11,9);
    glScalef(1,1,1/s2);

    glRotatef(rot4,0,0,1);
    glBegin(GL_TRIANGLES);{
        float temp = -s2*r2;
        //float temp = 0;
        glVertex3f(0,0,temp);
        glVertex3f(0,r3,temp-r3);
        glVertex3f(0,-r3,temp-r3);
    }
    glEnd();

    glTranslatef(0,0,-s2*r2-r3);

    glPushMatrix();
    {
        glTranslatef(0,r3,0);
        glTranslatef(0,0,-r4*s3);
        glRotatef(rot5,0,0,1);
        glScalef(1,1,s3);
        glutWireSphere(r4,10,7);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(0,-r3,0);
        glRotatef(rot6,0,1,0);
        glTranslatef(0,0,-r4*s3);
        glScalef(1,1,s3);
        glutWireSphere(r4,10,7);
    }
    glPopMatrix();
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			//drawgrid=1-drawgrid;
			if(rot1 <= 40)
                rot1 += 5.0;
			break;
        case '2':
            if(rot1 >= -40)
                rot1 -= 5.0;
            break;
        case 'q':
            if(rot2 <= 85)
                rot2 += 5.0;
            break;
        case 'w':
            if(rot2 >= 5.0)
                rot2 -= 5.0;
            break;
        case '3':
			if(rot3 <= 40)
                rot3 += 5.0;
			break;
        case '4':
            if(rot3 >= 5.0)
                rot3 -= 5.0;
            break;
        case '5':
			if(rot4 <= 85)
                rot4 += 5.0;
			break;
        case '6':
            if(rot4 >= 5.0)
                rot4 -= 5.0;
            break;
        case '7':
			if(rot5 <= 85)
                rot5 += 5.0;
			break;
        case '8':
            if(rot5 >= 5.0)
                rot5 -= 5.0;
            break;
        case '9':
			if(rot6 <= 85)
                rot6 += 5.0;
			break;
        case '0':
            if(rot6 >= 5.0)
                rot6 -= 5.0;
            break;
		default:
			break;
	}
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
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
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
	gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);


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
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
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
