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

struct point
{
	double x,y,z;
};

typedef struct point point;

int length = 50;
int radius = 10;

struct point pos;

point u,r,l;

double angle = 0.02;
double cosAngle = cos(angle);
double sinAngle = sin(angle);
struct point new_l, new_r, new_u;
double norm_l, norm_r, norm_u;

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

void drawSquare(double a)
{
    glColor3f(1,1,1);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,a);
		glVertex3f( a,-a,a);
		glVertex3f(-a,-a,a);
		glVertex3f(-a, a,a);
	}glEnd();
}

void drawCylinder(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_QUADS);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i].x,points[i].y,height);
			glVertex3f(points[i+1].x,points[i+1].y,height);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawOneFourthCylinder(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*(pi/2));
        points[i].y=radius*sin(((double)i/(double)segments)*(pi/2));
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        glColor3f(0,1,0);

        glBegin(GL_QUADS);
        {
			glVertex3f(points[i].x,points[i].y,-height/2);
			glVertex3f(points[i].x,points[i].y,height/2);
			glVertex3f(points[i+1].x,points[i+1].y,height/2);
			glVertex3f(points[i+1].x,points[i+1].y,-height/2);
        }
        glEnd();
    }
}

void drawOneEightSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*(pi/2));
			points[i][j].y=r*sin(((double)j/(double)slices)*(pi/2));
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f(1,0,0);
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

void drawBottomOneEightSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*(pi/2));
			points[i][j].y=r*sin(((double)j/(double)slices)*(pi/2));
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f(1,0,0);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
		    new_l.x = l.x*cosAngle - r.x*sinAngle;
		    new_l.y = l.y*cosAngle - r.y*sinAngle;
		    new_l.z = l.z*cosAngle - r.z*sinAngle;
		    norm_l = sqrt(new_l.x*new_l.x + new_l.y*new_l.y + new_l.z*new_l.z);

		    new_r.x = r.x*cosAngle + l.x*sinAngle;
		    new_r.y = r.y*cosAngle + l.y*sinAngle;
		    new_r.z = r.z*cosAngle + l.z*sinAngle;
		    norm_r = sqrt(new_r.x*new_r.x + new_r.y*new_r.y + new_r.z*new_r.z);


		    l.x = new_l.x/norm_l;
		    l.y = new_l.y/norm_l;
		    l.z = new_l.z/norm_l;

		    r.x = new_r.x/norm_r;
		    r.y = new_r.y/norm_r;
		    r.z = new_r.z/norm_r;
			break;
		case '2':
		    new_l.x = l.x*cosAngle + r.x*sinAngle;
		    new_l.y = l.y*cosAngle + r.y*sinAngle;
		    new_l.z = l.z*cosAngle + r.z*sinAngle;
		    norm_l = sqrt(new_l.x*new_l.x + new_l.y*new_l.y + new_l.z*new_l.z);

		    new_r.x = r.x*cosAngle - l.x*sinAngle;
		    new_r.y = r.y*cosAngle - l.y*sinAngle;
		    new_r.z = r.z*cosAngle - l.z*sinAngle;
		    norm_r = sqrt(new_r.x*new_r.x + new_r.y*new_r.y + new_r.z*new_r.z);


		    l.x = new_l.x/norm_l;
		    l.y = new_l.y/norm_l;
		    l.z = new_l.z/norm_l;

		    r.x = new_r.x/norm_r;
		    r.y = new_r.y/norm_r;
		    r.z = new_r.z/norm_r;
			break;
		case '3':
		    new_l.x = l.x*cosAngle + u.x*sinAngle;
		    new_l.y = l.y*cosAngle + u.y*sinAngle;
		    new_l.z = l.z*cosAngle + u.z*sinAngle;
		    norm_l = sqrt(new_l.x*new_l.x + new_l.y*new_l.y + new_l.z*new_l.z);

		    new_u.x = u.x*cosAngle - l.x*sinAngle;
		    new_u.y = u.y*cosAngle - l.y*sinAngle;
		    new_u.z = u.z*cosAngle - l.z*sinAngle;
		    norm_u = sqrt(new_u.x*new_u.x + new_u.y*new_u.y + new_u.z*new_u.z);


		    l.x = new_l.x/norm_l;
		    l.y = new_l.y/norm_l;
		    l.z = new_l.z/norm_l;

		    u.x = new_u.x/norm_u;
		    u.y = new_u.y/norm_u;
		    u.z = new_u.z/norm_u;
			break;
		case '4':
		    new_l.x = l.x*cosAngle - u.x*sinAngle;
		    new_l.y = l.y*cosAngle - u.y*sinAngle;
		    new_l.z = l.z*cosAngle - u.z*sinAngle;
		    norm_l = sqrt(new_l.x*new_l.x + new_l.y*new_l.y + new_l.z*new_l.z);

		    new_u.x = u.x*cosAngle + l.x*sinAngle;
		    new_u.y = u.y*cosAngle + l.y*sinAngle;
		    new_u.z = u.z*cosAngle + l.z*sinAngle;
		    norm_u = sqrt(new_u.x*new_u.x + new_u.y*new_u.y + new_u.z*new_u.z);


		    l.x = new_l.x/norm_l;
		    l.y = new_l.y/norm_l;
		    l.z = new_l.z/norm_l;

		    u.x = new_u.x/norm_u;
		    u.y = new_u.y/norm_u;
		    u.z = new_u.z/norm_u;
			break;
		case '5':
		    new_u.x = u.x*cosAngle - r.x*sinAngle;
		    new_u.y = u.y*cosAngle - r.y*sinAngle;
		    new_u.z = u.z*cosAngle - r.z*sinAngle;
		    norm_u = sqrt(new_u.x*new_u.x + new_u.y*new_u.y + new_u.z*new_u.z);

		    new_r.x = r.x*cosAngle + u.x*sinAngle;
		    new_r.y = r.y*cosAngle + u.y*sinAngle;
		    new_r.z = r.z*cosAngle + u.z*sinAngle;
		    norm_r = sqrt(new_r.x*new_r.x + new_r.y*new_r.y + new_r.z*new_r.z);

		    u.x = new_u.x/norm_u;
		    u.y = new_u.y/norm_u;
		    u.z = new_u.z/norm_u;

		    r.x = new_r.x/norm_r;
		    r.y = new_r.y/norm_r;
		    r.z = new_r.z/norm_r;
			break;
		case '6':
		    new_u.x = u.x*cosAngle + r.x*sinAngle;
		    new_u.y = u.y*cosAngle + r.y*sinAngle;
		    new_u.z = u.z*cosAngle + r.z*sinAngle;
		    norm_u = sqrt(new_u.x*new_u.x + new_u.y*new_u.y + new_u.z*new_u.z);

		    new_r.x = r.x*cosAngle - u.x*sinAngle;
		    new_r.y = r.y*cosAngle - u.y*sinAngle;
		    new_r.z = r.z*cosAngle - u.z*sinAngle;
		    norm_r = sqrt(new_r.x*new_r.x + new_r.y*new_r.y + new_r.z*new_r.z);

		    u.x = new_u.x/norm_u;
		    u.y = new_u.y/norm_u;
		    u.z = new_u.z/norm_u;

		    r.x = new_r.x/norm_r;
		    r.y = new_r.y/norm_r;
		    r.z = new_r.z/norm_r;
			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			pos.x++;
			pos.y++;
			break;
		case GLUT_KEY_UP:		// up arrow key
			pos.x--;
			pos.y--;
			break;

		case GLUT_KEY_RIGHT:
			pos.x += r.x;
			pos.y += r.y;
			pos.z += r.z;
			break;
		case GLUT_KEY_LEFT:
			pos.x -= r.x;
			pos.y -= r.y;
			pos.z -= r.z;
			break;

		case GLUT_KEY_PAGE_UP:
			pos.x += u.x;
			pos.y += u.y;
			pos.z += u.z;
			break;
		case GLUT_KEY_PAGE_DOWN:
			pos.x -= u.x;
			pos.y -= u.y;
			pos.z -= u.z;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
		    if (radius < length/2)
                radius += 1;
			break;
		case GLUT_KEY_END:
		    if (radius > 0)
                radius -= 1;
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

