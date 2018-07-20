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

const float max_a = 40.0;
float rad = 0.0;
float a = max_a;
float amountToBeIncreased = 0.4;

struct point
{
	double x,y,z;
};

point pos, u, r, l;


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

void drawSquare(double a)
{
    glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}


void drawOneForthCylinder(double radius,double height,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*0.5*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*0.5*pi);
    }
    glColor3f(0,1,0);
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        //circle duita fillup hocche na , and etai amader dorkar
        glBegin(GL_QUADS);
        {
			glVertex3f(points[i].x,points[i].y,height/2.0);
			glVertex3f(points[i+1].x,points[i+1].y,height/2.0);
			glVertex3f(points[i+1].x,points[i+1].y,-height/2.0);
            glVertex3f(points[i].x,points[i].y,-height/2.0);
        }
        glEnd();
    }
}


void drawOneEighthSphere(double radius,int slices,int stacks,int isUpper)
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
			points[i][j].x=r*cos(((double)j/(double)slices)*0.5*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*0.5*pi);
			points[i][j].z=h;
		}
	}
	//glColor3f(1,1,0);
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    if(isUpper == 1) {
                    //upper hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			    }
			    else {
                    //lower hemisphere
                    glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                    glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                    glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                    glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			    }
			}glEnd();
		}
	}
}


void drawAllSphere()
{
    int t = a; //translation amount
    //int r = 30; //radius

    int rot = 90; //rotation angle

    int slices = 24;
    int stacks = 20;

    for(int isLower = -1; isLower <= 1; isLower += 2)
    {
        glPushMatrix();
        glTranslatef(t,t,isLower*t); //isLower er jaygay - chilo
        drawOneEighthSphere(rad,slices,stacks,isLower);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(t,-t,isLower*t);
        glRotatef(-rot,0,0,1);
        drawOneEighthSphere(rad,slices,stacks,isLower);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-t,-t,isLower*t);
        glRotatef(-2*rot,0,0,1);
        drawOneEighthSphere(rad,slices,stacks,isLower);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-t,t,isLower*t);
        glRotatef(-3*rot,0,0,1);
        drawOneEighthSphere(rad,slices,stacks,isLower);
        glPopMatrix();
    }

}

void drawAllCylinder()
{
    int t = a; //translation amount
    //int r = 20; //radius

    int rot = 90; //rotation angle

    int height = 2 * a;
    int segments = 20;

    //x, y er dike translate lagbe, z er dike lagbe na.

    glPushMatrix();
    glTranslatef(t,t,0);
    drawOneForthCylinder(rad,height,segments);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(t,-t,0);
    glRotatef(-rot,0,0,1);
    drawOneForthCylinder(rad,height,segments);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-t,-t,0);
    glRotatef(-2*rot,0,0,1);
    drawOneForthCylinder(rad,height,segments);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-t,t,0);
    glRotatef(-3*rot,0,0,1);
    drawOneForthCylinder(rad,height,segments);
    glPopMatrix();



    glPushMatrix();
    glRotatef(-45,0,1,0);
    glRotatef(90,1,0,0);
    glRotatef(-45,0,0,1);
    glTranslatef(t,t,0);
    drawOneForthCylinder(rad,height,segments);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-rot,0,0,1);
    glRotatef(-45,0,1,0);
    glRotatef(90,1,0,0);
    glRotatef(-45,0,0,1);
    glTranslatef(t,t,0);
    drawOneForthCylinder(rad,height,segments);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-2*rot,0,0,1);
    glRotatef(-45,0,1,0);
    glRotatef(90,1,0,0);
    glRotatef(-45,0,0,1);
    glTranslatef(t,t,0);
    drawOneForthCylinder(rad,height,segments);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-3*rot,0,0,1);
    glRotatef(-45,0,1,0);
    glRotatef(90,1,0,0);
    glRotatef(-45,0,0,1);
    glTranslatef(t,t,0);
    drawOneForthCylinder(rad,height,segments);
    glPopMatrix();


    glPushMatrix();
    glRotatef(45,0,1,0);
    glRotatef(90,1,0,0);
    glRotatef(-45,0,0,1);
    glTranslatef(t,t,0);
    drawOneForthCylinder(rad,height,segments);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-rot,0,0,1);
    glRotatef(45,0,1,0);
    glRotatef(90,1,0,0);
    glRotatef(-45,0,0,1);
    glTranslatef(t,t,0);
    drawOneForthCylinder(rad,height,segments);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-2*rot,0,0,1);
    glRotatef(45,0,1,0);
    glRotatef(90,1,0,0);
    glRotatef(-45,0,0,1);
    glTranslatef(t,t,0);
    drawOneForthCylinder(rad,height,segments);
    glPopMatrix();

    glPushMatrix();
    glRotatef(-3*rot,0,0,1);
    glRotatef(45,0,1,0);
    glRotatef(90,1,0,0);
    glRotatef(-45,0,0,1);
    glTranslatef(t,t,0);
    drawOneForthCylinder(rad,height,segments);
    glPopMatrix();

}

