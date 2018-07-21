#ifndef BASE_H
#define BASE_H

#include "drawing_code.hpp"
#include "bitmap_image.hpp"
#include <bits/stdc++.h>
using namespace std;

extern int recursion_level;

extern double dotProduct(point a, point b);
extern point crossProduct(point a, point b);

struct object
{
    point reference_point;
	double height, width, length;
	int shine;
	double color[3];
	double co_efficients[4];
	double source_factor = 1.0;
	double refracting_index = 1.5;

	object(){ }
	virtual void draw(){}
	virtual double intersect(Ray* r, double current_color[3], int level){}
	virtual double getIntersectionT(Ray* ray){}

	point getReflection(Ray* ray, point normal) {
	    const double cosI = dotProduct(ray->dir, normal);
        point reflection = ray->dir - 2.0 * cosI * normal;
        reflection.normalize();
        return reflection;
    }
    point getRefraction(Ray* ray, point normal) {
        const double cosI = -dotProduct(normal, ray->dir);
        const double sinT2 = refracting_index * refracting_index * ( 1.0 - sinT2 );
        if(sinT2 > 1.0)
            return {0,0,0};
        const double cosT = sqrt(1.0 - sinT2);
        return refracting_index * ray->dir + (refracting_index * cosI - cosT) * normal;
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
    void updateColorRange(double* current_color)
    {
        for (int i=0; i<3; i++) {
            if (current_color[i] > 1) {
                current_color[i] = 1;
            } else if (current_color[i] < 0) {
                current_color[i] = 0;
            }
        }
    }
    int getNearestPoint(Ray ray, vector<object*>& objects)
    {
        int nearest=-1;
        double minT = 9999999;

        for (int k=0; k < objects.size(); k++) {

            double tk = objects[k]->getIntersectionT(&ray);

            if(tk <= 0) {
                continue;
            } else if (tk < minT) {
                minT = tk;
                nearest = k;
            }

            //cout<<tk<<endl;
        }
        return nearest;
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

    void setColorAt(double* current_color, double* color)
    {
        for(int i = 0; i < 3; ++i)
            current_color[i] = color[i] * co_efficients[0];
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

        setColorAt(current_color, color);

        point normal = getNormal(intersectionPoint);

        point reflection = getReflection(ray, normal);
        point refraction = getRefraction(ray, normal);

        for (int i=0; i<lights.size(); i++) {

            point direction = lights[i] - intersectionPoint;
            double len = sqrt(direction.x*direction.x + direction.y*direction.y + direction.z*direction.z);
            direction.normalize();

            point start = intersectionPoint + direction * 1.0;
            Ray L(start, direction);
            //cout<<intersectionPoint<<L.start<<L.direction;

            bool hasObstacle = false;

            for (int j=0; j < objects.size(); j++) {

                double temp_intersect = objects[j]->getIntersectionT(&L);

                if(temp_intersect > 0   &&  temp_intersect <= len) {
                    hasObstacle = true;
                    break;
                }
            }

            if (hasObstacle == false){

                double lambert = dotProduct(L.dir, normal);
                double temp = dotProduct(reflection, ray->dir);
                double phong = pow(temp, shine);

                if(lambert < 0) lambert = 0;
                if(phong < 0) phong = 0;

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[1] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[2] * color[k];
                }
            }

            if (level < recursion_level) {

                start = intersectionPoint + reflection * 1.0;

                Ray reflectionRay(start, reflection);

                double reflected_color[3];
                int nearest = getNearestPoint(reflectionRay, objects);

                if(nearest!=-1) {

                    double ret = objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);
                    if(ret <= 0)
                        continue;

                    for (int k=0; k<3; k++) {
                        current_color[k] += reflected_color[k] * co_efficients[3];
                    }
                }


                start = intersectionPoint + refraction * 1.0;

                Ray refractionRay(start, refraction);

                double refracted_color[3];
                nearest = getNearestPoint(refractionRay, objects);

                if(nearest!=-1) {

                    double ret = objects[nearest]->intersect(&refractionRay, refracted_color, level+1);
                    if(ret <= 0)
                        continue;

                    for (int k=0; k<3; k++) {
                        current_color[k] += refracted_color[k] * refracting_index;
                    }
                }
            }
            updateColorRange(current_color);
        }

        return t;
    }
};

