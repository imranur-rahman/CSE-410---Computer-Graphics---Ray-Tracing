#ifndef OBJECTS_INCLUDED
#define OBJECTS_INCLUDED
#include "point.hpp"
#include "bitmap_image.hpp"
#include<cmath>
#include<iostream>

#define pi (2*acos(0.0))

using namespace std;


double dot(point a, point b)
{
    return (b.x*a.x+b.y*a.y+b.z*a.z);
}

point cross(point p, point q)
{
    point t;
    t.x=p.y*q.z-p.z*q.y;
    t.y=p.z*q.x-p.x*q.z;
    t.z=p.x*q.y-p.y*q.x;
    return t;
}

class Object
{
public:
	point reference_point;
	double height, width, length;
	int shine;
	double *color;
	double *co_efficients;
	double source_factor=1;
	double ref_index=1.33;

	Object()
	{
	    color=new double[3];
	    co_efficients=new double[4];
	}
	virtual void draw()=0;
	virtual point getNormal(point intersectionPoint)=0;
	virtual double getIntersectingT(Ray *r){
		return -1;
    }

    point getReflection(Ray *r, point intersectionPoint)
    {
        point normal=getNormal(intersectionPoint);
        point reflection=r->dir -  normal*(2 * dot(r->dir, normal));
        return reflection.normalize();
    }

    point getRefraction(Ray *r, point intersectionPoint)
    {
        point normal=getNormal(intersectionPoint);
        float N_dot_I = dot(normal, r->dir);
        float k = 1 - ref_index * ref_index * (1 - N_dot_I * N_dot_I);
        if (k < 0) return point(0, 0, 0);
        else{
            point out = r->dir * ref_index  - normal*(ref_index * N_dot_I + sqrtf(k));
            return out.normalize();
        }
    }

    virtual double intersect(Ray *r, double *current_color, int level){
		return -1;
    }

    void setColor(double a,double b,double c)
    {
        color[0]=a;
        color[1]=b;
        color[2]=c;
    }
    void setShine(int a)
    {
        shine=a;
    }
    //AMBIENT, DIFFUSE, SPECULAR, REFLECTION
    void setCoEfficients(double a,double d,double s,double r)
    {
        co_efficients[0]=a;
        co_efficients[1]=d;
        co_efficients[2]=s;
        co_efficients[3]=r;
    }
};

extern vector<Object*> objects;
extern vector<point> lights;
extern int recursion_level;


class Sphere:public Object
{
public:
    Sphere(point Center,double Radius){
        reference_point=Center;
        length=Radius;
    }

    point getNormal(point intersectionPoint)
    {
        point p=intersectionPoint-reference_point;
        return p.normalize();
    }

    double getIntersectingT(Ray *r){
		double a,b,c,d;
        point start = r->start - reference_point;
        a = 1;
        b = 2*dot(start,r->dir);
        c = dot(start, start)-length*length;

        d=b*b-4*a*c;

        if(d<0) return -1;
        double t1=(-b+sqrt(d))/(2*a);
        double t2=(-b-sqrt(d))/(2*a);
        double t=min(t1, t2);
        return t;
    }

    double intersect(Ray *r, double *current_color, int level){
        double t= getIntersectingT(r);
        if(t<=0) return -1;
        if(level==0)return t;

        for(int i=0;i<3;i++) current_color[i]=color[i]*co_efficients[0];

        point intersectionPoint = r->start+ (r->dir)*t;
        point normal = getNormal(intersectionPoint);
        point reflection=getReflection(r, intersectionPoint);
        point refraction=getRefraction(r, intersectionPoint);

        for(int i=0;i<lights.size();i++){
            point direction = lights[i]-intersectionPoint;
            double len = dot(direction, direction);
            direction=direction.normalize();

            point start= intersectionPoint + direction*1.0;
            Ray *L=new Ray(start, direction);

            bool obscured=false;
            for(int j=0;j<objects.size();j++){
                double t = objects[j]->getIntersectingT(L);
                if(t>0 &&t<len){
                    obscured=true;
                    break;
                }
            }

            if(!obscured){
                double lambert = max(0.0, dot(L->dir, normal));
                double phong = max(0.0, pow(dot(reflection, r->dir), shine));

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[1] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[2] * color[k];
                }
            }

            if(level<recursion_level){
                start=intersectionPoint+reflection*1;
                Ray *reflectionRay= new Ray(start, reflection);

                int nearest = -1;
                double t_min = 100000000;
                double *reflected_color=new double[3];
                for(int k=0;k<objects.size();k++){
                    double t =objects[k]->getIntersectingT(reflectionRay);
                    if(t<=0) continue;
                    if(t<t_min){
                        t_min=t;
                        nearest=k;
                    }
                }
                if(nearest!=-1){
                    double t =objects[nearest]->intersect(reflectionRay, reflected_color, level+1);
                    for (int k=0; k<3; k++) {
                        current_color[k] += co_efficients[3] * reflected_color[k];
                    }
                }

                start=intersectionPoint+refraction*1;
                Ray *refractionRay= new Ray(start, refraction);

                nearest = -1;
                t_min = 100000000;
                double *refracted_color=new double[3];
                for(int k=0;k<objects.size();k++){
                    double t =objects[k]->getIntersectingT(refractionRay);
                    if(t<=0) continue;
                    if(t<t_min){
                        t_min=t;
                        nearest=k;
                    }
                }
                if(nearest!=-1){
                    double t =objects[nearest]->intersect(refractionRay, refracted_color, level+1);
                    for (int k=0; k<3; k++) {
                        current_color[k] += ref_index * refracted_color[k];
                    }
                }
            }
            for(int k=0;k<3;k++){
                if(current_color[k]<0) current_color[k]=0;
                if(current_color[k]>1) current_color[k]=1;
            }
        }
        return t;
    }

    void draw(){
        //write codes for drawing sphere
        glColor3f(color[0],color[1],color[2]);
        glPushMatrix();
        glTranslatef(reference_point.x,reference_point.y,reference_point.z);
        glutSolidSphere(length,20,20);
        glPopMatrix();
    }
};

