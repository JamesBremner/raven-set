
#pragma once
#include <cfloat>

/// @brief 2D point or vector
class cxy
{

public:
    double x;
    double y;

    cxy()
        : x(-DBL_MAX),
        y(-DBL_MAX)
    {
    }
    cxy(double X, double Y)
        : x(X), y(Y)
    {
    }
    /// @brief vector from this point to other
    /// @param other
    /// @return
    cxy vect(const cxy &other) const;

    /// @brief distance squared from this point to other
    /// @param other
    /// @return
    double dist2(const cxy &other) const;

    // closest point on line to this point, fraction ( point = end1 + t * (end2-end1) )
    // return t
    double tclosest(
        const cxy &end1,
        const cxy &end2) const;

    // closest point on line segment to this point
    cxy closest(
        const cxy &end1,
        const cxy &end2) const;

    /// @brief distance squared from this point to nearest point on line segment
    /// @param end1 line segment endpoint
    /// @param end2 line segment endpoint
    /// @return
    double dis2toline(
        const cxy &end1,
        const cxy &end2) const;

    /// true if point inside polygon
    bool isInside(const std::vector<cxy> &polygon) const;

    static cxy enclosingWidthHeight(const std::vector<cxy> &polygon);

    /** true if line segments intersect
        @param[out] p point of intersection
        @param[in] a,b line segment
        @param[in] c,d line segment
        @return true if line segments intersect
    */
    static bool isIntersection(cxy &p,
                               const cxy &a, const cxy &b,
                               const cxy &c, const cxy &d);

    static bool isIntersect(cxy &p1, cxy &q1, cxy &p2, cxy &q2);


    /// @brief angle between line segments, radians
    /// @param a,b line segment
    /// @param c,d line segment
    /// @return angle in radians

    static double angle(
        const cxy &a, const cxy &b,
        const cxy &c, const cxy &d);

    /// @brief clockwise turn going from a to b to c, radians
    /// @param a 
    /// @param b 
    /// @param c 
    /// @return angle in radians

    static double clockwise(
    const cxy &a,
    const cxy &b,
    const cxy &c);

    bool isValid() const
    {
        return ( ( x > -DBL_MAX+1) && ( y > -DBL_MAX+1 ));
    }
    void invalidate()
    { 
        x = -DBL_MAX;
        y = -DBL_MAX;
    }
    bool operator==(const cxy &other) const
    {
        return x == other.x && y == other.y;
    }
    friend std::ostream &operator<<(std::ostream &os, cxy p)
    {
        os << "(" << p.x << "," << p.y << ")";
        return os;
    }
};