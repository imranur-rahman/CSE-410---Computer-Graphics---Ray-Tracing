#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<bits/stdc++.h>
#include <windows.h>
#include <glut.h>
#include <iostream>

using namespace std;

#define pi (2*acos(0.0))
#define pCos cos(pi/90)
#define nCos cos(-pi/90)
#define pSin sin(pi/90)
#define nSin sin(-pi/90)
#define MAX_VAL 9999999
#define MIN_VAL -9999999

#include "bitmap_image.hpp"
#include "classes.hpp"

#define WINDOW_WIDTH 500.0
#define WINDOW_HEIGHT 500.0

double fovY;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
int imageWidth;
int imageHeight;
double tempVal;

int recursion_level;
vector <Object*>  objects;
vector <Point> lights;


Point pos(0, -100, 10);
Point u(0,0,1);
Point r(1, 0, 0);
Point l(0, 1, 0);
Point temp(0,0,0);

double thick = 0.5;

void drawAxes()
{
	if(drawaxes==1)
	{

		glBegin(GL_LINES);{
		    glColor3f(1.0, 0.0, 0.0);
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

            glColor3f(0.0, 1.0, 0.0);
			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glColor3f(0.0, 0.0, 1.0);
			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}



void drawSquare(double a, double z)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,z);
		glVertex3f( a,-a,z);
		glVertex3f(-a,-a,z);
		glVertex3f(-a, a,z);
	}glEnd();
}

void drawPoint(Point pt) {

    glColor3f(1.0, 1.0, 0.0);

    glBegin(GL_QUADS);
    {
        glVertex3f(pt.x+thick, pt.y, pt.z+thick);
        glVertex3f(pt.x+thick, pt.y, pt.z-thick);
        glVertex3f(pt.x-thick, pt.y, pt.z-thick);
        glVertex3f(pt.x-thick, pt.y, pt.z+thick);
    }
    glEnd();



}

void loadTestData(){
    Object *temp;
    imageWidth = 768;
    imageHeight = 768;
    recursion_level = 4;

    temp= new Sphere("shpere",Point(20,0,10), 10); // Center(0,0,10), Radius 10
    temp->setColor(1,0.0,0.0);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    objects.push_back(temp);
    Point center(13,15,15);
    double radius = 5;

    temp = new Sphere("shpere",center, radius);
    temp->setColor(1,0,1);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    objects.push_back(temp);
    center = Point(-20,20,20);
    radius = 20;
    temp = new Sphere("shpere",center, radius);
    temp->setColor(1,1,0);
    temp->setCoEfficients(0.5,0.2,0.4,0.4);
    temp->setShine(1);
    objects.push_back(temp);

    Point light1(-50,50,50);
    lights.push_back(light1);
    Point light2(30,-30,50);
    lights.push_back(light2);

    temp=new Floor("floor",1000, 20);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    objects.push_back(temp);

    Point a(10,0,10);
    Point b(50,0,50);
    Point c(80,10,5);
    temp = new Triangle("shpere",a,b,c);
    temp->setColor(0,0,1);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    objects.push_back(temp);

    double coeff[] = {1, 1, 1, 0, 0, 0, -20, -20, -20, 200};
    Point reff(0, 0, 0);

    temp = new GeneralQuadratic("shpere",coeff, reff, 0, 30, 5);
    temp->setColor(1, 0, 0);
    temp->setCoEfficients(0.4, 0.2, 0.1, 0.3);
    temp->setShine(3);
    objects.push_back(temp);



}

void loadActualData(){

    freopen("scene.txt","r",stdin);
    cin >> recursion_level;
    cin >> imageWidth;
    imageHeight = imageWidth;

    int object_nums;
    cin >> object_nums;

    Object *temp;
    int refract_count = 0;
    for(int i=0;i<object_nums;i++){
        string object_type;
        cin >> object_type;
        if(object_type == "sphere"){
            double a,b,c,r;
            cin >> a >> b >> c >> r;
            temp = new Sphere(object_type,Point(a,b,c), r);

            cin >> a >> b >> c;
            temp->setColor(a,b,c);

            cin >> a >> b >> c >> r;
            temp->setCoEfficients(a,b,c,r);

            cin >> a;
            temp->setShine(a);
            if(refract_count < 3) {
                    temp->isRefract = true;
                    refract_count++;
            }

        }
        else if(object_type == "triangle"){
            double a,b,c,r;

            cin >> a >> b >> c;
            Point p1(a,b,c);

            cin >> a >> b >> c;
            Point p2(a,b,c);

            cin >> a >> b >> c;
            Point p3(a,b,c);

            temp = new Triangle(object_type,p1, p2, p3);

            cin >> a >> b >> c;
            temp->setColor(a,b,c);

            cin >> a >> b >> c >> r;
            temp->setCoEfficients(a,b,c,r);

            cin >> a;
            temp->setShine(a);

        }
        else if(object_type == "general"){
            double a,b,c, r,coff[10];
            for(int j=0;j<10;j++){
                cin >> coff[j];
            }
            cin >> a >> b >> c;
            Point reff(a,b,c);
            cin >> a >> b >> c;
            temp = new GeneralQuadratic(object_type,coff, reff, a, b, c);

            cin >> a >> b >> c;
            temp->setColor(a,b,c);

            cin >> a >> b >> c >> r;
            temp->setCoEfficients(a,b,c,r);

            cin >> a;
            temp->setShine(a);

        }

        objects.push_back(temp);

    }
    int light_nums;
    cin >> light_nums;
    for(int i=0; i<light_nums;i++){
        double a,b,c;
        cin>>a>>b>>c;
        Point light(a,b,c);
        lights.push_back(light);
    }

    temp=new Floor("floor",1000, 20);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    objects.push_back(temp);
}


