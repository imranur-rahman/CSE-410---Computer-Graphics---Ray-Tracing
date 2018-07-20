#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <glut.h>

#include "base.hpp"
#include "bitmap_image.hpp"

using namespace std;

#define pi (2*acos(0.0))

#define windowWidth 500
#define windowHeight 500
#define fov 80

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

const float max_a = 40.0;
float rad = 0.0;
float a = max_a;
float amountToBeIncreased = 0.4;

void capture();

int imageWidth, imageHeight;
int recursion_level;

point pos, u, r, l;

vector<object*> objects;
vector<point> lights; //actually not point, vector

point crossProduct(point a, point b)
{
    point ret;
    ret.x = a.y * b.z - a.z * b.y;
    ret.y = a.z * b.x - a.x * b.z;
    ret.z = a.x * b.y - a.y * b.x;
    return ret;
}

double dotProduct(point a, point b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
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
        case '0':
            capture();
            break;
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

void draw()
{
    for(int i = 0; i < objects.size(); ++i)
        objects[i]->draw();
    for(int i = 0; i < lights.size(); ++i)
    {
        glPushMatrix();
        glTranslatef(lights[i].x, lights[i].y, lights[i].z);
        drawSphere(0.5);
        glPopMatrix();
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

    //drawMixture();
    draw();

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

	u = {0, 0, 1};
	r = {1, 0, 0};
	l = {0, 1, 0};
	pos = {0, -100, 10};

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

void loadTestData()
{
    imageWidth = imageHeight = 768;
    recursion_level = 3;

    point center(0,0,10);
    double radius = 10;
    object *temp;

	temp=new sphere(center, radius); // Center(0,0,10), Radius 10
	temp->setColor(1,0,0);
	temp->setCoEfficients(0.4,0.2,0.2,0.2);
	temp->setShine(1);

	objects.push_back(temp);

	temp=new sphere({10,40,0}, radius); // Center(0,0,10), Radius 10
	temp->setColor(0,1,0);
	temp->setCoEfficients(0.4,0.2,0.2,0.2);
	temp->setShine(1);

	objects.push_back(temp);

	point light1(-50,50,50);
	lights.push_back(light1);

	//object *t = new floor()
	temp=new Floor(1000, 20);
	temp->setCoEfficients(0.4,0.2,0.2,0.2);
	temp->setShine(1);
	objects.push_back(temp);
}

void capture()
{
    point** frameBuffer;
    frameBuffer = new point* [imageWidth];

    for (int i = 0; i < imageWidth; i++) {
        frameBuffer[i] = new point [imageHeight];
        for (int j = 0; j < imageHeight; j++) {
            frameBuffer[i][j] = {0, 0, 0};
        }
    }

    double planeDistance = (windowHeight/2.0)/tan(fov*pi/360.0);

    point topLeft = pos + l * planeDistance - r * windowWidth / 2.0 + u * windowHeight / 2.0;
    cout << topLeft;

    double du = (windowWidth * 1.0) / imageWidth;
    double dv = (windowHeight * 1.0) / imageHeight;

    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageHeight; j++) {

            point cornerDir = topLeft + r*j*du - u*i*dv;
            //cout<<cornerDir;

            Ray r(pos, cornerDir - pos);

            int nearest=-1;
            double minT = 9999999;
            double color[3];

            for (int k = 0; k < objects.size(); k++) {

                double t = objects[k]->intersect(&r, color, 0);

                if(t <= 0)
                    continue;
                else if (t < minT) {
                    minT = t;
                    nearest = k;
                }

            }

            if(nearest!=-1) {

                double t = objects[nearest]->intersect(&r, color, 1);

                frameBuffer[i][j] = point(color);

            }
        }
    }


    bitmap_image image(imageWidth, imageHeight);

    for (int i=0; i<imageWidth; i++) {
        for (int j=0; j<imageHeight; j++) {
            image.set_pixel(j, i, frameBuffer[i][j].x*255, frameBuffer[i][j].y*255, frameBuffer[i][j].z*255);
        }
    }

    image.save_image("output.bmp");
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

	loadTestData();

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
