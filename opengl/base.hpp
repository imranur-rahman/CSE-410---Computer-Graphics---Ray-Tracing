#ifndef BASE_H
#define BASE_H

#include "drawing_code.hpp"
#include <bits/stdc++.h>
using namespace std;

extern int recursion_level;

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

	point getReflection(Ray* ray, point normal) {
        point reflection = ray->dir - 2.0 * dotProduct(ray->dir, normal) * normal;
        reflection.normalize();
        return reflection;
    }
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

    point getNormal(point intersection) {
        point normal = intersection - reference_point;
        normal.normalize();
        return normal;
    }

    double intersect(Ray* ray, double current_color[3], int level) {

        double t = getIntersectionT(ray);

        if (t <= 0) return -1;
        if (level == 0) return t;

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
            double len = sqrt(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);
            direction.normalize();

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

            if (level < recursion_level) {

                start = intersectionPoint + reflection * 1.0;

                Ray reflectionRay(start, reflection);

                int nearest=-1;
                double minT = 9999999;
                double reflected_color[3];

                for (int k=0; k < objects.size(); k++) {

                    double tk = objects[k]->getIntersectionT(&reflectionRay);

                    if(tk <= 0) {
                        continue;
                    } else if (tk < minT) {
                        minT = tk;
                        nearest = k;
                    }

                    //cout<<tk<<endl;
                }

                if(nearest!=-1) {

                    objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);

                    for (int k=0; k<3; k++) {
                        current_color[k] += reflected_color[k] * co_efficients[REFLECTION];
                    }
                }
            }
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

            for (int k=0; k<3; k++) {
                if (current_color[k] > 1) {
                    current_color[k] = 1;
                } else if (current_color[k] < 0) {
                    current_color[k] = 0;
                }
            }


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

    point getNormal(point intersection) {
        point normal(0,0,1);
        normal.normalize();
        return normal;
    }

    double getIntersectionT(Ray* ray) {

        point normal = getNormal(reference_point);

        double t = dotProduct(normal, ray->start) * (-1) / dotProduct(normal, ray->dir);

        return t;
    }


    double intersect(Ray* ray, double current_color[3], int level) {

        double t = getIntersectionT(ray);

        point intersectionPoint = ray->start + ray->dir * t;

        double xMin = reference_point.x;
        double xMax = xMin * (-1);

        double yMin = reference_point.y;
        double yMax = yMin * (-1);



        if (xMin > intersectionPoint.x || intersectionPoint.x > xMax ||
                yMin > intersectionPoint.y || intersectionPoint.y > yMax) {
            return -1;
        }

        //cout<<xMin<<','<<xMax<<';'<<yMin<<','<<yMax<<endl;
        //cout<<intersectionPoint;

        int xCord = (intersectionPoint.x - reference_point.x) / length;
        int yCord = (intersectionPoint.y - reference_point.y) / length;

        if ((xCord+yCord)%2) {
            color[0] = color[1] = color[2] = 0;
        } else {
            color[0] = color[1] = color[2] = 1;
        }

//        unsigned char r, g, b;
//        int x = (intersectionPoint.x + abs(reference_point.x)) * bdWidth;
//        int y = (intersectionPoint.y + abs(reference_point.y)) * bdHeight;
//
//        //cout<<x<<','<<y<<endl;
//
//        bd.get_pixel(x, y, r, g, b);
//
//        double rgb[] = {r, g, b};
        //cout<<rgb[0];

        for (int i=0; i<3; i++) {
            //current_color[i] = color[i] * co_efficients[AMBIENT] * rgb[i] / 255.0;
            current_color[i] = color[i] * co_efficients[AMBIENT];
        }

        point normal = getNormal(intersectionPoint);

        point reflection = getReflection(ray, normal);
        //point refraction = getRefraction(ray, normal);

        for (int i=0; i<lights.size(); i++) {

            point dir = lights[i] - intersectionPoint;
            double len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
            dir.normalize();

            point start = intersectionPoint + dir*1.0;
            Ray L(start, dir);
            //cout<<intersectionPoint<<L.start<<L.dir;

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

                if(lambert < 0) lambert = 0;
                if(phong < 0) phong = 0;

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[DIFFUSE] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[SPECULAR] * color[k];
                }
            }

            if (level < recursion_level) {

                start = intersectionPoint + reflection * 1.0;

                Ray reflectionRay(start, reflection);

                int nearest=-1;
                double minT = 9999999;
                double reflected_color[3];

                for (int k=0; k < objects.size(); k++) {

                    double tk = objects[k]->getIntersectionT(&reflectionRay);

                    if(tk <= 0) {
                        continue;
                    } else if (tk < minT) {
                        minT = tk;
                        nearest = k;
                    }

                    //cout<<tk<<endl;
                }

                if(nearest!=-1) {

                    objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);

                    for (int k=0; k<3; k++) {
                        current_color[k] += reflected_color[k] * co_efficients[REFLECTION];
                    }
                }

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
            }

            for (int k=0; k<3; k++) {
                if (current_color[k] > 1) {
                    current_color[k] = 1;
                } else if (current_color[k] < 0) {
                    current_color[k] = 0;
                }
            }


        }

        return t;
    }
};

