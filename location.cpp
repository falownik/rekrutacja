/// @author Marcin Michalski

#include "location.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <algorithm>

Point Circle::getPoint(int division, int part)
{
    Point point;
    point.x = radius * cos((2 * M_PI * part) / division);
    point.y = radius * sin((2 * M_PI * part) / division);
    if (abs(point.x) < 0.000001)
        point.x = 0;
    if (abs(point.y) < 0.000001)
        point.y = 0;
    return point;
}

//Read points from file
void Calculation::getPoints()
{
    std::ifstream file;
    file.open("data/point_tag_id.txt");
    if (!file)
    {
        std::cout << "something went wrong with point_tag_id.txt";
        return;
    }
    Point_id point;
    while (!file.eof())
    {
        file >> point.tag_id;
        file >> point.x;
        file >> point.y;
        point_id.push_back(point);
    }
    file.close();
}

//Read polygons
void Calculation::getPolygons()
{
    //Read polygon nodes
    std::ifstream file;
    file.open("data/polygon_node_location.txt");
    if (!file)
    {
        std::cout << "something went wrong with polygon_node_location.txt";
        return;
    }
    Point_id point;
    std::vector<Point_id> node;
    while (!file.eof())
    {
        file >> point.tag_id;
        file >> point.x;
        file >> point.y;
        node.push_back(point);
    }
    file.close();

    //Match polygon with nodes
    Polygon poly;
    Point p;
    int brackets = 0;
    int polCount = 0;
    int temp;
    char s;
    file.open("data/polygon_node_id.txt");
    if (!file)
    {
        std::cout << "something went wrong with polygon_node_id.txt";
        return;
    }
    while (!file.eof())
    {
        if (file.fail() && brackets == 0)
        {
            file.clear();
            file.get(s);
            brackets = 1;
        }
        else if (file.fail() && brackets == 1)
        {
            file.clear();
            file.get(s);
            file >> temp;
            polygon.push_back(poly);
            poly.node.erase(poly.node.begin(), poly.node.end());
            brackets = 0;
            polCount++;
        }
        else
        {
            while (file >> temp && brackets == 1)
            {
                p.x = node[temp].x;
                p.y = node[temp].y;
                poly.node.push_back(p);
            }
        }
    }
}

//Given 4 points check whether tese points makes lines intersecting within
bool Calculation::intersect(Point pointOfCircle, Point_id point, Point &node1, Point &node2)
{

    double a1, a2, b1, b2;
    double x_intersect, y_intersect;
    if (pointOfCircle.x != point.x)
    {
        a1 = (pointOfCircle.y - point.y) / (pointOfCircle.x - point.x);
        b1 = (pointOfCircle.x * point.y - point.x * pointOfCircle.y) / (pointOfCircle.x - point.x);
    }
    if (node1.x != node2.x)
    {
        a2 = (node1.y - node2.y) / (node1.x - node2.x);
        b2 = (node1.x * node2.y - node2.x * node1.y) / (node1.x - node2.x);
    }
    if (pointOfCircle.x != point.x && node1.x != node2.x)
    {
        x_intersect = (b2 - b1) / (a1 - a2);
        y_intersect = (a1 * b2 - a2 * b1) / (a1 - a2);
    }
    else if (pointOfCircle.x == point.x && node1.x != node2.x)
    {
        x_intersect = point.x;
        y_intersect = a2 * point.x + b2;
    }
    else if (pointOfCircle.x != point.x && node1.x == node2.x)
    {
        x_intersect = node1.x;
        y_intersect = a1 * node1.x + b1;
    }
    else
    {
        std::cout << "parallel and horizontal";
        return false;
    }
    if (a1 == a2)
    {
        std::cout << "parallel";
        return false;
    }

    double x11, x12, x21, x22;
    double y11, y12, y21, y22;

    if (node1.x > node2.x)
    {
        x12 = node1.x;
        x11 = node2.x;
    }
    else
    {
        x12 = node2.x;
        x11 = node1.x;
    }
    if (node1.y > node2.y)
    {
        y12 = node1.y;
        y11 = node2.y;
    }
    else
    {
        y12 = node2.y;
        y11 = node1.y;
    }

    if (pointOfCircle.x > point.x)
    {
        x22 = pointOfCircle.x;
        x21 = point.x;
    }
    else
    {
        x22 = point.x;
        x21 = pointOfCircle.x;
    }
    if (pointOfCircle.y > point.y)
    {
        y22 = pointOfCircle.y;
        y21 = point.y;
    }
    else
    {
        y22 = point.y;
        y21 = pointOfCircle.y;
    }

    if (((x_intersect > x11 && x_intersect < x12) || (y_intersect > y11 && y_intersect < y12)) && ((x_intersect > x21 && x_intersect < x22) || (y_intersect > y21 && y_intersect < y22)))
    {
        return true;
    }
    else
        return false;
}

