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

double cxy::dis2toline(
    const cxy &line1,
    const cxy &line2) const
{
    cxy AB = line1.vect(line2);
    cxy AP = line1.vect(*this);
    double lAB = AB.x * AB.x + AB.y * AB.y;
    double t = (AB.x * AP.x + AB.y * AP.y) / lAB;
    if (t < 0)
        t = 0;
    if (t > 1)
        t = 1;
    cxy closest;
    closest.x = line1.x + t * AB.x;
    closest.y = line1.y + t * AB.y;
    return dist2(closest);
}

bool cxy::isInside( const std::vector<cxy>& polygon ) const
{
    int c = 0;
    std::vector<cxy>::const_iterator j = polygon.end()-1;
    for ( std::vector<cxy>::const_iterator i= polygon.begin();
            i != polygon.end(); j = i++ )
    {
        if( (( i->y > y ) != ( j->y > y )) &&
                (x < ( j->x - i->x ) * ( y - i->y ) / ( j->y - i->y ) + i->x ) )
            c  = ! c;
    }
    return ( c == 1 );
}

cxy cxy::enclosingWidthHeight(
     const std::vector<cxy>& polygon )
     {
        cxy max, min;
        for( const cxy& p : polygon ) {
            if( p.x > max.x )
                max.x = p.x;
            if( p.y > max.y )
                max.y = p.y;
            if( p.x < min.x )
                min.x = p.x;
            if( p.y < min.y )
                min.y = p.y;
        }
        static cxy ret;
        ret.x = max.x - min.x;
        ret.y = max.y - min.y;
        return ret;
     }

    bool cxy::isIntersection( cxy& p,
                     const cxy& a, const cxy& b,
                     const cxy& c, const cxy& d )
    {
        float s1_x, s1_y, s2_x, s2_y;

        s1_x = b.x - a.x;
        s1_y = b.y - a.y;
        s2_x = d.x - c.x;
        s2_y = d.y - c.y;

        float s, t;
        s = (-s1_y * (a.x -c.x) + s1_x * (a.y - c.y)) /
            (-s2_x * s1_y + s1_x * s2_y);
        t = (s2_x * (a.x - c.x) - s2_y * (a.x - c.x)) /
            (-s2_x * s1_y + s1_x * s2_y);

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
        {
            // Collision detected
            p.x = a.x + (t * s1_x);
            p.y = a.y + (t * s1_y);
            return true;
        }

        return false; // No collision
    }

    double cxy::angle(  const cxy& a, const cxy& b,
               const cxy& c, const cxy& d )
{
    cxy v1 = a.vect( b );
    cxy v2 = c.vect( d );
    double dot = v1.x* v2.x + v1.y*v2.y;
    double det = v1.x*v2.y - v1.y*v2.x;
    return atan2(det, dot);
}
