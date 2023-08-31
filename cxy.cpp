#include <vector>
#include <iostream>
#include <math.h>
#include "cxy.h"

cxy cxy::vect(const cxy &other) const
{
    cxy v;
    v.x = other.x - x;
    v.y = other.y - y;
    return v;
}

double cxy::dist2(const cxy &other) const
{
    cxy v = vect(other);
    return v.x * v.x + v.y * v.y;
}

double cxy::closest(
    const cxy &line1,
    const cxy &line2) const
{
    cxy AB = line1.vect(line2);
    cxy AP = line1.vect(*this);
    double lAB = AB.x * AB.x + AB.y * AB.y;
    return (AB.x * AP.x + AB.y * AP.y) / lAB;
}

double cxy::dis2toline(
    const cxy &line1,
    const cxy &line2) const
{
    double t = closest( line1, line2 );
    if (t < 0)
        t = 0;
    if (t > 1)
        t = 1;
    cxy AB = line1.vect(line2);
    cxy closest(
        line1.x + t * AB.x,
        line1.y + t * AB.y );
    return dist2(closest);
}

bool cxy::isInside(const std::vector<cxy> &polygon) const
{
    int c = 0;
    std::vector<cxy>::const_iterator j = polygon.end() - 1;
    for (std::vector<cxy>::const_iterator i = polygon.begin();
         i != polygon.end(); j = i++)
    {
        if (((i->y > y) != (j->y > y)) &&
            (x < (j->x - i->x) * (y - i->y) / (j->y - i->y) + i->x))
            c = !c;
    }
    return (c == 1);
}

cxy cxy::enclosingWidthHeight(
    const std::vector<cxy> &polygon)
{
    cxy max, min;
    for (const cxy &p : polygon)
    {
        if (p.x > max.x)
            max.x = p.x;
        if (p.y > max.y)
            max.y = p.y;
        if (p.x < min.x)
            min.x = p.x;
        if (p.y < min.y)
            min.y = p.y;
    }
    static cxy ret;
    ret.x = max.x - min.x;
    ret.y = max.y - min.y;
    return ret;
}

bool cxy::isIntersection(cxy &p,
                         const cxy &a, const cxy &b,
                         const cxy &c, const cxy &d)
{

    /*
    https://www.topcoder.com/thrive/articles/Geometry%20Concepts%20part%202:%20%20Line%20Intersection%20and%20its%20Applications

Ax+By=C

A = y2-y1
B = x1-x2
C = Ax1+By1

    */
    double A1 = b.y - a.y;
    double B1 = a.x - b.x;
    double C1 = A1 * a.x + B1 * a.y;
    double A2 = d.y - c.y;
    double B2 = c.x - d.x;
    double C2 = A2 * c.x + B2 * c.y;

    /**
     * intersection point
     *
     * double det = A1 * B2 - A2 * B1
    if (det == 0) {
      //Lines are parallel
    } else {
      double x = (B2 * C1 - B1 * C2) / det
      double y = (A1 * C2 - A2 * C1) / det
    }
     *
     */
    double det = A1 * B2 - A2 * B1;
    if (fabs(det) < 0.0001)
        return false;
    else
    {
        // intersection point of infinite lines
        p.x = (B2 * C1 - B1 * C2) / det;
        p.y = (A1 * C2 - A2 * C1) / det;

        // check segments intersect
        if (!(std::min(a.x, b.x) <= p.x && p.x <= std::max(a.x, b.x)))
            return false;
        if (!(std::min(a.y, b.y) <= p.y && p.y <= std::max(a.y, b.y)))
            return false;

        return true;
    }
}

double cxy::angle(const cxy &a, const cxy &b,
                  const cxy &c, const cxy &d)
{
    cxy v1 = a.vect(b);
    cxy v2 = c.vect(d);
    double dot = v1.x * v2.x + v1.y * v2.y;
    double det = v1.x * v2.y - v1.y * v2.x;
    return atan2(det, dot);
}