struct Floor: object{
    int numberOfTiles;
    bitmap_image texture;
    double texture_height, texture_width;
    Floor(double FloorWidth, double TileWidth){
        reference_point = point(-FloorWidth / 2.0, -FloorWidth / 2.0, 0);
        length = TileWidth;
        numberOfTiles = FloorWidth / TileWidth;
        texture = bitmap_image("newt.bmp");
        texture_height = texture.height() / abs(FloorWidth);
        texture_width = texture.width() / abs(FloorWidth);
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

    void setColorAt(double* current_color, double* color, double* rgb)
    {
        for(int i = 0; i < 3; ++i)
            current_color[i] = color[i] * co_efficients[0] * rgb[i] / 255.0;
    }


    double intersect(Ray* ray, double current_color[3], int level) {

        double t = getIntersectionT(ray);

        point intersectionPoint = ray->start + ray->dir * t;

        double xMin = reference_point.x;
        double xMax = -xMin;

        double yMin = reference_point.y;
        double yMax = -yMin;

        if (xMin > intersectionPoint.x || intersectionPoint.x > xMax ||
                yMin > intersectionPoint.y || intersectionPoint.y > yMax) {
            return -1;
        }


        int xVal = (intersectionPoint.x - reference_point.x) / length;
        int yVal = (intersectionPoint.y - reference_point.y) / length;

        if ((xVal+yVal)%2) {
            color[0] = color[1] = color[2] = 0;
        } else {
            color[0] = color[1] = color[2] = 1;
        }

        unsigned char r, g, b;
        int x = (intersectionPoint.x + abs(reference_point.x)) * texture_width;
        int y = (intersectionPoint.y + abs(reference_point.y)) * texture_height;

        //cout<<x<<','<<y<<endl;

        texture.get_pixel(x, y, r, g, b);

        double rgb[] = {r, g, b};
        //cout<<rgb[0];

        setColorAt(current_color, color, rgb);

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

            bool hasObstacle = false;

            for (int j=0; j < objects.size(); j++) {

                double temp_intersect = objects[j]->getIntersectionT(&L);

                if(temp_intersect > 0  &&  temp_intersect <= len) {
                    hasObstacle = true;
                    break;
                }
            }

            if (hasObstacle == false){

                double lambert = dotProduct(L.dir, normal);
                double temp = dotProduct(reflection, ray->dir);
                double phong = pow(temp, shine);

                if(lambert < 0) lambert = 0;
                if(phong < 0) phong = 0;

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[1] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[2] * color[k];
                }
            }

            if (level < recursion_level) {

                start = intersectionPoint + reflection * 1.0;

                Ray reflectionRay(start, reflection);

                double reflected_color[3];
                int nearest = getNearestPoint(reflectionRay, objects);

                if(nearest!=-1) {

                    double ret = objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);
                    if(ret <= 0)
                        continue;

                    for (int k=0; k<3; k++) {
                        current_color[k] += reflected_color[k] * co_efficients[3];
                    }
                }
            }

            updateColorRange(current_color);
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

    Triangle(point ar[3])
    {
        this->a = ar[0];
        this->b = ar[1];
        this->c = ar[2];
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

    void setColorAt(double* current_color, double* color)
    {
        for (int i=0; i<3; i++) {
            current_color[i] = color[i] * co_efficients[0];
        }
    }


    double intersect(Ray* ray, double current_color[3], int level) {

        double t = getIntersectionT(ray);

        if (t <= 0) return -1;
        if (level == 0 ) return t;

        setColorAt(current_color, color);

        point intersectionPoint = ray->start + ray->dir * t;

        for (int i=0; i<lights.size(); i++) {

            point normal = getNormal(intersectionPoint);

            point dir = lights[i] - intersectionPoint;
            double len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
            dir.normalize();

            point reflection = getReflection(ray, normal);

            point start = intersectionPoint + dir*1.0;
            Ray L(start, dir);
            //cout<<intersectionPoint<<L.start<<L.dir;

            bool hasObstacle = false;

            for (int j=0; j < objects.size(); j++) {

                double temp_intersect = objects[j]->getIntersectionT(&L);

                if(temp_intersect > 0  &&  temp_intersect <= len) {
                    hasObstacle = true;
                    break;
                }
            }

            if (hasObstacle == false){

                double lambert = dotProduct(L.dir, normal);
                double temp = dotProduct(reflection, ray->dir);
                double phong = pow(temp, shine);

                if(lambert < 0) lambert = 0;
                if(phong < 0) phong = 0;

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[1] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[2] * color[k];
                }
            }

            if (level < recursion_level) {

                start = intersectionPoint + reflection * 1.0;

                Ray reflectionRay(start, reflection);

                double reflected_color[3];
                int nearest = getNearestPoint(reflectionRay, objects);

                if(nearest!=-1) {

                    double ret = objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);
                    if(ret <= 0)
                        continue;

                    for (int k=0; k<3; k++) {
                        current_color[k] += reflected_color[k] * co_efficients[3];
                    }
                }
            }

            updateColorRange(current_color);

        }

        return t;
    }



};


struct GeneralQuadratic: object {

public:

    double A, B, C, D, E, F, G, H, I, J;

    GeneralQuadratic(double coeff[10], point reff, double length, double width, double height) {
        this->A = coeff[0];
        this->B = coeff[1];
        this->C = coeff[2];
        this->D = coeff[3];
        this->E = coeff[4];
        this->F = coeff[5];
        this->G = coeff[6];
        this->H = coeff[7];
        this->I = coeff[8];
        this->J = coeff[9];
        this->reference_point = reff;
        this->height = height;
        this->width = width;
        this->length = length;
    }

    void draw() {}

    point getNormal(point intersection) {

        double m = 2 * A * intersection.x + D * intersection.y + F * intersection.z  + G;//dF/dx
        double n = 2 * B * intersection.y + D * intersection.x + E * intersection.z  + H;//dF/dy
        double o = 2 * C * intersection.z + E * intersection.y + F * intersection.x  + I;//dF/dz

        point normal(m, n, o);
        normal.normalize();
        return normal;
    }