void drawEightSpheres(){
    glPushMatrix();
    {
        glTranslatef((length/2 - radius),(length/2 - radius),(length/2 - radius));
        drawOneEightSphere(radius, 24, 20);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawOneEightSphere(radius, 24, 20);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawOneEightSphere(radius, 24, 20);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawOneEightSphere(radius, 24, 20);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef((length/2 - radius),(length/2 - radius),-(length/2 - radius));
        drawBottomOneEightSphere(radius, 24, 20);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawBottomOneEightSphere(radius, 24, 20);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawBottomOneEightSphere(radius, 24, 20);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawBottomOneEightSphere(radius, 24, 20);
    }
    glPopMatrix();
}

void drawTwelveCylinders()
{
    glPushMatrix();
    {
        glTranslatef((length/2 - radius),(length/2 - radius),0);
        drawOneFourthCylinder(radius,(length - 2*radius),80);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawOneFourthCylinder(radius,(length - 2*radius),80);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawOneFourthCylinder(radius,(length - 2*radius),80);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawOneFourthCylinder(radius,(length - 2*radius),80);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(0,-(length/2 - radius),(length/2 - radius));
        glTranslatef((length/2 - radius),(length/2 - radius),0);
        glRotatef(90,1,0,0);
        drawOneFourthCylinder(radius,(length - 2*radius),80);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawOneFourthCylinder(radius,(length - 2*radius),80);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawOneFourthCylinder(radius,(length - 2*radius),80);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawOneFourthCylinder(radius,(length - 2*radius),80);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-(length/2 - radius),0,-(length/2 - radius));
        glTranslatef((length/2 - radius),(length/2 - radius),0);
        glRotatef(90,0,1,0);
        drawOneFourthCylinder(radius,(length - 2*radius),80);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawOneFourthCylinder(radius,(length - 2*radius),80);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawOneFourthCylinder(radius,(length - 2*radius),80);

        glTranslatef(-(length - 2*radius),0,0);
        glRotatef(90,0,0,1);
        drawOneFourthCylinder(radius,(length - 2*radius),80);
    }
    glPopMatrix();
}

void drawSixSquares()
{
    glPushMatrix();
    {
        glTranslatef(0, 0, radius);
        drawSquare(length/2 - radius);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(0, 0, -radius);
        drawSquare(-(length/2 - radius));
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotatef(90,0,1,0);
        glTranslatef(0, 0, radius);
        drawSquare(length/2 - radius);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotatef(90,0,-1,0);
        glTranslatef(0, 0, radius);
        drawSquare(length/2 - radius);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotatef(90,1,0,0);
        glTranslatef(0, 0, radius);
        drawSquare(length/2 - radius);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glRotatef(90,-1,0,0);
        glTranslatef(0, 0, radius);
        drawSquare(length/2 - radius);
    }
    glPopMatrix();
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
	//gluLookAt(100*cos(cameraAngle), 100*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);

	gluLookAt(pos.x, pos.y, pos.z, pos.x+l.x, pos.y+l.y, pos.z+l.z, u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();

    drawEightSpheres();

    drawTwelveCylinders();

    drawSixSquares();


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	//angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=50.0;
	cameraAngle=1.0;
	//angle=0;

	u = point {0,0,1};
	r = point { -1.0/sqrt(2), 1.0/sqrt(2), 0 };
	l = point { -1.0/sqrt(2), -1.0/sqrt(2), 0 };

    pos = point {100, 100, 0};

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
	glutInitWindowPosition(100, 100);
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
