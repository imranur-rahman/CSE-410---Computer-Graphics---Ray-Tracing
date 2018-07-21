#ifndef CLASSES_HPP
#define CLASSES_HPP

//#include<vector>

#define eps 0.00001
enum {AMBIENT, DIFFUSE, SPECULAR, REFLECTION};
int refract_count = 0;
class Point
{
public:
	double x,y,z;

	Point(){
	}
	Point(double a, double b, double c){
        x = a;
        y = b;
        z = c;
	}
	~Point(){}

	double dot(Point v){
        return x*v.x + y*v.y + z*v.z;
	}

	Point operator+(Point pt) {
        return Point(x + pt.x, y + pt.y, z + pt.z);
    }

    Point operator-(Point pt) {
        return Point(x - pt.x, y - pt.y, z - pt.z);
    }

    Point operator*(double pt) {

        return Point(x*pt, y*pt, z*pt);
    }

    Point operator*(Point v){
        return Point(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
    }

    Point operator/(double pt) {

        return Point(x/pt, y/pt, z/pt);
    }

    friend ostream &operator<<(ostream &output, Point pt) {
        output<<pt.x<<","<<pt.y<<","<<pt.z<<endl;
        return output;
    }

    Point normalize() {
        return *this / sqrt(x*x + y*y + z*z);
    }
};

class Ray{
public:
    Point start;
    //Point last;
    Point dir;
    Ray(){}
    Ray(Point start, Point dir){
        this->start = start;

        this->dir = dir.normalize();
    }
    ~Ray(){}
};


extern vector <Point> lights;
extern int recursion_level;

class Object{

public:

    Point reference_point;
    string name;
    bool isRefract;
    double height, width, length, refractive_index = 1.5, source_factor = 1.0;// refIdx = 1.5;

    int shine;

    double color[3];

    double co_efficients[4];

    Object() {}
    ~Object(){}

    virtual void draw() = 0;

    virtual double solveIntersectionT(Ray *ray) = 0;

    virtual Point getNormal(Point intersectionPoint) = 0;

    virtual double intersect(Ray* r, double current_color[3], int level) = 0;

    void setColor(double r, double g, double b) {
        color[0] = r;
        color[1] = g;
        color[2] = b;
    }

    void setShine(int shine) {
        this->shine = shine;
    }



    void setCoEfficients(double a, double d, double s, double r) {
        co_efficients[AMBIENT] = a;
        co_efficients[DIFFUSE] = d;
        co_efficients[SPECULAR] = s;
        co_efficients[REFLECTION] = r;
    }

    double dotProduct(Point p1, Point p2) {
        return p1.dot(p2);
    }

    Point crossProduct(Point p1, Point p2) {
        return p1 * p2;
    }

    Point getReflection(Ray* ray, Point normal) {
        Point reflection = ray->dir - normal * 2.0 * dotProduct(ray->dir, normal);
        return reflection.normalize();
    }

    Point getRefraction(Ray* ray, Point normal) {

        Point refraction(0, 0, 0);

        double n_dot_i = dotProduct(normal, ray->dir);
        double k = 1.0 - refractive_index * refractive_index * (1.0 - n_dot_i * n_dot_i);

        if (k >= 0) {
            refraction = ray->dir * refractive_index - normal * (refractive_index * n_dot_i + sqrt(k));
            refraction = refraction.normalize();
        }

        return refraction;
    }

