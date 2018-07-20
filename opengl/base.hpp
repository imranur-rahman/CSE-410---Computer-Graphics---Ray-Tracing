#ifndef BASE_H
#define BASE_H

#include "drawing_code.hpp"
#include <bits/stdc++.h>
using namespace std;

extern double dotProduct(point a, point b);
extern point crossProduct(point a, point b);

struct object
{
    point reference_point;
	double height, width, length;
	int shine;
	double color[3];
	double co_efficients[4];

	object(){ }
	virtual void draw(){}
	virtual double intersect(ray* r, double current_color[3], int level){}
    void setColor(double r, double g, double b)
    {
        color[0] = r;
        color[1] = g;
        color[2] = b;
    }
    void setCoEfficients(double a, double b, double c, double d)
    {
        co_efficients[0] = a;
        co_efficients[1] = b;
        co_efficients[2] = c;
        co_efficients[3] = d;
    }
    void setShine(double shine)
    {
        this->shine = shine;
    }

};

struct sphere: object{
    sphere(point center, double radius){
        reference_point = center;
        length = radius;
    }

    void draw(){
        //write codes for drawing sphere
        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        drawSphere(length);
        glPopMatrix();
    }

    double getIntersectionT(ray* ray) {

        point start = ray->start - reference_point;
        //start = start.normalize();

        double a = 1.0;
        double b = 2 * dotProduct(ray->dir, start);
        double c = dotProduct(start, start) - length*length;

        double d = b * b - 4 * a * c;

        if (d < 0) {
            return -1;
        }

        double t1 = (- b + sqrt(d)) / (2.0 * a);
        double t2 = (- b - sqrt(d)) / (2.0 * a);

        return min(t1, t2);
    }

    double intersect(ray* ray, double current_color[3], int level) {

        double t = getIntersectionT(ray);

        if (t <= 0) {
            return -1;
        }

        if (level == 0 ) {
            return t;
        }

        for (int i=0; i<3; i++) {
            current_color[i] = color[i];
        }
        return t;
    }
};

struct Floor: object{
    int numberOfTiles;
    Floor(double FloorWidth, double TileWidth){
        reference_point = point(-FloorWidth / 2.0, -FloorWidth / 2.0, 0);
        length = TileWidth;
        numberOfTiles = FloorWidth / TileWidth;
    }

    void draw(){
        //write codes for drawing black and white floor
        for (int i = 0; i < numberOfTiles; ++i)
        {
            bool flag = (i % 2) ? false : true;
            for (int j = 0; j < numberOfTiles; ++j)
            {
                flag = !flag;
                glColor3f(flag, flag, flag);

                glBegin(GL_QUADS);
                {
                    glVertex3f(reference_point.x + length * i, reference_point.y + length * j, reference_point.z);
                    glVertex3f(reference_point.x + length * (i + 1), reference_point.y + length * j, reference_point.z);
                    glVertex3f(reference_point.x + length * (i + 1), reference_point.y + length * (j + 1), reference_point.z);
                    glVertex3f(reference_point.x + length * i, reference_point.y + length * (j + 1), reference_point.z);
                }
                glEnd();
            }
        }
    }
};


extern vector<object*> objects;
extern vector<point> lights; //actually not point, vector

#endif