void drawAllSquare()
{
    int t = a;

    glPushMatrix();
    glTranslatef(a+rad,0,0);
    glRotatef(90,0,1,0);
    drawSquare(a);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(a+rad),0,0);
    glRotatef(90,0,1,0);
    drawSquare(a);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,a+rad,0);
    glRotatef(90,1,0,0);
    drawSquare(a);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,-(a+rad),0);
    glRotatef(90,1,0,0);
    drawSquare(a);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0,a+rad);
    drawSquare(a);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0,-(a+rad));
    drawSquare(a);
    glPopMatrix();
}

void drawMixture()
{
    drawAllSphere();
    drawAllCylinder();
    drawAllSquare();
}

point crossProduct(point a, point b)
{
    point ret;
    ret.x = a.y * b.z - a.z * b.y;
    ret.y = a.z * b.x - a.x * b.z;
    ret.z = a.x * b.y - a.y * b.x;
    return ret;
}

void update(point *toupdate, point *by, double angle)
{
    toupdate->x = toupdate->x * cos(angle) + by->x * sin(angle);
    toupdate->y = toupdate->y * cos(angle) + by->y * sin(angle);
    toupdate->z = toupdate->z * cos(angle) + by->z * sin(angle);
}


point t1, t2;

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			t1 = crossProduct(u, l);
			t2 = crossProduct(u, r);

//			l.x = l.x * cos(pi/60.0) + t1.x * sin(pi/60.0);
//			l.y = l.y * cos(pi/60.0) + t1.y * sin(pi/60.0);
//			l.z = l.z * cos(pi/60.0) + t1.z * sin(pi/60.0);
//
//			r.x = r.x * cos(pi/60.0) + t2.x * sin(pi/60.0);
//			r.y = r.y * cos(pi/60.0) + t2.y * sin(pi/60.0);
//			r.z = r.z * cos(pi/60.0) + t2.z * sin(pi/60.0);

            update(&l, &t1, pi/60.0);
            update(&r, &t2, pi/60.0);

			break;
        case '2':
            t1 = crossProduct(u, l);
			t2 = crossProduct(u, r);
			update(&l, &t1, -pi/60.0);
            update(&r, &t2, -pi/60.0);
            break;
        case '3':
            t1 = crossProduct(r, l);
			t2 = crossProduct(r, u);
			update(&l, &t1, pi/60.0);
            update(&u, &t2, pi/60.0);
            break;
        case '4':
            t1 = crossProduct(r, l);
			t2 = crossProduct(r, u);
			update(&l, &t1, -pi/60.0);
            update(&u, &t2, -pi/60.0);
            break;
        case '5':
            t1 = crossProduct(l, u);
			t2 = crossProduct(l, r);
			update(&u, &t1, pi/60.0);
            update(&r, &t2, pi/60.0);
            break;
        case '6':
            t1 = crossProduct(l, u);
			t2 = crossProduct(l, r);
			update(&u, &t1, -pi/60.0);
            update(&r, &t2, -pi/60.0);
            break;
		default:
			break;
	}
}

double f1 = 5.0;


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 3.0;

			pos.x = pos.x - l.x * f1;
			pos.y = pos.y - l.y * f1;
			pos.z = pos.z - l.z * f1;

			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 3.0;

			pos.x = pos.x + l.x * f1;
			pos.y = pos.y + l.y * f1;
			pos.z = pos.z + l.z * f1;

			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;

            pos.x = pos.x + r.x * f1;
			pos.y = pos.y + r.y * f1;
			pos.z = pos.z + r.z * f1;

			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;

			pos.x = pos.x - r.x * f1;
			pos.y = pos.y - r.y * f1;
			pos.z = pos.z - r.z * f1;

			break;

		case GLUT_KEY_PAGE_UP:

		    pos.x = pos.x + u.x * f1;
			pos.y = pos.y + u.y * f1;
			pos.z = pos.z + u.z * f1;

			break;
		case GLUT_KEY_PAGE_DOWN:

		    pos.x = pos.x - u.x * f1;
			pos.y = pos.y - u.y * f1;
			pos.z = pos.z - u.z * f1;

			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
		    if(a < max_a - amountToBeIncreased)
            {
                a += amountToBeIncreased;
                rad -= amountToBeIncreased;
            }
			break;
		case GLUT_KEY_END:
		    //if(2 * r < max_a)
		    if(a > amountToBeIncreased)
            {
                rad += amountToBeIncreased;
                a -= amountToBeIncreased;
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
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);

	gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);


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

    //drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);

    drawMixture();

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

	u = {0,0,1};
	r = {-1.0/sqrt(2), 1.0/sqrt(2), 0};
	l = {-1.0/sqrt(2), -1.0/sqrt(2), 0};
	pos = {100,100,0};

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