//Check if given point's inside polygon, odd number of intersections - point inside polygon
//precision is variable for multiple check of intersection, when precision == n method checks
//intersections for n points on the circumference of circle. Circle must contain all points
//and polygons! (radius further than furthest point and node)
Result Calculation::doIntersect(const int &precision)
{
    Circle circle;
    InPolygon inPolygon;
    Result result;
    bool isIn = false;
    for (auto i = 0; i < point_id.size(); i++)
    {
        inPolygon.tag_id = point_id[i].tag_id;
        inPolygon.point = point_id[i];
        for (auto j = 0; j < polygon.size(); j++)
        {
            for (auto k = 0; k < precision; k++)
            {
                isIn = false;
                for (auto l = 0; l < polygon[j].node.size() - 1; l++)
                {
                    isIn ^= this->intersect(circle.getPoint(precision, k), point_id[i], polygon[j].node[l], polygon[j].node[l + 1]);
                }
            }
            inPolygon.isInPolygon[j] = isIn;
        }
        result.data.push_back(inPolygon);
    }
    return result;
}

//Check how many points were in each polygon
void Result::sumAll()
{
    int sum = 0;
    for (auto j = 0; j < 5; j++)
    {
        std::cout << j << ": ";
        for (auto i = 0; i < data.size(); i++)
        {
            sum += data[i].isInPolygon[j];
        }
        std::cout << sum << std::endl;
        sum = 0;
    }
}

//Auxiliary method to print all coordinates in brackets (for later visualization)
void Calculation::printAlltoFile()
{
    std::ofstream file;
    file.open("results/pointsFormated.txt");
    for (auto i = 0; i < point_id.size(); i++)
        file << "(" << point_id[i].x << "," << point_id[i].y << ")";
    file.close();
}

void Result::sort()
{
    std::sort(data.begin(), data.end());
}

//Print all points in given order with coordinates
void Result::printAlltoFile(std::string str)
{
    std::ofstream file;
    file.open("results/" + str);
    if (!file)
        std::cout << "something went wrong with " << str << std::endl;
    file << "tag_id polygon       coordinates" << std::endl;
    file << "      0 1 2 3 4        x                  y" << std::endl;
    file << "---------------------------------------------------------------" << std::endl;
    for (auto i = 0; i < data.size(); i++)
    {
        file << "  ";
        file.fill(' ');
        file.width(4);
        file << std::left << data[i].tag_id;
        for (auto j = 0; j < 5; j++)
            file << data[i].isInPolygon[j] << " ";

        file << std::setprecision(17);
        file << "   " << std::setw(20) << data[i].point.x << "   " << std::setw(20) << data[i].point.y;
        file << std::endl;
    }
    file.close();
}

//Print all points in given order without coordinates
void Result::printAlltoFileWithoutCoordinates(std::string str)
{
    std::ofstream file;
    file.open("results/" + str);
    if (!file)
        std::cout << "something went wrong with " << str << std::endl;
    file << "tag_id polygon" << std::endl;
    file << "      0 1 2 3 4" << std::endl;
    file << "---------------" << std::endl;
    for (auto i = 0; i < data.size(); i++)
    {
        file << "  ";
        file.fill(' ');
        file.width(4);
        file << std::left << data[i].tag_id;
        for (auto j = 0; j < 5; j++)
            file << data[i].isInPolygon[j] << " ";
        file << std::endl;
    }
    file.close();
}

//Print conjunction of tag_ids contained in polygon
void Result::printTagsInPolygonsConjunction(std::string str)
{
    this->sort();
    InPolygon inPolygon;
    Result result;
    inPolygon.tag_id = 0;
    for (auto a : inPolygon.isInPolygon)
        a = true;

    for (auto i = 0; i < data.size(); i++)
    {
        if (inPolygon.tag_id != data[i].tag_id)
        {
            result.data.push_back(inPolygon);
            inPolygon.tag_id = data[i].tag_id;
            for (auto a : inPolygon.isInPolygon)
                a = true;
        }
        for (auto j = 0; j < inPolygon.isInPolygon.size(); j++)
            inPolygon.isInPolygon[j] = inPolygon.isInPolygon[j] & data[i].isInPolygon[j];
    }
    result.data.push_back(inPolygon);
    result.printAlltoFileWithoutCoordinates("iloczyn logiczny punktow.txt");

    std::ofstream file;
    file.open("results/" + str);
    if (!file)
        std::cout << "something went wrong with " << str << std::endl;

    for (auto i = 0; i < inPolygon.isInPolygon.size(); i++)
    {
        file << "In polygon no. " << i << " are tag_ids:" << std::endl
             << std::endl;
        for (auto j = 0; j < result.data.size(); j++)
        {
            if (result.data[j].isInPolygon[i])
                file << result.data[j].tag_id << " ";
        }
        file << std::endl;
    }
    file.close();
}

