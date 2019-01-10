#include <stdlib.h>
#include <math.h>

#include <Math/Line.h>

using namespace PGE;

Line2f::Line2f() {
    pointA = Vector2f::zero; pointB = Vector2f::zero;
}

Line2f::Line2f(const Vector2f& a,const Vector2f& b) {
    pointA = a; pointB = b;
}

Line2f::Line2f(float ax,float ay,float bx,float by) {
    pointA = Vector2f(ax,ay); pointB = Vector2f(bx,by);
}

Line2f::Line2f(const Line2i& li) {
    pointA = Vector2f(li.pointA); pointB = Vector2f(li.pointB);
}

Rectanglef Line2f::boundingBox() const {
    return Rectanglef(pointA,pointB);
}

bool Line2f::intersects(const Line2f& other,Vector2f& point,bool segmentOnly) const {
    Vector2f p1a = pointA; Vector2f p1b = pointB;
    Vector2f p2a = other.pointA; Vector2f p2b = other.pointB;

    float denominator = ((p1a.x-p1b.x)*(p2a.y-p2b.y))-((p1a.y-p1b.y)*(p2a.x-p2b.x));
    //If denominator=0, then lines are parallel and cannot intersect.
    //Note: we consider coincident lines to not intersect.
    if (fabs(denominator)<0.001f) { return false; }

    point.x = (((p1a.x*p1b.y)-(p1a.y*p1b.x))*(p2a.x-p2b.x))-((p1a.x-p1b.x)*((p2a.x*p2b.y)-(p2a.y*p2b.x)));
    point.y = (((p1a.x*p1b.y)-(p1a.y*p1b.x))*(p2a.y-p2b.y))-((p1a.y-p1b.y)*((p2a.x*p2b.y)-(p2a.y*p2b.x)));
    point = point.multiply(1.f/denominator);

    if (!segmentOnly) { return true; }
    return boundingBox().isPointInside(point) && other.boundingBox().isPointInside(point);
}

Vector2f Line2f::closestPoint(const Vector2f& point,bool segmentOnly) const {
    Vector2f aToP = point.subtract(pointA);
    Vector2f aToB = pointB.subtract(pointA);

    float t = aToP.dotProduct(aToB)/aToB.lengthSquared();

    if (segmentOnly) {
        if (t<0) { return pointA; }
        if (t>1) { return pointB; }
    }
    return pointA.add(aToB.multiply(t));
}

Line2i::Line2i() {
    pointA = Vector2i::zero; pointB = Vector2i::zero;
}

Line2i::Line2i(const Vector2i& a,const Vector2i& b) {
    pointA = a; pointB = b;
}

Line2i::Line2i(int ax,int ay,int bx,int by) {
    pointA = Vector2i(ax,ay); pointB = Vector2i(bx,by);
}

Line2i::Line2i(const Line2f& li) {
    pointA = Vector2i(li.pointA); pointB = Vector2i(li.pointB);
}

Rectanglei Line2i::boundingBox() const {
    return Rectanglei(pointA,pointB);
}

bool Line2i::intersects(const Line2i& other,Vector2f& point,bool segmentOnly) const {
    Vector2i p1a = pointA; Vector2i p1b = pointB;
    Vector2i p2a = other.pointA; Vector2i p2b = other.pointB;

    float denominator = (float)(((p1a.x-p1b.x)*(p2a.y-p2b.y))-((p1a.y-p1b.y)*(p2a.x-p2b.x)));
    //If denominator=0, then lines are parallel and cannot intersect.
    //Note: we consider coincident lines to not intersect.
    if (fabs(denominator)<0.001f) { return false; }

    point.x = (float)((((p1a.x*p1b.y)-(p1a.y*p1b.x))*(p2a.x-p2b.x))-((p1a.x-p1b.x)*((p2a.x*p2b.y)-(p2a.y*p2b.x))));
    point.y = (float)((((p1a.x*p1b.y)-(p1a.y*p1b.x))*(p2a.y-p2b.y))-((p1a.y-p1b.y)*((p2a.x*p2b.y)-(p2a.y*p2b.x))));
    point = point.multiply(1.f/denominator);

    if (!segmentOnly) { return true; }
    return boundingBox().isPointInside(point) && other.boundingBox().isPointInside(point);
}