void capture(){
    // init image to black
    //double start_time = timeGetTime();

    bitmap_image image(imageWidth, imageHeight);
    for(int i=0;i<imageWidth;i++){
        for(int j=0;j<imageHeight;j++){
            image.set_pixel(i, j, 0, 0, 0);
        }
    }

    // Now other things
    double plane_distance = (WINDOW_HEIGHT/2.0)/tan(fovY*pi/360);

    Point topLeft = pos + l*plane_distance - r * (WINDOW_WIDTH/2.0) + u * (WINDOW_HEIGHT/2.0);
    cout<<"topLeft: "<<topLeft<<endl;

    double du = WINDOW_WIDTH/imageWidth;
    double dv = WINDOW_HEIGHT/imageHeight;

    cout<<"du = "<<du<<", dv = "<<dv<<endl;

    cout<< "objects: "<<objects.size()<<endl;
    for(int i=0; i<imageWidth;i++){
        for(int j=0;j<imageHeight;j++){
            Point corner = topLeft + r*j*du - u*i*dv;
            //cout<<"Corner = "<<corner<<endl;

            Ray ray(pos, (corner-pos));
            int nearest = -1;
            double t_min = MAX_VAL;
            double colorAt[3];

            for(int k=0;k<objects.size();k++){
                double t = objects[k]->intersect(&ray, colorAt, 0);
                //cout << t <<endl;
                if(t <=0) continue;
                if(t < t_min){
                    t_min = t;
                    nearest = k;
                }
            }
            if(nearest != -1){
                double t = objects[nearest]->intersect(&ray, colorAt, 1);
                //cout << "t: "<<t<<endl;
                //cout << colorAt[0] << ", "<<colorAt[1]<<", "<<colorAt[2]<<endl;
                image.set_pixel(j,i, colorAt[0]*255, colorAt[1]*255, colorAt[2]*255);
            }

        }
    }
    image.save_image("out.bmp");

    cout<<"finished Capture"<<endl;
}


void freeMemory(){
    vector<Object*>().swap(objects);
    vector<Point>().swap(lights);
}