class Floor:public Object{
public:
    bitmap_image texture;
    Floor(double FloorWidth,double TileWidth){
        point p(-FloorWidth/2, -FloorWidth/2,0);
        reference_point=p;
        length=TileWidth;
        texture = bitmap_image("texture1.bmp");
    }

    point getNormal(point intersectionPoint)
    {
        return point(0,0,1);
    }

    double getIntersectingT(Ray *r){
		point normal = getNormal(reference_point);
        //return (dot(normal, reference_point)-dot(normal, r->start)) / dot(normal, r->dir);
        return dot(normal, r->start) * (-1) / dot(normal, r->dir);
    }

    double intersect(Ray *r, double *current_color, int level){
        double t= getIntersectingT(r);
        if(t<=0) return -1;
        point intersectionPoint = r->start+ (r->dir)*t;
        if(intersectionPoint.x<reference_point.x || intersectionPoint.y<reference_point.y || intersectionPoint.x > -reference_point.x || intersectionPoint.y> -reference_point.y){
            return -1;
        }

        if(level==0) return t;

        int x = round((intersectionPoint.x-reference_point.x) / length);
        int y = round((intersectionPoint.y-reference_point.y) / length);

        if ((x+y)%2==0){
            color[0] = color[1] = color[2] = 0;
        }else{
            color[0] = color[1] = color[2] = 1;
        }

        int x1 = (intersectionPoint.x + abs(reference_point.x)) * texture.width()/1000;
        int y1 = (intersectionPoint.y + abs(reference_point.y)) * texture.height()/1000;

        unsigned char a, b, c;
        texture.get_pixel(x1, y1, a, b, c);

        current_color[0] = color[0] * co_efficients[0] * a / 255.0;
        current_color[1] = color[1] * co_efficients[0] * b / 255.0;
        current_color[2] = color[2] * co_efficients[0] * c / 255.0;

        point normal = getNormal(intersectionPoint);
        point reflection=getReflection(r, intersectionPoint);


        for(int i=0;i<lights.size();i++){
            point direction = lights[i]-intersectionPoint;
            double len = dot(direction, direction);
            direction=direction.normalize();

            point start= intersectionPoint + direction*1;
            Ray *L=new Ray(start, direction);

            bool obscured=false;
            for(int j=0;j<objects.size();j++){
                double t = objects[j]->getIntersectingT(L);
                if(t>0&&t<len){
                    obscured=true;
                    break;
                }
            }

            if(!obscured){
                double lambert = max(0.0, dot(L->dir, normal));
                double phong = max(0.0, pow(dot(reflection, r->dir), shine));

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[1] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[2] * color[k];
                }
            }

            if(level<recursion_level){
                start=intersectionPoint+reflection*1;
                Ray *reflectionRay= new Ray(start, reflection);

                int nearest = -1;
                double t_min = 100000000;
                double *reflected_color=new double[3];
                for(int k=0;k<objects.size();k++){
                    double t =objects[k]->getIntersectingT(reflectionRay);
                    if(t<=0) continue;
                    if(t<t_min){
                        t_min=t;
                        nearest=k;
                    }
                }
                if(nearest!=-1){
                    double t =objects[nearest]->intersect(reflectionRay, reflected_color, level+1);
                    for (int k=0; k<3; k++) {
                        current_color[k] += co_efficients[3] * reflected_color[k];
                    }
                }
            }
            for(int k=0;k<3;k++){
                if(current_color[k]<0) current_color[k]=0;
                if(current_color[k]>1) current_color[k]=1;
            }
        }


