/////////////////////////////////////////////////////////////////////////
///
///  \file          Constructor_UnitTest.cpp
///  \brief         Unit test for Constructor.h
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2019-03-27 16:26:07
///
///  \note
///
///  \bug
///
/////////////////////////////////////////////////////////////////////////
///
///  \attention
///  Copyright David Brownell 2019-22
///  Distributed under the Boost Software License, Version 1.0. See
///  accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt.
///
/////////////////////////////////////////////////////////////////////////
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_CONSOLE_WIDTH 200
#include "../Constructor.h"
#include "../Copy.h"
#include "../Move.h"
#include <catch.hpp>

struct EmptyObj {
    CONSTRUCTOR(EmptyObj);
};

TEST_CASE("EmptyObj") {
    EmptyObj();
    CHECK(true);
}

struct SingleMemberObj {
    int const a;
    CONSTRUCTOR(SingleMemberObj, a);
    COPY(SingleMemberObj, a);
    MOVE(SingleMemberObj, a);
};

TEST_CASE("SingleMemberObj") {
    CHECK(SingleMemberObj(10).a == 10);

    // The constructor should not eat copies
    SingleMemberObj const                   o1(10);
    SingleMemberObj const                   o2(o1);

    CHECK(o1.a == 10);
    CHECK(o2.a == 10);

    // The constructor should not eat moves
    SingleMemberObj                         o3(10);
    SingleMemberObj                         o4(std::move(o3));

    CHECK(o4.a == 10);
}

struct SingleBaseObj : public SingleMemberObj {
    CONSTRUCTOR(SingleBaseObj, BASES(SingleMemberObj));
    COPY(SingleBaseObj, BASES(SingleMemberObj));
    MOVE(SingleBaseObj, BASES(SingleMemberObj));
};

TEST_CASE("SingleBaseObj") {
    CHECK(SingleBaseObj(10).a == 10);

    // The constructor should not eat copies
    SingleBaseObj const                     o1(10);
    SingleBaseObj const                     o2(o1);

    CHECK(o1.a == 10);
    CHECK(o2.a == 10);

    // The constructor should not eat moves
    SingleBaseObj                           o3(10);
    SingleBaseObj                           o4(std::move(o3));

    CHECK(o4.a == 10);  
}

struct SingleMemberSingleBaseObj : public SingleMemberObj {
    bool const b;
    CONSTRUCTOR(SingleMemberSingleBaseObj, MEMBERS(b), BASES(SingleMemberObj));
};

TEST_CASE("SingleMemberSingleBaseObj") {
    SingleMemberSingleBaseObj const         o1(true, 10);

    CHECK(o1.a == 10);
    CHECK(o1.b == true);
}

struct MultiMemberObj {
    bool const b;
    char const c;

    CONSTRUCTOR(MultiMemberObj, b, c);
};

TEST_CASE("MultiMemberObj") {
    MultiMemberObj const                    o1(true, 'c');

    CHECK(o1.b == true);
    CHECK(o1.c == 'c');
}

struct MultiMemberMultiBaseObj : public MultiMemberObj, SingleMemberObj {
    double const d;
    float const f;

    CONSTRUCTOR(MultiMemberMultiBaseObj, MEMBERS(d, f), BASES(MultiMemberObj, SingleMemberObj), FLAGS(CONSTRUCTOR_BASE_ARGS_0(2)));
};

TEST_CASE("MultiMemberMultiBaseObj") {
    MultiMemberMultiBaseObj const           o1(1.0, 2.0f, true, 'c', 10);

    CHECK(o1.a == 10);
    CHECK(o1.b == true);
    CHECK(o1.c == 'c');
    CHECK(o1.d == 1.0);
    CHECK(o1.f == 2.0f);
}

struct BasesBeforeMembersObj : public MultiMemberObj, SingleMemberObj {
    double const d;
    float const f;

    CONSTRUCTOR(BasesBeforeMembersObj, MEMBERS(d, f), BASES(MultiMemberObj, SingleMemberObj), FLAGS(CONSTRUCTOR_BASE_ARGS_0(2), CONSTRUCTOR_BASES_BEFORE_MEMBERS));
};

TEST_CASE("BasesBeforeMembersObj") {
    BasesBeforeMembersObj const             o1(true, 'c', 10, 1.0, 2.0f);

    CHECK(o1.a == 10);
    CHECK(o1.b == true);
    CHECK(o1.c == 'c');
    CHECK(o1.d == 1.0);
    CHECK(o1.f == 2.0f);
}

struct EmptyBaseObj : public EmptyObj {
    CONSTRUCTOR(EmptyBaseObj, BASES(EmptyObj), FLAGS(CONSTRUCTOR_BASE_ARGS_0(0)));
};

TEST_CASE("EmptyBaseObj") {
    EmptyBaseObj();
    CHECK(true);
}
