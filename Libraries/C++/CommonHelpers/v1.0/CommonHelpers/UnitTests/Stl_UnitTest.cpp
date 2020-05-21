/////////////////////////////////////////////////////////////////////////
///
///  \file          Stl_UnitTest.cpp
///  \brief         Unit test for Stl.h
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-05-21 14:35:41
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
#include "../Stl.h"
#include <catch.hpp>

#include "../Compare.h"
#include "../Constructor.h"
#include "../Copy.h"
#include "../Move.h"

namespace NS                                = CommonHelpers::Stl;

class Object {
public:
    int const                               I;

#define ARGS                                MEMBERS(I)

    NON_COPYABLE(Object);
    CONSTRUCTOR(Object, ARGS);
    MOVE(Object, ARGS);
    COMPARE(Object, ARGS);

#undef ARGS
};

TEST_CASE("CreateVector") {
    std::vector<Object> const               v0(NS::CreateVector<Object>());

    CHECK(v0.empty());

    std::vector<Object> const               v1(NS::CreateVector<Object>(Object(1)));

    REQUIRE(v1.size() == 1);
    CHECK(v1[0].I == 1);

    std::vector<Object> const               v2(NS::CreateVector<Object>(Object(10), Object(20)));

    REQUIRE(v2.size() == 2);
    CHECK(v2[0].I == 10);
    CHECK(v2[1].I == 20);
}
