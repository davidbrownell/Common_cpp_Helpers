/////////////////////////////////////////////////////////////////////////
///
///  \file          Finally_UnitTest.cpp
///  \brief         Unit test for Finally.h
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-05-30 16:22:15
///
///  \note
///
///  \bug
///
/////////////////////////////////////////////////////////////////////////
///
///  \attention
///  Copyright David Brownell 2020
///  Distributed under the Boost Software License, Version 1.0. See
///  accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt.
///
/////////////////////////////////////////////////////////////////////////
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_CONSOLE_WIDTH 200
#include "../Finally.h"
#include <catch.hpp>

TEST_CASE("Single Arg") {
    int                                     value(0);

    {
        CHECK(value == 0);
        FINALLY([&value](void) { ++value; });
        CHECK(value == 0);
    }

    CHECK(value == 1);
}

TEST_CASE("Multiple Args") {
    int                                     value1(0);
    int                                     value2(0);

    CHECK(value1 == 0);
    CHECK(value2 == 0);

    {
        FINALLY([&value1, &value2](void) { value1 += 1; value2 += 100; });

        CHECK(value1 == 0);
        CHECK(value2 == 0);
    }

    CHECK(value1 == 1);
    CHECK(value2 == 100);
}