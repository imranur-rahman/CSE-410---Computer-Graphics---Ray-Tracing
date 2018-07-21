#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<vector>
#include <windows.h>
#include <glut.h>
#include <iostream>

#include "objects.hpp"
#include "point.hpp"
#include "bitmap_image.hpp"


#define pi (2*acos(0.0))

using namespace std;

double cameraHeight;
double cameraAngle;
double angle;
double tr;
double ta;
double tslices;
double tstacks;
double s1;
int image_width,image_height;
double window_height,window_width;
double fovy;
int recursion_level;


point u,r,l,pos;

vector<Object*> objects;
vector<point> lights;

void drawAxes()
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


void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}

void drawCircle(double radius,int segments)
{
    int i;
    //struct point points[100];
    point *points;
    points=new point[100];
    glColor3f(1,0,0);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void loadTestData()
{
    pos.x=0;
    pos.y=-100;
    pos.z=20;

    image_width=768;
    image_height=768;

    window_height=500;
    window_width=500;

    recursion_level = 1;
    Object *temp;

	temp=new Sphere(point(0,0,10), 10); // Center(0,0,10), Radius 10
	temp->setColor(1,0,0);
	temp->setCoEfficients(0.4,0.2,0.2,0.2);
	temp->setShine(1);
	objects.push_back(temp);


	temp=new Floor(1000, 20);
	temp->setCoEfficients(0.4,0.2,0.2,0.2);
	temp->setShine(1);
	objects.push_back(temp);

    /*
	double arr[] = {1, 1, 1, 0, 0, 0, -20, -20, -20, 200};
    point p(0, 0, 0);

    temp = new Quadratic(arr, point(0, 0, 0), 0, 0, 5);
    temp->setColor(1, 0, 1);
    temp->setCoEfficients(0.4, 0.2, 0.1, 0.3);
    temp->setShine(3);
    objects.push_back(temp);

    /*
	temp=new Sphere(point(0,0,40), 10);
	temp->setColor(0,0,1);
	temp->setCoEfficients(0.4,0.2,0.2,0.5);
	temp->setShine(1);
	objects.push_back(temp);

	temp=new Sphere(point(40,0,10), 20);
	temp->setColor(1,1,1);
	temp->setCoEfficients(0.4,0.2,0.2,0.2);
	temp->setShine(1);
	objects.push_back(temp);


	temp=new Triangle(point(10,0,30), point(-10,0,30), point(0,0,45));
	temp->setColor(0,1,1);
	temp->setCoEfficients(0.4,0.2,0.2,0.2);
	temp->setShine(1);
	objects.push_back(temp);
	*/

	lights.push_back(point(-70,70,70));
	lights.push_back(point(70,70,70));

}

void loadActualData()
{
    pos.x=0;
    pos.y=-100;
    pos.z=50;

    window_height=500;
    window_width=500;

    freopen("scene.txt", "r", stdin);
    int num_obj,num_lights;

    cin>>recursion_level;
    cin>>image_width;
    image_height=image_width;
    cin>>num_obj;

    string cmd;
    double a,b,c,d;
    point p1,p2,p3;
    Object *temp;
    for(int i=0;i<num_obj;i++){
        cin>>cmd;
        if(cmd=="sphere"){
            cin>>p1.x>>p1.y>>p1.z;
            //cout<<p.x<<p.y<<p.z<<endl;
            cin>>a;
            temp = new Sphere(p1, a);

            cin>>a>>b>>c;
            temp->setColor(a,b,c);

            cin>>a>>b>>c>>d;
            temp->setCoEfficients(a,b,c,d);

            cin>>a;
            temp->setShine(a);
            objects.push_back(temp);
        }
        else if(cmd=="triangle"){
            cin>>p1.x>>p1.y>>p1.z;
            //cout<<p1.x<<p1.y<<p1.z<<endl;
            cin>>p2.x>>p2.y>>p2.z;
            cin>>p3.x>>p3.y>>p3.z;
            temp=new Triangle(p1, p2, p3);

            cin>>a>>b>>c;
            temp->setColor(a,b,c);

            cin>>a>>b>>c>>d;
            temp->setCoEfficients(a,b,c,d);

            cin>>a;
            temp->setShine(a);
            objects.push_back(temp);
        }
        else if(cmd=="general"){
            double arr[10];
            for(int j=0;j<10;j++) cin>>arr[j];

            cin>>p1.x>>p1.y>>p1.z;
            cin>>a>>b>>c;

            temp = new Quadratic(arr, p1, a, b, c);

            cin>>a>>b>>c;
            temp->setColor(a,b,c);

            cin>>a>>b>>c>>d;
            temp->setCoEfficients(a,b,c,d);

            cin>>a;
            temp->setShine(a);
            objects.push_back(temp);
        }
    }

    cin>>num_lights;
    //point *light;
    for(int i=0;i<num_lights;i++){
        cin>>a>>b>>c;
        //light=new point(a,b,c);
        lights.push_back(point(a,b,c));
    }

    temp = new Floor(1000, 20);
    temp->setCoEfficients(0.4,0.2,0.2,0.2);
    temp->setShine(1);
    objects.push_back(temp);

}

void capture()
{
    bitmap_image image(image_width,image_height);

    for(int i=0;i<image_width;i++){
        for(int j=0;j<image_height;j++){
            image.set_pixel(j,i,0,0,0);
        }
    }
    double plane_distance= (window_height/2)/tan(fovy*pi/360);
    point topleft;
    topleft= pos + l*plane_distance-r*(window_width/2)+u*(window_height/2);

    double du=window_width/image_width;
    double dv=window_height/image_height;

    cout<<du<<dv<<endl;

    for(int i=0;i<image_height;i++){
        for(int j=0;j<image_width;j++){
            point corner,temp;
            corner = topleft + r*j*du - u*i*dv;
            temp = (corner-pos).normalize();
            Ray *ray=new Ray(pos,temp);

            int nearest = -1;
            double t_min = 100000000;
            double *color=new double[3];
            for(int k=0;k<objects.size();k++){
                double t =objects[k]->intersect(ray, color, 0);
                if(t<=0) continue;
                if(t<t_min){
                    t_min=t;
                    nearest=k;
                }
            }
            if(nearest!=-1){
                double t =objects[nearest]->intersect(ray, color, 1);
                cout<<' '<<i<<' '<<j<<' ';
                cout<<nearest<<' '<<color[0]<<color[1]<<color[2]<<endl;
                image.set_pixel(j,i,color[0]*255,color[1]*255,color[2]*255);
            }
        }
    }
    image.save_image("capture.bmp");

    cout<<"captured"<<endl;
}


void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			double a,b,c;
			a=r.x*cos(pi/60)+u.x*sin(pi/60);
			b=r.y*cos(pi/60)+u.y*sin(pi/60);
			c=r.z*cos(pi/60)+u.z*sin(pi/60);

			r.x=a/sqrt(a*a+b*b+c*c);
			r.y=b/sqrt(a*a+b*b+c*c);
			r.z=c/sqrt(a*a+b*b+c*c);

			a=u.x*cos(pi/60)+r.x*sin(pi/60);
			b=u.y*cos(pi/60)+r.y*sin(pi/60);
			c=u.z*cos(pi/60)+r.z*sin(pi/60);

			u.x=a/sqrt(a*a+b*b+c*c);
			u.y=b/sqrt(a*a+b*b+c*c);
			u.z=c/sqrt(a*a+b*b+c*c);
			break;

        case '2':
			a=r.x*cos(pi/60)-u.x*sin(pi/60);
			b=r.y*cos(pi/60)-u.y*sin(pi/60);
			c=r.z*cos(pi/60)-u.z*sin(pi/60);

			r.x=a/sqrt(a*a+b*b+c*c);
			r.y=b/sqrt(a*a+b*b+c*c);
			r.z=c/sqrt(a*a+b*b+c*c);

			a=u.x*cos(pi/60)-r.x*sin(pi/60);
			b=u.y*cos(pi/60)-r.y*sin(pi/60);
			c=u.z*cos(pi/60)-r.z*sin(pi/60);

			u.x=a/sqrt(a*a+b*b+c*c);
			u.y=b/sqrt(a*a+b*b+c*c);
			u.z=c/sqrt(a*a+b*b+c*c);
			break;

        case '3':
			a=l.x*cos(pi/60)+u.x*sin(pi/60);
			b=l.y*cos(pi/60)+u.y*sin(pi/60);
			c=l.z*cos(pi/60)+u.z*sin(pi/60);

			l.x=a/sqrt(a*a+b*b+c*c);
			l.y=b/sqrt(a*a+b*b+c*c);
			l.z=c/sqrt(a*a+b*b+c*c);

			a=u.x*cos(pi/60)+l.x*sin(pi/60);
			b=u.y*cos(pi/60)+l.y*sin(pi/60);
			c=u.z*cos(pi/60)+l.z*sin(pi/60);

			u.x=a/sqrt(a*a+b*b+c*c);
			u.y=b/sqrt(a*a+b*b+c*c);
			u.z=c/sqrt(a*a+b*b+c*c);
			break;

        case '4':
			a=l.x*cos(pi/60)-u.x*sin(pi/60);
			b=l.y*cos(pi/60)-u.y*sin(pi/60);
			c=l.z*cos(pi/60)-u.z*sin(pi/60);

			l.x=a/sqrt(a*a+b*b+c*c);
			l.y=b/sqrt(a*a+b*b+c*c);
			l.z=c/sqrt(a*a+b*b+c*c);

			a=u.x*cos(pi/60)-l.x*sin(pi/60);
			b=u.y*cos(pi/60)-l.y*sin(pi/60);
			c=u.z*cos(pi/60)-l.z*sin(pi/60);

			u.x=a/sqrt(a*a+b*b+c*c);
			u.y=b/sqrt(a*a+b*b+c*c);
			u.z=c/sqrt(a*a+b*b+c*c);
			break;

        case '5':
			a=l.x*cos(pi/60)+r.x*sin(pi/60);
			b=l.y*cos(pi/60)+r.y*sin(pi/60);
			c=l.z*cos(pi/60)+r.z*sin(pi/60);

			l.x=a/sqrt(a*a+b*b+c*c);
			l.y=b/sqrt(a*a+b*b+c*c);
			l.z=c/sqrt(a*a+b*b+c*c);

			a=r.x*cos(pi/60)+l.x*sin(pi/60);
			b=r.y*cos(pi/60)+l.y*sin(pi/60);
			c=r.z*cos(pi/60)+l.z*sin(pi/60);

			r.x=a/sqrt(a*a+b*b+c*c);
			r.y=b/sqrt(a*a+b*b+c*c);
			r.z=c/sqrt(a*a+b*b+c*c);
			break;

        case '6':
			a=l.x*cos(pi/60)-r.x*sin(pi/60);
			b=l.y*cos(pi/60)-r.y*sin(pi/60);
			c=l.z*cos(pi/60)-r.z*sin(pi/60);

			l.x=a/sqrt(a*a+b*b+c*c);
			l.y=b/sqrt(a*a+b*b+c*c);
			l.z=c/sqrt(a*a+b*b+c*c);

			a=r.x*cos(pi/60)-l.x*sin(pi/60);
			b=r.y*cos(pi/60)-l.y*sin(pi/60);
			c=r.z*cos(pi/60)-l.z*sin(pi/60);

			r.x=a/sqrt(a*a+b*b+c*c);
			r.y=b/sqrt(a*a+b*b+c*c);
			r.z=c/sqrt(a*a+b*b+c*c);
			break;

        case '0':
            capture();
            break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			pos.x=pos.x-s1*l.x;
			pos.y=pos.y-s1*l.y;
			pos.z=pos.z-s1*l.z;
			break;
		case GLUT_KEY_UP:		// up arrow key
			pos.x=pos.x+s1*l.x;
			pos.y=pos.y+s1*l.y;
			pos.z=pos.z+s1*l.z;
			break;

		case GLUT_KEY_RIGHT:
			pos.x=pos.x+s1*r.x;
			pos.y=pos.y+s1*r.y;
			pos.z=pos.z+s1*r.z;
			break;
		case GLUT_KEY_LEFT:
			pos.x=pos.x-s1*r.x;
			pos.y=pos.y-s1*r.y;
			pos.z=pos.z-s1*r.z;
			break;

		case GLUT_KEY_PAGE_UP:
		    pos.x=pos.x+s1*u.x;
			pos.y=pos.y+s1*u.y;
			pos.z=pos.z+s1*u.z;
			break;
		case GLUT_KEY_PAGE_DOWN:
		    pos.x=pos.x-s1*u.x;
			pos.y=pos.y-s1*u.y;
			pos.z=pos.z-s1*u.z;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
		    tr=tr-1;
		    if(tr<0) tr=0;
			break;
		case GLUT_KEY_END:
		    tr=tr+1;
		    if(tr>ta) tr=ta;
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				//drawaxes=1-drawaxes;
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
	gluLookAt(pos.x, pos.y, pos.z,pos.x + l.x, pos.y + l.y, pos.z + l.z,u.x, u.y, u.z);
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

    //drawSpherePart();
    //drawHemisphere(tr,tslices,tstacks,1);

    for (int i=0; i < objects.size(); i++) {
        objects[i]->draw();
    }

    for (int i=0; i < lights.size(); i++) {
        lights[i].draw();
    }

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
	fovy=80;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;
	tslices=20;
	tstacks=24;

	tr=15;
	ta=30;
	s1=2;

    u.x=0;
    u.y=0;
    u.z=1;

    r.x=1;
    r.y=0;
    r.z=0;

    l.x=0;
    l.y=1;
    l.z=0;

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
	gluPerspective(fovy,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
    //cout<<"here"<<endl;
    //loadTestData();
    loadActualData();
    cout<<objects.size()<<endl;
    cout<<lights.size()<<endl;
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
