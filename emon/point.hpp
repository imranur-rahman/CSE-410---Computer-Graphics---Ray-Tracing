#ifndef POINT_INCLUDED
#define POINT_INCLUDED

class point
{
public:
	double x,y,z;
	point(){};
	point(double x,double y, double z)
	{
	    this->x=x;
	    this->y=y;
	    this->z=z;
	}
	point operator -(point const &q)
    {
        point t;
        t.x=x-q.x;
        t.y=y-q.y;
        t.z=z-q.z;
        return t;
    }
    point operator +(point const &q)
    {
        point t;
        t.x=x+q.x;
        t.y=y+q.y;
        t.z=z+q.z;
        return t;
    }
    point operator *(point const &q)
    {
        point t;
        t.x=x*q.x;
        t.y=y*q.y;
        t.z=z*q.z;
        return t;
    }
    point operator *(double a)
    {
        point t;
        t.x=x*a;
        t.y=y*a;
        t.z=z*a;
        return t;
    }
    point operator /(double a)
    {
        point t;
        t.x=x/a;
        t.y=y/a;
        t.z=z/a;
        return t;
    }
    double dot(point a)
    {
        return (x*a.x+y*a.y+z*a.z);
    }
    point normalize()
    {
        point t;
        t.x=x/sqrt(x*x+y*y+z*z);
        t.y=y/sqrt(x*x+y*y+z*z);
        t.z=z/sqrt(x*x+y*y+z*z);
        return t;
    }
    void draw()
    {
        glColor3f(1.0, 1.0, 0);
        double t=2;
        glBegin(GL_QUADS);
        {
            glVertex3f(x+t, y, z+t);
            glVertex3f(x+t, y, z-t);
            glVertex3f(x-t, y, z-t);
            glVertex3f(x-t, y, z+t);
        }
        glEnd();
    }
};

class Ray{
public:
	point start;
	point dir;
	//write appropriate constructor
	Ray(point s, point d)
	{
	    start=s;
	    dir=d;
	}
};


#endif