    void getIllumination(Ray *ray, double current_color[3], int level, double t, vector<Object*> objects){
        Point intersectionPoint = ray->start + ray->dir * t;


        Point normal = this->getNormal(intersectionPoint);
        Point reflection = this->getReflection(ray, normal);
        Point refraction = this->getRefraction(ray, normal);

        for(int i=0;i<lights.size();i++){
            Point direction = lights[i] - intersectionPoint;
            double distance = sqrt(direction.dot(direction));
            direction = direction.normalize();

            Point start = intersectionPoint + direction*1.0;
            Ray L(start, direction);

            //there is something wrong with obstacle
            bool obstacle = false;

            for(int j=0; j < objects.size(); j++){
                double t = objects[j]->solveIntersectionT(&L);
                if(t < 0 || t > distance){
                    continue;
                }
                obstacle = true;
                break;
            }

            if(!obstacle){
                double lambert = this->dotProduct(L.dir, normal);
                Point ref_L = this->getReflection(&L,normal);
                double phong = pow(this->dotProduct(ref_L, ray->dir), (double)this->shine);

                lambert = lambert > 0? lambert:0;
                phong = phong > 0? phong:0;

                for(int k=0;k<3;k++){
                    current_color[k] += this->source_factor * lambert * this->co_efficients[DIFFUSE] * this->color[k];
                    current_color[k] += this->source_factor * phong * this->co_efficients[SPECULAR] * this->color[k];
                }
            }

            if(level < recursion_level){
                start = intersectionPoint + reflection * 1.0;
                Ray reflectionRay(start, reflection);

                int nearest = -1;
                double t_min = MAX_VAL;
                double reflected_color[3];
                for(int k=0;k<objects.size();k++){
                    double t_k = objects[k]->intersect(&reflectionRay, reflected_color, 0);

                    if(t_k <= 0) continue;
                    if(t_k < t_min){
                        t_min = t_k;
                        nearest = k;
                    }
                }

                if(nearest != -1){
                    objects[nearest]->intersect(&reflectionRay, reflected_color, level+1);

                    for(int k=0; k<3; k++){
                        current_color[k] += reflected_color[k] * this->co_efficients[REFLECTION];
                    }
                }

                if(this->name == "sphere" && this->isRefract){
                    start = intersectionPoint + refraction * 1.0;
                    Ray refractionRay(start, refraction);

                    nearest = -1;
                    t_min = MAX_VAL;
                    double refracted_color[3];
                    for(int k=0;k<objects.size();k++){
                        double t_k = objects[k]->intersect(&refractionRay, refracted_color, 0);

                        if(t_k <= 0) continue;
                        if(t_k < t_min){
                            t_min = t_k;
                            nearest = k;
                        }
                    }

                    if(nearest != -1){

                            objects[nearest]->intersect(&refractionRay, refracted_color, level+1);

                            for(int k=0; k<3; k++){
                                current_color[k] += refracted_color[k] * this->refractive_index;
                            }

                    }
                }
            }

            //checking the color boundaries
            for (int k=0; k<3; k++) {
                if (current_color[k] > 1) {
                    current_color[k] = 1;
                } else if (current_color[k] < 0) {
                    current_color[k] = 0;
                }
            }

        }
    }
};


extern vector <Object*>  objects;

class Sphere: public Object{
public:
    Sphere(string name, Point center, double radius){
        reference_point=center;
        length=radius;
        this->name = name;
        isRefract = false;
    }