        return t;
    }

    void draw() {
        //write codes for drawing black and white floor
        int n = abs(reference_point.x*2/length);
        int seq = 0;

        for (int i=0; i<n; i++) {
            for (int j=0; j<n; j++) {
                if ((i+j)%2==0) {
                    glColor3f(0, 0, 0);
                } else {
                    glColor3f(1, 1, 1);
                }
                glBegin(GL_QUADS);
                {
                    glVertex3f(reference_point.x+length*i, reference_point.y+length*j, 0);
                    glVertex3f(reference_point.x+length*(i+1), reference_point.y+length*j, 0);
                    glVertex3f(reference_point.x+length*(i+1), reference_point.y+length*(j+1), 0);
                    glVertex3f(reference_point.x+length*i, reference_point.y+length*(j+1), 0);
                }
                glEnd();
            }
        }
    }

};

class Triangle:public Object
{
public:
    point a,b,c;
    Triangle(point a, point b, point c)
    {
        this->a=a;
        this->b=b;
        this->c=c;
    }

    point getNormal(point intersectionPoint)
    {
        point p=cross(b-a, c-a);
        return p.normalize();
    }

    double getIntersectingT(Ray *r){
        const float EPSILON = 0.0000001;
		point edge1, edge2, h, s, q;
        float aa,f,u,v;
        edge1 = b - a;
        edge2 = c - a;
        h = cross(r->dir, edge2);
        aa = dot(edge1, h);
        if (aa > -EPSILON && aa < EPSILON) return -1;

        f = 1/aa;
        s = r->start - a;
        u = f * dot(s, h);
        if (u < 0.0 || u > 1.0) return -1;

        q = cross(s, edge1);
        v = f * dot(r->dir, q);
        if (v < 0.0 || u + v > 1.0) return -1;

        float t = f * dot(edge2, q);
        if (t > EPSILON) return t;

        else return -1;
    }

    double intersect(Ray *r, double *current_color, int level){
        double t= getIntersectingT(r);
        if(t<=0) return -1;
        if(level==0)return t;

        for(int i=0;i<3;i++) current_color[i]=color[i]*co_efficients[0];

        point intersectionPoint = r->start+ (r->dir)*t;
        point normal = getNormal(intersectionPoint);
        point reflection=getReflection(r, intersectionPoint);


        for(int i=0;i<lights.size();i++){
            point direction = lights[i]-intersectionPoint;
            double len = dot(direction, direction);
            direction=direction.normalize();

            point start= intersectionPoint + direction*1;
            Ray *L=new Ray(start, direction);

            bool obscured=false;
            for(int j=0;j<objects.size();j++){
                double t = objects[j]->getIntersectingT(L);
                if(t>0 && t<len){
                    obscured=true;
                    break;
                }
            }

            if(!obscured){
                double lambert = max(0.0, dot(L->dir, normal));
                double phong = max(0.0, pow(dot(reflection, r->dir), shine));

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[1] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[2] * color[k];
                }
            }

            if(level<recursion_level){
                start=intersectionPoint+reflection*1;
                Ray *reflectionRay= new Ray(start, reflection);

                int nearest = -1;
                double t_min = 100000000;
                double *reflected_color=new double[3];
                for(int k=0;k<objects.size();k++){
                    double t =objects[k]->getIntersectingT(reflectionRay);
                    if(t<=0) continue;
                    if(t<t_min){
                        t_min=t;
                        nearest=k;
                    }
                }
                if(nearest!=-1){
                    double t =objects[nearest]->intersect(reflectionRay, reflected_color, level+1);
                    for (int k=0; k<3; k++) {
                        current_color[k] += co_efficients[3] * reflected_color[k];
                    }
                }
            }
            for(int k=0;k<3;k++){
                if(current_color[k]<0) current_color[k]=0;
                if(current_color[k]>1) current_color[k]=1;
            }
        }

        return t;
    }

    void draw(){
        glColor3f(color[0],color[1],color[2]);
        glBegin(GL_TRIANGLES);

        glVertex3f(a.x, a.y, a.z);
        glVertex3f(b.x, b.y, b.z);
        glVertex3f(c.x, c.y, c.z);

        glEnd();
    }
};

class Quadratic:public Object
{
public:
    double A, B, C, D, E, F, G, H, I, J;
    Quadratic(double arr[10], point reference_point, double length, double width, double height) {
        A = arr[0];
        B = arr[1];
        C = arr[2];
        D = arr[3];
        E = arr[4];
        F = arr[5];
        G = arr[6];
        H = arr[7];
        I = arr[8];
        J = arr[9];

        this->reference_point = reference_point;
        this->height = height;
        this->width = width;
        this->length = length;
    }

