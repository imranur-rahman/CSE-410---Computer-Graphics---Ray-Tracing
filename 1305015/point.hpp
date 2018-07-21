#ifndef POINT_H
#define POINT_H

#include <ostream>
using namespace std;

struct point
{
	double x,y,z;
	point() {}
	point(double x, double y, double z)
	{
	    this->x = x;
	    this->y = y;
	    this->z = z;
	}
	point(double ar[3])
	{
	    this->x = ar[0];
	    this->y = ar[1];
	    this->z = ar[2];
	}
	void swap(point& other)
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
    }
    point& operator = (point other)
    {
        swap(other);
        return *this;
    }
    point operator + (point pt) {
        point temp(x + pt.x, y + pt.y, z + pt.z);
        return temp;
    }

    point operator - (point pt) {
        point temp(x - pt.x, y - pt.y, z - pt.z);
        return temp;
    }

    point operator * (double pt) {
        point temp(x * pt, y * pt, z * pt);
        return temp;
    }

    point operator / (double pt) {
        point temp(x / pt, y / pt, z / pt);
        return temp;
    }
    void normalize()
    {
        double d = sqrt(x * x + y * y + z * z);
        x /= d;
        y /= d;
        z /= d;
    }
    friend ostream &operator<<(ostream &output, point pt) {
        output<<pt.x<<","<<pt.y<<","<<pt.z<<endl;
        return output;
    }
};

point operator * (double d, point& pt)
{
    return pt * d;
}

struct Ray{
    point start;
    point dir;

    Ray(point start, point dir)
    {
        this->start = start;
        this->dir = dir;
        this->dir.normalize();
    }
};

#endif // POINT_H