    void draw(){
        //write codes for drawing sphere
        glColor3f(color[0], color[1], color[2]);

        Point points[100][100];

        double h, r;

        int slices = 24, stacks = 20;



        //generate points
        for(int i=0; i<=stacks; i++) {
            h = length * sin(((double)i/(double)stacks)*(pi/2));
            r = length * cos(((double)i/(double)stacks)*(pi/2));
            for(int j=0; j<=slices; j++) {
                points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
                points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
                points[i][j].z=h;
            }
        }

        //draw quads using generated points
        for(int i=0; i<stacks; i++) {
            for(int j=0; j<slices; j++) {
                glBegin(GL_QUADS);
                {
                    //upper hemisphere
                    glVertex3f(points[i][j].x+reference_point.x,points[i][j].y+reference_point.y,points[i][j].z+reference_point.z);
                    glVertex3f(points[i][j+1].x+reference_point.x,points[i][j+1].y+reference_point.y,points[i][j+1].z+reference_point.z);
                    glVertex3f(points[i+1][j+1].x+reference_point.x,points[i+1][j+1].y+reference_point.y,points[i+1][j+1].z+reference_point.z);
                    glVertex3f(points[i+1][j].x+reference_point.x,points[i+1][j].y+reference_point.y,points[i+1][j].z+reference_point.z);
                    //lower hemisphere
                    glVertex3f(points[i][j].x+reference_point.x,points[i][j].y+reference_point.y,-points[i][j].z+reference_point.z);
                    glVertex3f(points[i][j+1].x+reference_point.x,points[i][j+1].y+reference_point.y,-points[i][j+1].z+reference_point.z);
                    glVertex3f(points[i+1][j+1].x+reference_point.x,points[i+1][j+1].y+reference_point.y,-points[i+1][j+1].z+reference_point.z);
                    glVertex3f(points[i+1][j].x+reference_point.x,points[i+1][j].y+reference_point.y,-points[i+1][j].z+reference_point.z);
                }
                glEnd();
            }
        }
    }

    double solveIntersectionT(Ray *ray){
        Point R0 = ray->start - reference_point;
        double a = 1;
        double b = 2 * dotProduct(ray->dir, R0);
        double c = dotProduct(R0, R0) - length*length;

        double d = b*b - 4*a*c;

        if(d < 0) return -1;
        double t1 = (-b + sqrt(d))/(2.0*a);
        double t2 = (-b - sqrt(d))/(2.0*a);
        double t = t1<t2?t1:t2;
        return t;
    }

    Point getNormal(Point intersectionPoint){
        Point normal = intersectionPoint - this->reference_point;
        return normal.normalize();
    }


    double intersect(Ray *ray, double current_color[3], int level){
        double t = solveIntersectionT(ray);
        if(t<0) return -1;

        if(level == 0) return t;
        /*colorAt[0] = this->color[0];
        colorAt[1] = this->color[1];
        colorAt[2] = this->color[2];*/
        for(int i=0;i<3;i++){
            current_color[i] = this->color[i] * this->co_efficients[AMBIENT];
        }
        this->getIllumination(ray, current_color, level, t, objects);

        return t;

    }

};


class Floor: public Object {

public:



    Floor(string name, double floorWidth, double tileWidth) {
        reference_point = Point(-floorWidth/2, -floorWidth/2, 0);
        length = tileWidth;
        width = floorWidth;
        this->name = name;

    }

    void draw() {

        int numOfTiles = 2.0*width/length;// floorWidth/length

        for (int i=0; i<numOfTiles; i++) {
            for (int j=0; j<numOfTiles; j++) {

                if ((i+j)%2 == 1) {
                    glColor3f(0, 0, 0);
                } else {
                    glColor3f(1, 1, 1);
                }

                glBegin(GL_QUADS);
                {
                    glVertex3f(reference_point.x+length*i, reference_point.y+length*j, reference_point.z);
                    glVertex3f(reference_point.x+length*(i+1), reference_point.y+length*j, reference_point.z);
                    glVertex3f(reference_point.x+length*(i+1), reference_point.y+length*(j+1), reference_point.z);
                    glVertex3f(reference_point.x+length*i, reference_point.y+length*(j+1), reference_point.z);
                }
                glEnd();
            }
        }
    }

    Point getNormal(Point intersectionPoint){
        return Point(0,0,1);
    }



    double solveIntersectionT(Ray *ray){
        Point normal = this->getNormal(this->reference_point);
        double t = ((-1) * this->dotProduct(normal, ray->start))/ this->dotProduct(normal, ray->dir);

        return t;
    }



