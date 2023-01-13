/// @brief 2D point or vector
#pragma once

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
    bool isInside( const std::vector<cxy>& polygon ) const;

    static cxy enclosingWidthHeight( const std::vector<cxy>& polygon );
};