void keyboardListener(unsigned char key, int x,int y){

	switch(key){
        case '0':
            capture();
            break;
		case '1':


            temp.x = l.x*pCos - r.x*pSin;
            temp.y = l.y*pCos - r.y*pSin;
            tempVal = sqrt(temp.x*temp.x + temp.y*temp.y + temp.z*temp.z);
            temp.x /= tempVal;
            temp.y /= tempVal;

            r.x = r.x*pCos + l.x*pSin;
            r.y = r.y*pCos + l.y*pSin;
            tempVal = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);
            r.x /= tempVal;
            r.y /= tempVal;
            l.x = temp.x;
            l.y = temp.y;

			break;
        case '2':

            temp.x = l.x*nCos - r.x*nSin;
            temp.y = l.y*nCos - r.y*nSin;
            tempVal = sqrt(temp.x*temp.x + temp.y*temp.y + temp.z*temp.z);
            temp.x /= tempVal;
            temp.y /= tempVal;

            r.x = r.x*nCos + l.x*nSin;
            r.y = r.y*nCos + l.y*nSin;
            tempVal = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);
            r.x /= tempVal;
            r.y /= tempVal;
            l.x = temp.x;
            l.y = temp.y;
			break;
        case '3':
            temp.x = l.x*pCos + u.x*pSin;
            temp.y = l.y*pCos + u.y*pSin;
            temp.z = l.z*pCos + u.z*pSin;
            tempVal = sqrt(temp.x*temp.x + temp.y*temp.y + temp.z*temp.z);
            temp.x /= tempVal;
            temp.y /= tempVal;
            temp.z /= tempVal;

            u.x = u.x*pCos - l.x*pSin;
            u.y = u.y*pCos - l.y*pSin;
            u.z = u.z*pCos - l.z*pSin;
            tempVal = sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
            u.x /= tempVal;
            u.y /= tempVal;
            u.z /= tempVal;
            l.x = temp.x;
            l.y = temp.y;
            l.z = temp.z;

			break;
        case '4':
            temp.x = l.x*nCos + u.x*nSin;
            temp.y = l.y*nCos + u.y*nSin;
            temp.z = l.z*nCos + u.z*nSin;
            tempVal = sqrt(temp.x*temp.x + temp.y*temp.y + temp.z*temp.z);
            temp.x /= tempVal;
            temp.y /= tempVal;
            temp.z /= tempVal;

            u.x = u.x*nCos - l.x*nSin;
            u.y = u.y*nCos - l.y*nSin;
            u.z = u.z*nCos - l.z*nSin;
            tempVal = sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
            u.x /= tempVal;
            u.y /= tempVal;
            u.z /= tempVal;
            l.x = temp.x;
            l.y = temp.y;
            l.z = temp.z;

			break;
        case '5':
            temp.x = r.x*pCos - u.x*pSin;
            temp.y = r.y*pCos - u.y*pSin;
            temp.z = r.z*pCos - u.z*pSin;
            tempVal = sqrt(temp.x*temp.x + temp.y*temp.y + temp.z*temp.z);
            temp.x /= tempVal;
            temp.y /= tempVal;
            temp.z /= tempVal;

            u.x = u.x*pCos + r.x*pSin;
            u.y = u.y*pCos + r.y*pSin;
            u.z = u.z*pCos + r.z*pSin;
            tempVal = sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
            u.x /= tempVal;
            u.y /= tempVal;
            u.z /= tempVal;
            r.x = temp.x;
            r.y = temp.y;
            r.z = temp.z;
			break;
        case '6':
            temp.x = r.x*nCos - u.x*nSin;
            temp.y = r.y*nCos - u.y*nSin;
            temp.z = r.z*nCos - u.z*nSin;
            tempVal = sqrt(temp.x*temp.x + temp.y*temp.y + temp.z*temp.z);
            temp.x /= tempVal;
            temp.y /= tempVal;
            temp.z /= tempVal;


            u.x = u.x*nCos + r.x*nSin;
            u.y = u.y*nCos + r.y*nSin;
            u.z = u.z*nCos + r.z*nSin;
            tempVal = sqrt(u.x*u.x + u.y*u.y + u.z*u.z);
            u.x /= tempVal;
            u.y /= tempVal;
            u.z /= tempVal;
            r.x = temp.x;
            r.y = temp.y;
            r.z = temp.z;

			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			pos.x -= 2.0*l.x;
			pos.y -= 2.0*l.y;

			break;
		case GLUT_KEY_UP:		// up arrow key
			pos.x += 2.0*l.x;
			pos.y += 2.0*l.y;
			break;

		case GLUT_KEY_RIGHT:
			pos.x += 2.0*r.x;
			pos.y += 2.0*r.y;
			break;
		case GLUT_KEY_LEFT:
			pos.x -= 2.0*r.x;
			pos.y -= 2.0*r.y;
			break;

		case GLUT_KEY_PAGE_UP:
		    pos.z += 2.0;
			break;
		case GLUT_KEY_PAGE_DOWN:
		    pos.z -= 2.0;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
           /* squareLength = (squareLength/2) > 0? squareLength-2:squareLength;
            circleRadius = circleRadius < 19? circleRadius+1:circleRadius;*/
			break;
		case GLUT_KEY_END:
		    /*squareLength = (squareLength/2) < 16? squareLength+2:squareLength;
            circleRadius = circleRadius > 0? circleRadius-1:circleRadius;*/
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
	//gluLookAt(100*cos(cameraAngle), 100*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(pos.x, pos.y, pos.z, pos.x + l.x, pos.y + l.y, pos.z + l.z, u.x, u.y, u.z);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();

	for (int i=0; i < objects.size(); i++) {
        objects[i]->draw();
    }

    for (int i=0; i < lights.size(); i++) {
        drawPoint(lights[i]);
    }

	//drawMyBox(squareLength/2);
    //drawMySphere(circleRadius, 30, 50);
    //drawMyCylinder(squareLength/2,circleRadius,30,50);



	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){

	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=100.0;
	cameraAngle=1.0;
	angle=0;
    fovY = 80;
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
	gluPerspective(fovY,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}




int main(int argc, char **argv){
    loadActualData();
	glutInit(&argc,argv);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
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
    freeMemory();
	return 0;
}