    double intersect(Ray *ray, double current_color[3], int level){
        double t = solveIntersectionT(ray);
        Point intersectionPoint = ray->start + ray->dir * t;
        intersectionPoint.z = 0;

        double min_x = this->reference_point.x;
        double max_x = -1 * min_x;

        double min_y = this->reference_point.y;
        double max_y = -1 * min_y;
        //cout<<min_x <<", "<< intersectionPoint.x <<", "<<max_x<<endl;
        //cout<<min_y <<", "<< intersectionPoint.y <<", "<<max_y<<endl;
        //cout<<"i m here: "<<t<<endl;
        if(intersectionPoint.x < min_x || intersectionPoint.x > max_x || intersectionPoint.y < min_y || intersectionPoint.y > max_y) return -1;
        int x_coord = (intersectionPoint.x - this->reference_point.x) / length;
        int y_coord = (intersectionPoint.y - this->reference_point.y) / length;
        if(level == 0) return t;

        if((x_coord + y_coord)%2 == 1){
            color[0] = color[1] = color[2] = 0;
        }
        else{
            color[0] = color[1] = color[2] = 1;
        }

        bitmap_image floorTexture("sky.bmp");
        double textHeight = floorTexture.height()/width;
        double textWidth = floorTexture.width()/width;

        int x = (intersectionPoint.x - this->reference_point.x)*textWidth;
        int y = (intersectionPoint.y - this->reference_point.y)*textHeight;

        unsigned char r,g,b;
        floorTexture.get_pixel(x,y,r,g,b);

        double rgb[] = {r,g,b};

        for(int i=0;i<3;i++){
            current_color[i] = this->color[i] * this->co_efficients[AMBIENT] * rgb[i]/255.0;
        }

        this->getIllumination(ray, current_color, level, t, objects);
        return t;

    }
};

class Triangle:public Object{
public:
    Point p1, p2, p3;
    Triangle(string name, Point p1, Point p2, Point p3){
        this->p1 = p1;
        this->p2 = p2;
        this->p3 = p3;
        this->name = name;
    }
    void draw(){
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(p1.x, p1.y, p1.z);
            glVertex3f(p2.x, p2.y, p2.z);
            glVertex3f(p3.x, p3.y, p3.z);

        }
        glEnd();

    }

    Point getNormal(Point intersectionPoint){
        Point A = p2 - p1;
        Point B = p3 - p1;
        Point normal = this->crossProduct(A, B);
        return normal.normalize();
    }

    double getDET(Point e1, Point e2, Point e3){
        return e1.x*(e2.y*e3.z - e2.z*e3.y) - e1.y*(e2.x*e3.z - e2.z*e3.x) + e1.z*(e2.x*e3.y - e2.y*e3.x);
    }

    double solveIntersectionT(Ray *ray){
        Point edge1 = p1 - p2;
        Point edge2 = p1 - p3;
        double det = this->getDET(edge1, edge2, ray->dir);
        //cout << "tri det: "<<det <<endl;
        if(det > -eps && det < eps) return -1;
        Point temp = p1 - ray->start;
        double beta = this->getDET(temp, edge2, ray->dir)/det;
        double gamma = this->getDET(edge1, temp, ray->dir)/det;
        //cout<<"beta "<<beta<<" gamma "<<gamma<<endl;
        if(beta < 0 || gamma < 0 || beta+gamma > 1) return -1;

        double t = this->getDET(edge1, edge2, temp)/det;
        //cout<<"tri t: "<<t<<endl;
        return t;

    }

    double intersect(Ray *ray, double current_color[3], int level){
        double t = this->solveIntersectionT(ray);
        if(t<=0) return -1;
        if(level == 0) return t;
        for(int i=0;i<3;i++){
            current_color[i] = this->color[i] * this->co_efficients[AMBIENT];
        }
        this->getIllumination(ray, current_color,level,t,objects);
        return t;
    }


} ;

class GeneralQuadratic: public Object{
public:
    double A, B, C,D,E,F,G,H,I,J;