    point getNormal(point intersectionPoint)
    {
        point p;
        p.x = 2*A*intersectionPoint.x + D*intersectionPoint.y + F*intersectionPoint.z + G;
        p.y = 2*B*intersectionPoint.y + D*intersectionPoint.x + E*intersectionPoint.z + H;
        p.z = 2*C*intersectionPoint.z + E*intersectionPoint.y + F*intersectionPoint.x + I;
        return p.normalize();
    }

    double getIntersectingT(Ray *r){
		double a,b,c,d;

        a = A * r->dir.x * r->dir.x + B * r->dir.y * r->dir.y + C * r->dir.z * r->dir.z +
            D * r->dir.x * r->dir.y + E * r->dir.y * r->dir.z + F * r->dir.z * r->dir.x;

        b = 2 * (A * r->start.x * r->dir.x + B * r->start.y * r->dir.y + C * r->start.z * r->dir.z) +
            D * (r->start.x * r->dir.y + r->dir.x * r->start.y) +
            E * (r->start.y * r->dir.z + r->dir.y * r->start.z) +
            F * (r->start.z * r->dir.x + r->dir.z * r->start.x) +
            G * r->dir.x + H * r->dir.y + I * r->dir.z;

        c = A * r->start.x * r->start.x + B * r->start.y * r->start.y + C * r->start.z * r->start.z +
            D * r->start.x * r->start.y + E * r->start.y * r->start.z + F * r->start.z * r->start.x +
            G * r->start.x + H * r->start.y + I * r->start.z + J;

        d=b*b-4*a*c;

        if(d<0) return -1;
        double t1=(-b+sqrt(d))/(2*a);
        double t2=(-b-sqrt(d))/(2*a);

        point p1 = r->start + r->dir * t1;
        point p2 = r->start + r->dir * t2;

        bool out1 = (length > 0 && (reference_point.x > p1.x || p1.x > (reference_point.x+length) )||
                      width > 0 && (reference_point.y > p1.y || p1.y > (reference_point.y+width)) ||
                      height > 0 && (reference_point.z > p1.z || p1.z > (reference_point.z+height)));

        bool out2 = (length > 0 && (reference_point.x > p2.x || p2.x > (reference_point.x+length) )||
                      width > 0 && (reference_point.y > p2.y || p2.y > (reference_point.y+width)) ||
                      height > 0 && (reference_point.z > p2.z || p2.z > (reference_point.z+height)));

        if(out1 && out2) return -1;
        else if(out1) return t2;
        else if(out2) return t1;
        else return min(t1, t2);
    }

    double intersect(Ray *r, double *current_color, int level){
        double t= getIntersectingT(r);
        if(t<=0) return -1;
        if(level==0)return t;

        for(int i=0;i<3;i++) current_color[i]=color[i]*co_efficients[0];

        point intersectionPoint = r->start+ (r->dir)*t;
        point normal = getNormal(intersectionPoint);
        point reflection=getReflection(r, intersectionPoint);

        for(int i=0;i<lights.size();i++){
            point direction = lights[i]-intersectionPoint;
            double len = dot(direction, direction);
            direction=direction.normalize();

            point start= intersectionPoint + direction*1;
            Ray *L=new Ray(start, direction);

            bool obscured=false;
            for(int j=0;j<objects.size();j++){
                double t = objects[j]->getIntersectingT(L);
                if(t>0 && t<len){
                    obscured=true;
                    break;
                }
            }

            if(!obscured){
                double lambert = max(0.0, dot(L->dir, normal));
                double phong = max(0.0, pow(dot(reflection, r->dir), shine));

                for (int k=0; k<3; k++) {
                    current_color[k] += source_factor * lambert * co_efficients[1] * color[k];
                    current_color[k] += source_factor * phong * co_efficients[2] * color[k];
                }
            }

            if(level<recursion_level){
                start=intersectionPoint+reflection*1;
                Ray *reflectionRay= new Ray(start, reflection);

                int nearest = -1;
                double t_min = 100000000;
                double *reflected_color=new double[3];
                for(int k=0;k<objects.size();k++){
                    double t =objects[k]->getIntersectingT(reflectionRay);
                    if(t<=0) continue;
                    if(t<t_min){
                        t_min=t;
                        nearest=k;
                    }
                }
                if(nearest!=-1){
                    double t =objects[nearest]->intersect(reflectionRay, reflected_color, level+1);
                    for (int k=0; k<3; k++) {
                        current_color[k] += co_efficients[3] * reflected_color[k];
                    }
                }
            }
            for(int k=0;k<3;k++){
                if(current_color[k]<0) current_color[k]=0;
                if(current_color[k]>1) current_color[k]=1;
            }
        }

        return t;
    }

    void draw(){
    }
};


#endif
