
#pragma once

/// @brief 2D point or vector
class cxy
{

public:
    double x;
    double y;

    cxy()
    {
    }
    cxy(int X, int Y)
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

    /// angle between line segments, radians

    /// @brief angle between line segments, radians
    /// @param a,b line segment
    /// @param c,d line segment 
    /// @return angle in radians

    static double angle(
        const cxy &a, const cxy &b,
        const cxy &c, const cxy &d);


    bool operator==( const cxy& other ) const
    {
        return x == other.x && y == other.y;
    }
};