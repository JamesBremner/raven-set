
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
        return ((x > -DBL_MAX + 1) && (y > -DBL_MAX + 1));
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
/// @brief 3D point or vector
class cxyz
{
public:
    double x, y, z;

    cxyz()
        : x(-DBL_MAX),
          y(-DBL_MAX),
          z(-DBL_MAX)
    {
    }
    cxyz(double X, double Y, double Z)
        : x(X), y(Y), z(Z)
    {
    }

    /// @brief vector from this point to other
    /// @param other
    /// @return
    cxyz vect(const cxyz &other) const
    {
        return cxyz(
            other.x - x,
            other.y - y,
            other.z - z);
    }

    static cxyz plane(
        const cxyz &p0,
        const cxyz &p1,
        const cxyz &p2)
    {
        cxyz p01 = p0.vect(p1);
        cxyz p02 = p0.vect(p2);
        return cxyz(
            p0.x + p01.x + p02.x,
            p0.y + p01.y + p02.y,
            p0.z + p01.z + p02.z);
    }

    cxyz cross(const cxyz &other)
    {
        return cxyz(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.z);
    }

    double dot(const cxyz &other)
    {
        return x * other.x +
               y * other.y +
               z * other.z;
    }

    /// @brief intersection point between line and triangle
    /// @param la
    /// @param lb
    /// @param plane
    /// @return intersection point
    /// https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection#Parametric_form

    static cxyz intersectLineTriangle(
        const cxyz &la, const cxyz &lb,
        const cxyz &p0, const cxyz &p1, const cxyz &p2)
    {
        cxyz crossall = p0.vect(p1).cross(p0.vect(p2));
        cxyz crossu = p0.vect(p2).cross(lb.vect(la));
        cxyz crossv = lb.vect(la).cross(p0.vect(p2));
        cxyz lap0(la.x - p0.x, la.y - p0.y, la.z - p0.x);
        double divisor = lb.vect(la).dot(crossall);
        double t = crossall.dot(lap0) / divisor;
        double u = crossu.dot(lap0) / divisor;
        double v = crossv.dot(lap0) / divisor;

        // check that line intersects triangle
        if (t >= 0 && t <= 1)
            if (u >= 0 && u <= 1)
                if (v >= 0 && v <= 1)
                    if (u + v <= 1)

                        return cxyz(
                            la.x + t * (lb.x - la.x),
                            la.y + t * (lb.y - la.y),
                            la.z + t * (lb.z - la.z));

        return cxyz();
    }
};