    GeneralQuadratic(string name, double coefficient[10], Point refPoint, double length, double width, double height){
        this->A = coefficient[0];
        this->B = coefficient[1];
        this->C = coefficient[2];
        this->D = coefficient[3];
        this->E = coefficient[4];
        this->F = coefficient[5];
        this->G = coefficient[6];
        this->H = coefficient[7];
        this->I = coefficient[8];
        this->J = coefficient[9];
        this->reference_point = refPoint;
        this->length = length;
        this->width = width;
        this->height = height;
        this->name = name;
    }

    void draw(){}

    Point getNormal(Point intersectionPoint){
        double dx = 2 * A * intersectionPoint.x + D * intersectionPoint.y + F * intersectionPoint.z + G;
        double dy = 2 * B * intersectionPoint.y + D * intersectionPoint.x + E * intersectionPoint.z + H;
        double dz = 2 * C * intersectionPoint.z + E * intersectionPoint.y + F * intersectionPoint.x + I;

        Point normal(dx, dy, dz);
        return normal.normalize();
    }

    double solveIntersectionT(Ray *ray){
        double a = A*ray->dir.x*ray->dir.x + B*ray->dir.y*ray->dir.y + C*ray->dir.z*ray->dir.z + D * ray->dir.x * ray->dir.y + E * ray->dir.y * ray->dir.z + F * ray->dir.z * ray->dir.x;
        double b = 2 * (A * ray->start.x * ray->dir.x + B * ray->start.y * ray->dir.y + C * ray->start.z * ray->dir.z) + D * (ray->start.x * ray->dir.y + ray->dir.x * ray->start.y) + E * (ray->start.y * ray->dir.z + ray->dir.y * ray->start.z) + F * (ray->start.z * ray->dir.x + ray->dir.z * ray->start.x) + G * ray->dir.x + H * ray->dir.y + I * ray->dir.z;
        double c = A * ray->start.x * ray->start.x + B * ray->start.y * ray->start.y + C * ray->start.z * ray->start.z + D * ray->start.x * ray->start.y + E * ray->start.y * ray->start.z + F * ray->start.z * ray->start.x + G * ray->start.x + H * ray->start.y + I * ray->start.z + J;
        double d = b*b - 4*a*c;

        if(d<0) return -1;

        double t1 = (-b + sqrt(d))/(2.0*a);
        double t2 = (-b - sqrt(d))/(2.0*a);

        Point intersectionPoint1 = ray->start + ray->dir * t1;
        Point intersectionPoint2 = ray->start + ray->dir * t2;

        double min_x = this->reference_point.x;
        double max_x = min_x + length;

        double min_y = this->reference_point.y;
        double max_y = min_y + width;

        double min_z = this->reference_point.z;
        double max_z = min_z + height;

        bool isNotT1 = length > 0 && (intersectionPoint1.x < min_x || intersectionPoint1.x > max_x) || width > 0 && (intersectionPoint1.y < min_y || intersectionPoint1.y > max_y) || height > 0 && (intersectionPoint1.z < min_z || intersectionPoint1.z > max_z);
        bool isNotT2 = length > 0 && (intersectionPoint2.x < min_x || intersectionPoint2.x > max_x) || width > 0 && (intersectionPoint2.y < min_y || intersectionPoint2.y > max_y) || height > 0 && (intersectionPoint2.z < min_z || intersectionPoint2.z > max_z);

        if (isNotT1 && isNotT2) return -1;
        else if (isNotT1) return t2;
        else if(isNotT2) return t1;
        else{
            t1 = t1 < t2? t1:t2;
            return t1;
        }

    }

    double intersect(Ray *ray, double current_color[3], int level){
        double t = this->solveIntersectionT(ray);
        //cout<<t<<endl;
        if(t <= 0) return -1;
        if(level == 0) return t;
        for(int i=0;i<3;i++){
            current_color[i] = this->color[i] * this->co_efficients[AMBIENT];
        }
        this->getIllumination(ray, current_color, level, t, objects);
        return t;

    }

};


#endif // CLASSES_HPP
