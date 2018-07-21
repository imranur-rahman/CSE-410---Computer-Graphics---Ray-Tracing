#ifndef DRAW_H
#define DRAW_H

#define pi (2*acos(0.0))

#include "point.hpp"
#include <bits/stdc++.h>
using namespace std;

extern int drawaxes;
extern int drawgrid;
//extern struct point;

void drawAxes()
{
	if(drawaxes==1)
	{
//		glColor3f(1.0, 1.0, 1.0);
//		glBegin(GL_LINES);{
//			glVertex3f( 100,0,0);
//			glVertex3f(-100,0,0);
//
//			glVertex3f(0,-100,0);
//			glVertex3f(0, 100,0);
//
//			glVertex3f(0,0, 100);
//			glVertex3f(0,0,-100);
//		}glEnd();
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


void drawSphere(double radius){
    double length = radius * 2;
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

#endif