//Print disjunction of tag_ids contained in polygon
void Result::printTagsInPolygonsDisjunction(std::string str)
{
    this->sort();
    InPolygon inPolygon;
    Result result;
    inPolygon.tag_id = 0;
    for (auto a : inPolygon.isInPolygon)
        a = false;

    for (auto i = 0; i < data.size(); i++)
    {
        if (inPolygon.tag_id != data[i].tag_id)
        {
            result.data.push_back(inPolygon);
            inPolygon.tag_id = data[i].tag_id;
            for (auto a : inPolygon.isInPolygon)
                a = false;
        }
        for (auto j = 0; j < inPolygon.isInPolygon.size(); j++)
            inPolygon.isInPolygon[j] = inPolygon.isInPolygon[j] | data[i].isInPolygon[j];
    }
    result.data.push_back(inPolygon);
    result.printAlltoFileWithoutCoordinates("suma logiczna punktow.txt");

    std::ofstream file;
    file.open("results/" + str);
    if (!file)
        std::cout << "something went wrong with " << str << std::endl;

    for (auto i = 0; i < result.data[0].isInPolygon.size(); i++)
    {
        file << "In polygon no. " << i << " are tag_ids:" << std::endl
             << std::endl;
        file << "tag_id: " << std::endl;
        file << "          " << std::endl;
        file << "--------" << std::endl;
        for (auto j = 0; j < result.data.size(); j++)
        {
            if (result.data[j].isInPolygon[i])
                if (!(i == 1 && (result.data[j].isInPolygon[2] == true)))
                {
                    file << "  " << std::setw(4) << std::left << result.data[j].tag_id << " ";
                    file << std::setprecision(17);
                    file << std::endl;
                }
        }
        file << std::endl;
    }
    file.close();
}

//Print tag_ids contained in polygon with repetition
void Result::printAllTagsInPolygons(std::string str)
{
    this->sort();

    std::ofstream file;
    file.open("results/" + str);
    if (!file)
        std::cout << "something went wrong with " << str << std::endl;

    for (auto i = 0; i < data[0].isInPolygon.size(); i++)
    {
        file << "In polygon no. " << i << " are tag_ids:" << std::endl
             << std::endl;
        file << "tag_id       coordinates" << std::endl;
        file << "           x                      y" << std::endl;
        file << "---------------------------------------------------------------" << std::endl;
        for (auto j = 0; j < data.size(); j++)
        {
            if (data[j].isInPolygon[i])
                if (!(i == 1 && (data[j].isInPolygon[2] == true)))
                {
                    file << "  " << std::setw(4) << std::left << data[j].tag_id << " ";
                    file << std::setprecision(17);
                    file << "   " << std::setw(20) << std::left << data[j].point.x << "   " << std::setw(20) << std::left << data[j].point.y;
                    file << std::endl;
                }
        }
        file << std::endl;
    }
    file.close();
}

//Most of tag_ids appearance, tag_ids not contained
void Result::printStatistics(std::string str)
{
    PointsAppearance pointsAppearance;
    PointsApp pointsApp;
    int pointsBeyond = 0;
    std::ofstream file;
    file.open("results/" + str);
    if (!file)
        std::cout << "something went wrong with " << str << std::endl;

    for (auto i = 0; i < data.size() ; i++)
    {
        int sum = 0;
        for (auto j = 0; j < data[0].isInPolygon.size(); j++)
            sum += data[i].isInPolygon[j];
        if (sum == 0)
            pointsBeyond++;
    }
        
    int tag_id = 0;
    int appearances = 0;
    this->sort();

    for (auto j = 0; j < data.size(); j++)
    {
        if (tag_id != data[j].tag_id)
        {
            pointsApp.tag_id = tag_id;
            pointsApp.appearance = appearances;
            pointsAppearance.pointsApp.push_back(pointsApp);
            tag_id = data[j].tag_id;
            appearances = 0;
        }

        for (auto i = 0; i < data[0].isInPolygon.size(); i++)
        {
            appearances += data[j].isInPolygon[i];
        }
    }
            pointsApp.tag_id = tag_id;
            pointsApp.appearance = appearances;
            pointsAppearance.pointsApp.push_back(pointsApp);

    std::sort(pointsAppearance.pointsApp.begin(), pointsAppearance.pointsApp.end());
    file << " Tag_ids with most appearances" << std::endl << std::endl;
    file << "tag_id       no. of apearances" << std::endl;
    file << "-------------------------------" << std::endl;
    for (auto i = pointsAppearance.pointsApp.size() - 1; i > pointsAppearance.pointsApp.size() - 8 ; i--)
        file << "   " << std::setw(4) << std::left << pointsAppearance.pointsApp[i].tag_id << "         " << pointsAppearance.pointsApp[i].appearance << std::endl;
    file << std::endl << "No. of tag_ids without appearance in polygons: " << pointsBeyond << std::endl;
    file.close();
}

int main(void)
{
    int precision = 1;
    Circle circle;
    Calculation cal;
    cal.getPoints();
    cal.getPolygons();

    Result result = cal.doIntersect(precision);
    result.printStatistics("statystyka.txt");
    result.printTagsInPolygonsConjunction("iloczyn logiczny w wielokatach.txt");
    result.printTagsInPolygonsDisjunction("suma logiczna w wielokatach.txt");
    result.printAllTagsInPolygons("wszystkie punkty w danych wielokatach.txt");
    return 0;
}