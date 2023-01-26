#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include "cutest.h"
#include "cxy.h"

TEST(intersection)
{
    cxy a1(-100, 0);
    cxy a2(100, 0);
    cxy b1(-15, -5);
    cxy b2(5, 15);
    cxy p;
    CHECK(cxy::isIntersection(p, a1, a2, b1, b2));
    CHECK_EQUAL(-10, p.x);
    CHECK_EQUAL(0, p.y);
}

main()
{
    return raven::set::UnitTest::RunAllTests();
}
