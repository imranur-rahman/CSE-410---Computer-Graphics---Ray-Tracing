#ifndef BASE_H
#define BASE_H

#include "drawing_code.hpp"
#include <bits/stdc++.h>
using namespace std;



extern double dotProduct(point a, point b);
extern point crossProduct(point a, point b);

enum {AMBIENT, DIFFUSE, SPECULAR, REFLECTION};

struct object
{
    point reference_point;
	double height, width, length;
	int shine;
	double color[3];
	double co_efficients[4];
	double source_factor = 1.0;

	object(){ }
	virtual void draw(){}
	virtual double intersect(Ray* r, double current_color[3], int level){}
	virtual double getIntersectionT(Ray* ray){}
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

extern vector<object*> objects;
extern vector<point> lights; //actually not point, vector

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

    double getIntersectionT(Ray* ray) {

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

//    double* getColorAt(point intersectionPoint)
//    {
//        double
//    }

    void setColorAt(double* current_color, double* color)
    {
        for(int i = 0; i < 3; ++i)
            current_color[i] = color[i] * co_efficients[AMBIENT];
    }

    point getReflection(Ray* ray, point normal) {
        point reflection = ray->dir - 2.0 * dotProduct(ray->dir, normal) * normal;
        reflection.normalize();
        return reflection;
    }

    point getNormal(point intersection) {
        point normal = intersection - reference_point;
        normal.normalize();
        return normal;
    }

    double intersect(Ray* ray, double current_color[3], int level) {

        double t = getIntersectionT(ray);

        if (t <= 0) return -1;
        if (level == 0 ) return t;

        point intersectionPoint = ray->start + ray->dir * t;

//        for (int i=0; i<3; i++) {
//            current_color[i] = color[i];
//        }
        //double* colorAt = getColorAt(intersectionPoint);
        setColorAt(current_color, color);

        point normal = getNormal(intersectionPoint);

        point reflection = getReflection(ray, normal);

        for (int i=0; i<lights.size(); i++) {

            point direction = lights[i] - intersectionPoint;
            direction.normalize();
            double len = sqrt(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);

            point start = intersectionPoint + direction * 1.0;
            Ray L(start, direction);
            //cout<<intersectionPoint<<L.start<<L.direction;

            bool flag = false;

            for (int j=0; j < objects.size(); j++) {

                double tObj = objects[j]->getIntersectionT(&L);

                if(tObj > 0 || abs(tObj) > len) {
                    flag = true;
                    break;
                }
            }

            if (flag){

                double lambert = dotProduct(L.dir, normal);
                double phong = pow(dotProduct(reflection, ray->dir), shine);

                //lambert = lambert > 0 ? lambert : 0;
                //phong = phong > 0 ? phong : 0;
                if(lambert < 0) lambert = 0;
                if(phong < 0) phong = 0;

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[DIFFUSE] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[SPECULAR] * color[k];
                }
            }

//            if (level < recursion_level) {
//
//                start = intersectionPoint + reflection * 1.0;
//
//                Ray reflectionRay(start, reflection);
//
//                int nearest=-1;
//                double minT = 9999999;
//                double reflected_color[3];
//
//                for (int k=0; k < objects.size(); k++) {
//
//                    double tk = objects[k]->getIntersectionT(&reflectionRay, true);
//
//                    if(tk <= 0) {
//                        continue;
//                    } else if (tk < minT) {
//                        minT = tk;
//                        nearest = k;
//                    }
//
//                    //cout<<tk<<endl;
//                }
//
//                if(nearest!=-1) {
//
//                    objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);
//
//                    for (int k=0; k<3; k++) {
//                        current_color[k] += reflected_color[k] * co_efficients[REFLECTION];
//                    }
//                }
//
//                start = intersectionPoint + refraction * 1.0;
//
//                Ray refractionRay(start, refraction);
//
//                nearest=-1;
//                minT = 9999999;
//                double refracted_color[3];
//
//                for (int k=0; k < objects.size(); k++) {
//
//                    double tk = objects[k]->getIntersectionT(&refractionRay, true);
//
//                    if(tk <= 0) {
//                        continue;
//                    } else if (tk < minT) {
//                        minT = tk;
//                        nearest = k;
//                    }
//
//                    //cout<<tk<<endl;
//                }
//
//                if(nearest!=-1) {
//
//                    objects[nearest]->intersect(&refractionRay, refracted_color, level+1);
//
//                    for (int k=0; k<3; k++) {
//                        current_color[k] += refracted_color[k] * refIdx;
//                    }
//                }
//            }

//            for (int k=0; k<3; k++) {
//                if (current_color[k] > 1) {
//                    current_color[k] = 1;
//                } else if (current_color[k] < 0) {
//                    current_color[k] = 0;
//                }
//            }


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




#endif
