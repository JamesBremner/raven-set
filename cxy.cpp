#include <vector>
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
