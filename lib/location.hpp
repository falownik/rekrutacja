#include <iostream>
#include <vector>
#include <cstdbool>

//struct of given points 
struct Point_id
{
double x;
double y;
int tag_id;
};
//struct to hold points on circuit of circle
struct Point
{
double x;
double y;
};
class PointsApp
{
public:
    int tag_id;
    int appearance;
    PointsApp(){}
    bool operator<(const PointsApp& op1){return appearance < op1.appearance;}
};
class PointsAppearance
{
public:
    PointsAppearance(){}
    std::vector <PointsApp> pointsApp;
};
class InPolygon
{
public:
    InPolygon(){}
    int tag_id;
    std::vector <bool> isInPolygon = std::vector <bool> (5);
    Point_id point;
    bool operator<(const InPolygon& op1){return tag_id < op1.tag_id;}
};
class Polygon
{
    public:
    std::vector<Point> node;
    int numberOfNodes;
};
class Circle
{
public:
    //Radius must be further than any point from origin
    double radius;
    Point point;
    Circle(double r = 2.0)
    :radius(r){}
    Point getPoint(int division, int part);
};

class Result
{
    public:
    std::vector <InPolygon> data;
    Result(){}
    void printAlltoFile(std::string str);
    void printAlltoFileWithoutCoordinates(std::string str);
    void printAllTagsInPolygons(std::string str);
    void printTagsInPolygonsConjunction(std::string str);
    void printTagsInPolygonsDisjunction(std::string str);
    void printStatistics(std::string str);
    void sumAll();
    void sort();
};
class Calculation
{
public:
    std::vector<Point_id> point_id;
    std::vector<Polygon> polygon;
    Calculation(){}
    void getPoints();
    void getPolygons();
    Result doIntersect(const int& precision);
    bool intersect(Point pointOfCircle, Point_id point, Point& node1, Point& node2);
    void printAlltoFile();

};