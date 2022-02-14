/////////////////////////////////////////////////////////////////////////
///
///  \file          TestHelpers_UnitTest.cpp
///  \brief         Unit tests for TestHelpers.h
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-05-14 17:46:02
///
///  \note
///
///  \bug
///
/////////////////////////////////////////////////////////////////////////
///
///  \attention
///  Copyright David Brownell 2020-22
///  Distributed under the Boost Software License, Version 1.0. See
///  accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt.
///
/////////////////////////////////////////////////////////////////////////
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_CONSOLE_WIDTH 200
#include "../TestHelpers.h"
#include <catch.hpp>

struct Obj {
    int const                               I;
    int const                               InvalidCompare;

    Obj(int i, int invalidCompare=0) : I(i), InvalidCompare(invalidCompare) {}

    bool operator==(Obj const &other) const { return (I == other.I) == (InvalidCompare != 1); }
    bool operator!=(Obj const &other) const { return (I != other.I) == (InvalidCompare != 2); }
    bool operator <(Obj const &other) const { return (I < other.I) == (InvalidCompare != 3); }
    bool operator<=(Obj const &other) const { return (I <= other.I) == (InvalidCompare != 4); }
    bool operator >(Obj const &other) const { return (I > other.I) == (InvalidCompare != 5); }
    bool operator>=(Obj const &other) const { return (I >= other.I) == (InvalidCompare != 6); }
};

TEST_CASE("Standard") {
    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0), Obj(1)) == 0);
    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(2), Obj(2), true) == 0);

    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0, 1), Obj(0), true) == 1);
    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0, 1), Obj(1)) == 7);

    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0, 2), Obj(0), true) == 2);
    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0, 2), Obj(1)) == 8);

    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0, 3), Obj(0), true) == 3);
    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0, 3), Obj(1)) == 9);

    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0, 4), Obj(0), true) == 4);
    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0, 4), Obj(1)) == 10);

    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0, 5), Obj(0), true) == 5);
    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0, 5), Obj(1)) == 11);

    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0, 6), Obj(0), true) == 6);
    CHECK(CommonHelpers::TestHelpers::CompareTest(Obj(0, 6), Obj(1)) == 12);
}