    double getIntersectionT(Ray* ray) {

        double a = A * ray->dir.x * ray->dir.x + B * ray->dir.y * ray->dir.y + C * ray->dir.z * ray->dir.z
                 + D * ray->dir.x * ray->dir.y + E * ray->dir.y * ray->dir.z + F * ray->dir.z * ray->dir.x;
        double b = 2 * (A * ray->start.x * ray->dir.x + B * ray->start.y * ray->dir.y + C * ray->start.z * ray->dir.z) +
                 + D * (ray->start.x * ray->dir.y + ray->dir.x * ray->start.y)
                 + E * (ray->start.y * ray->dir.z + ray->dir.y * ray->start.z)
                 + F * (ray->start.z * ray->dir.x + ray->dir.z * ray->start.x)
                 + G * ray->dir.x + H * ray->dir.y + I * ray->dir.z;
        double c = A * ray->start.x * ray->start.x + B * ray->start.y * ray->start.y + C * ray->start.z * ray->start.z
                 + D * ray->start.x * ray->start.y + E * ray->start.y * ray->start.z + F * ray->start.z * ray->start.x
                 + G * ray->start.x + H * ray->start.y + I * ray->start.z + J;

        double d = b*b - 4*a*c;


        if (d < 0) {
            return -1;
        }

        double t1 = (- b + sqrt(d)) / (2.0*a);
        double t2 = (- b - sqrt(d)) / (2.0*a);


        point intersectionPoint1 = ray->start + ray->dir * t1;
        point intersectionPoint2 = ray->start + ray->dir * t2;

        double xMin = reference_point.x;
        double xMax = xMin + length;

        double yMin = reference_point.y;
        double yMax = yMin + width;

        double zMin = reference_point.z;
        double zMax = zMin + height;

        //cout<<xMin<<','<<xMax<<';'<<yMin<<','<<yMax<<';'<<zMin<<','<<zMax<<endl;
        //cout<<intersectionPoint1<<intersectionPoint2;

        bool f11 = length > 0 && (xMin > intersectionPoint1.x || intersectionPoint1.x > xMax);
        bool f12 = width > 0 && (yMin > intersectionPoint1.y || intersectionPoint1.y > yMax);
        bool f13 = height > 0 && (zMin > intersectionPoint1.z || intersectionPoint1.z > zMax);

        bool flag1 = f11 || f12 || f13;

        bool f21 = length > 0 && (xMin > intersectionPoint2.x || intersectionPoint2.x > xMax);
        bool f22 = width > 0 && (yMin > intersectionPoint2.y || intersectionPoint2.y > yMax);
        bool f23 = height > 0 && (zMin > intersectionPoint2.z || intersectionPoint2.z > zMax);

        bool flag2 = f21 || f22 || f23;

        //cout<<flag1<<','<<flag2<<endl;

        if (flag1 && flag2) {
            return -1;
        } else if (flag1) {
            return t2;
        } else if (flag2) {
            return t1;
        } else {
            return min(t1, t2);
        }
    }

    void setColorAt(double* current_color, double* color)
    {
        for (int i=0; i<3; i++) {
            current_color[i] = color[i] * co_efficients[0];
        }
    }


    double intersect(Ray* ray, double current_color[3], int level) {

        double t = getIntersectionT(ray);

        if (t <= 0) return -1;
        if (level == 0 ) return t;

        setColorAt(current_color, color);

        //cout<<t<<endl;
        point intersectionPoint = ray->start + ray->dir * t;

        point normal = getNormal(intersectionPoint);

        point reflection = getReflection(ray, normal);


        for (int i=0; i<lights.size(); i++) {

            point dir = lights[i] - intersectionPoint;
            double len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
            dir.normalize();

            point start = intersectionPoint + dir*1.0;
            Ray L(start, dir);
            //cout<<intersectionPoint<<L.start<<L.dir;

            bool hasObstacle = false;

            for (int j=0; j < objects.size(); j++) {

                double temp_intersect = objects[j]->getIntersectionT(&L);

                if(temp_intersect > 0  &&  temp_intersect <= len) {
                    hasObstacle = true;
                    break;
                }
            }

            if (hasObstacle == false){

                double lambert = dotProduct(L.dir, normal);
                double temp = dotProduct(reflection, ray->dir);
                double phong = pow(temp, shine);

                if(lambert < 0) lambert = 0;
                if(phong < 0) phong = 0;

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[1] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[2] * color[k];
                }
            }

            if (level < recursion_level) {

                start = intersectionPoint + reflection * 1.0;

                Ray reflectionRay(start, reflection);

                double reflected_color[3];
                int nearest = getNearestPoint(reflectionRay, objects);

                if(nearest!=-1) {

                    double ret = objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);
                    if(ret <= 0)
                        continue;

                    for (int k=0; k<3; k++) {
                        current_color[k] += reflected_color[k] * co_efficients[3];
                    }
                }
            }

            updateColorRange(current_color);
        }

        return t;
    }
};


#endif