struct Triangle: object {

    point a, b, c;

    Triangle(point a, point b, point c) {
        this->a = a;
        this->b = b;
        this->c = c;
    }

    void draw() {
        glColor3f(color[0],color[1],color[2]);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(a.x, a.y, a.z);
            glVertex3f(b.x, b.y, b.z);
            glVertex3f(c.x, c.y, c.z);
        }
        glEnd();
    }

    point getNormal(point intersection) {

        point edge1 = b - a;
        point edge2 = c - a;

        point normal = crossProduct(edge1, edge2);
        normal.normalize();

        return normal;
    }

    double getIntersectionT(Ray* ray) {

        const float EPSILON = 0.0000001;

        point edge1 = b - a;
        point edge2 = c - a;

        point h = crossProduct(ray->dir, edge2);
        double det = dotProduct(edge1, h);

        if(det > -EPSILON && det < EPSILON) {
            return -1;
        }

        double inv_det = 1.0 / det;

        point s = ray->start - a;

        double u = dotProduct(s, h) * inv_det;

        if(u < 0.0 || u > 1.0) {
            return -1;
        }

        point q = crossProduct(s, edge1);

        double v = dotProduct(ray->dir, q) * inv_det;

        if(v < 0.0 || u + v  > 1.0) {
            return -1;
        }

        double t = dotProduct(edge2, q) * inv_det;

        if(t > EPSILON) { //ray intersection

            return t;
        }

        return -1;
    }


    double intersect(Ray* ray, double current_color[3], int level) {

        double t = getIntersectionT(ray);

        if (t <= 0) {
            return -1;
        }

        if (level == 0 ) {
            return t;
        }


        for (int i=0; i<3; i++) {
            current_color[i] = color[i] * co_efficients[AMBIENT];
        }


        point intersectionPoint = ray->start + ray->dir * t;

        for (int i=0; i<lights.size(); i++) {

            point normal = getNormal(intersectionPoint);

            point dir = lights[i] - intersectionPoint;
            double len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
            dir.normalize();

            if (dotProduct(dir, normal) > 0) {
                normal = normal * (-1);
            }

            point reflection = getReflection(ray, normal);
            //point refraction = getRefraction(ray, normal);

            point start = intersectionPoint + dir*1.0;
            Ray L(start, dir);
            //cout<<intersectionPoint<<L.start<<L.dir;

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

                if(lambert < 0) lambert = 0;
                if(phong < 0) phong = 0;

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[DIFFUSE] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[SPECULAR] * color[k];
                }
            }

            if (level < recursion_level) {

                start = intersectionPoint + reflection * 1.0;

                Ray reflectionRay(start, reflection);

                int nearest=-1;
                double minT = 9999999;
                double reflected_color[3];

                for (int k=0; k < objects.size(); k++) {

                    double tk = objects[k]->getIntersectionT(&reflectionRay);

                    if(tk <= 0) {
                        continue;
                    } else if (tk < minT) {
                        minT = tk;
                        nearest = k;
                    }

                    //cout<<tk<<endl;
                }

                if(nearest!=-1) {

                    objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);

                    for (int k=0; k<3; k++) {
                        current_color[k] += reflected_color[k] * co_efficients[REFLECTION];
                    }
                }

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
            }

            for (int k=0; k<3; k++) {
                if (current_color[k] > 1) {
                    current_color[k] = 1;
                } else if (current_color[k] < 0) {
                    current_color[k] = 0;
                }
            }


        }

        return t;
    }



};


#endif
