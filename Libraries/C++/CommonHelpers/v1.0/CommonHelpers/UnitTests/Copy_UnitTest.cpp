/////////////////////////////////////////////////////////////////////////
///
///  \file      Copy_UnitTest.cpp
///  \brief     Unit test for Copy.h
///
///  \author    David Brownell <db@DavidBrownell.com>
///  \date      2019-03-21 21:12:43
///
///  \note
///
///  \bug
///
/////////////////////////////////////////////////////////////////////////
///
///  \attention
///  Copyright David Brownell 2019-20
///  Distributed under the Boost Software License, Version 1.0. See
///  accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt.
///
/////////////////////////////////////////////////////////////////////////
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_CONSOLE_WIDTH 200
#include "../Copy.h"
#include <catch.hpp>

struct EmptyObj {
    EmptyObj(void) = default;
    COPY(EmptyObj);
};

struct SingleMemberObj {
    SingleMemberObj(int i_param) : i(i_param) {}
    COPY(SingleMemberObj, MEMBERS(i));

    int const i;
};

struct SingleBaseObj : public SingleMemberObj {
    SingleBaseObj(int i_param) : SingleMemberObj(i_param) {}
    COPY(SingleBaseObj, BASES(SingleMemberObj));
};

struct MultiMemberObj {
    MultiMemberObj(bool b_param, char c_param) : b(b_param), c(c_param) {}
    COPY(MultiMemberObj, MEMBERS(b, c));

    bool const b;
    char const c;
};

struct MultiBaseObj : public SingleMemberObj, public MultiMemberObj {
    MultiBaseObj(int i_param, bool b_param, char c_param) : SingleMemberObj(i_param), MultiMemberObj(b_param, c_param) {}
    COPY(MultiBaseObj, BASES(SingleMemberObj, MultiMemberObj));
};

struct SingleMemberSingleBaseObj : public SingleMemberObj {
    SingleMemberSingleBaseObj(int i_param, char c_param) : SingleMemberObj(i_param), c(c_param) {}
    // Typically, MEMBERS will appear before BASES, but they can be
    // applied in any order.
    COPY(SingleMemberSingleBaseObj, BASES(SingleMemberObj), MEMBERS(c));

    char const c;
};

struct MultiMemberMultiBaseObj : public SingleMemberObj, MultiMemberObj {
    MultiMemberMultiBaseObj(int i_param, bool b_param, char c_param, double d_param, float f_param) :
            SingleMemberObj(i_param),
            MultiMemberObj(b_param, c_param),
            d(d_param),
            f(f_param) {}
    COPY(MultiMemberMultiBaseObj, MEMBERS(d, f), BASES(SingleMemberObj, MultiMemberObj));

    double const d;
    float const f;
};

struct NoAssignObj {
    NoAssignObj(int a_param, int b_param) : a(a_param), b(b_param) {}
    COPY(NoAssignObj, MEMBERS(a, b), FLAGS(COPY_NO_ASSIGNMENT));

    int const a;
    int const b;
};

struct NoConstructObj {
    NoConstructObj(int a_param, int b_param) : a(a_param), b(b_param) {}
    COPY(NoConstructObj, MEMBERS(a, b), FLAGS(COPY_NO_CONSTRUCTOR));
    NoConstructObj(NoConstructObj const &) = delete;

    int const a;
    int const b;
};

struct NonCopyableObj {
    NonCopyableObj(int a_param) : a(a_param) {}
    NON_COPYABLE(NonCopyableObj);

    int const a;
};

class EventObj {
private:
    unsigned int &_final_construct;
    unsigned int &_copy_final_construct;
    unsigned int &_final_assign;
    unsigned int &_copy_final_assign;

public:
    EventObj(
        unsigned int &final_construct,
        unsigned int &copy_final_construct,
        unsigned int &final_assign,
        unsigned int &copy_final_assign
    ) :
            _final_construct(final_construct),
            _copy_final_construct(copy_final_construct),
            _final_assign(final_assign),
            _copy_final_assign(copy_final_assign) {}
    COPY(EventObj, _final_construct, _copy_final_construct, _final_assign, _copy_final_assign);

private:
    friend class CommonHelpers::TypeTraits::Access;

    void FinalConstruct(void) { ++_final_construct; }

    void CopyFinalConstruct(void) { ++_copy_final_construct; }

    void FinalAssign(void) { ++_final_assign; }

    void CopyFinalAssign(void) { ++_copy_final_assign; }
};

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
TEST_CASE("Ctor and Assign <Empty>") {
    EmptyObj const o1;
    EmptyObj const o2(o1);
    EmptyObj o3;

    o3 = o2;
}

TEST_CASE("Ctor and Assign <Single Member>") {
    SingleMemberObj const o1(10);
    SingleMemberObj const o2(o1);
    SingleMemberObj o3(0);

    o3 = o2;

    CHECK(o1.i == 10);
    CHECK(o2.i == 10);
    CHECK(o3.i == 10);
}

TEST_CASE("Ctor and Assign <Single Base>") {
    SingleBaseObj const o1(10);
    SingleBaseObj const o2(o1);
    SingleBaseObj o3(0);

    o3 = o2;

    CHECK(o1.i == 10);
    CHECK(o2.i == 10);
    CHECK(o3.i == 10);
}

TEST_CASE("Ctor and Assign <Multi Member>") {
    MultiMemberObj const o1(true, 'q');
    MultiMemberObj const o2(o1);
    MultiMemberObj o3(false, 'z');

    o3 = o2;

    CHECK(o1.b == true);
    CHECK(o1.c == 'q');
    CHECK(o2.b == true);
    CHECK(o2.c == 'q');
    CHECK(o3.b == true);
    CHECK(o3.c == 'q');
}

TEST_CASE("Ctor and Assign <Multi Base>") {
    MultiBaseObj const o1(10, true, 'q');
    MultiBaseObj const o2(o1);
    MultiBaseObj o3(0, false, 'z');

    o3 = o2;

    CHECK(o1.i == 10);
    CHECK(o1.b == true);
    CHECK(o1.c == 'q');
    CHECK(o2.i == 10);
    CHECK(o2.b == true);
    CHECK(o2.c == 'q');
    CHECK(o3.i == 10);
    CHECK(o3.b == true);
    CHECK(o3.c == 'q');
}

TEST_CASE("Ctor and Assign <Single Member/Single Base>") {
    SingleMemberSingleBaseObj const o1(10, 'q');
    SingleMemberSingleBaseObj const o2(o1);
    SingleMemberSingleBaseObj o3(0, 'z');

    o3 = o2;

    CHECK(o1.i == 10);
    CHECK(o1.c == 'q');
    CHECK(o2.i == 10);
    CHECK(o2.c == 'q');
    CHECK(o3.i == 10);
    CHECK(o3.c == 'q');
}

TEST_CASE("Ctor and Assign <Multi Member/Multi Base>") {
    MultiMemberMultiBaseObj const o1(10, true, 'q', 1.0, 2.0);
    MultiMemberMultiBaseObj const o2(o1);
    MultiMemberMultiBaseObj o3(0, false, 'z', 0.0, 1000.0);

    o3 = o2;

    CHECK(o1.i == 10);
    CHECK(o1.b == true);
    CHECK(o1.c == 'q');
    CHECK(o1.d == 1.0);
    CHECK(o1.f == 2.0f);

    CHECK(o2.i == 10);
    CHECK(o2.b == true);
    CHECK(o2.c == 'q');
    CHECK(o2.d == 1.0);
    CHECK(o2.f == 2.0f);

    CHECK(o3.i == 10);
    CHECK(o3.b == true);
    CHECK(o3.c == 'q');
    CHECK(o3.d == 1.0);
    CHECK(o3.f == 2.0f);
}

TEST_CASE("No Assign Obj") {
    NoAssignObj const o1(10, 20);
    NoAssignObj const o2(o1);

#if(defined COMPILE_ERRORS)
    NoAssignObj o3(0, 0);

    o3 = o2;  // This should fail
    REQUIRE(false);
#endif

    CHECK(o1.a == 10);
    CHECK(o1.b == 20);
    CHECK(o2.a == 10);
    CHECK(o2.b == 20);
}

TEST_CASE("No Construct Obj") {
    NoConstructObj const o1(10, 20);

#if(defined COMPILE_ERRORS)
    NoConstructObj const o2(o1);  // This should fail
    REQUIRE(false);
#endif

    NoConstructObj o3(0, 0);

    o3 = o1;

    CHECK(o1.a == 10);
    CHECK(o1.b == 20);
    CHECK(o3.a == 10);
    CHECK(o3.b == 20);
}

TEST_CASE("Non Copyable") {
    NonCopyableObj const o1(10);

#if(defined COMPILE_ERRORS)
    NonCopyableObj const o2(o1);  // This should fail
    REQUIRE(false);
#endif

#if(defined COMPILE_ERRORS)
    NonCopyableObj o3(0);

    o3 = o1;  // This should fail
    REQUIRE(false);
#endif

    CHECK(o1.a == 10);
}

TEST_CASE("Access Events") {
    unsigned int final_construct(0);
    unsigned int copy_final_construct(0);
    unsigned int final_assign(0);
    unsigned int copy_final_assign(0);

    auto const reset_func([&](void) {
        final_construct = 0;
        copy_final_construct = 0;
        final_assign = 0;
        copy_final_assign = 0;
    });
    auto const verify_func([&](unsigned int fc, unsigned int cfc, unsigned int fa, unsigned int cfa) {
        CHECK(fc == final_construct);
        CHECK(cfc == copy_final_construct);
        CHECK(fa == final_assign);
        CHECK(cfa == copy_final_assign);
    });

    verify_func(0, 0, 0, 0);

    EventObj const o1(final_construct, copy_final_construct, final_assign, copy_final_assign);

    verify_func(0, 0, 0, 0);
    reset_func();

    EventObj const o2(o1);

    verify_func(1, 1, 0, 0);
    reset_func();

    EventObj o3(final_construct, copy_final_construct, final_assign, copy_final_assign);

    verify_func(0, 0, 0, 0);
    reset_func();

    o3 = o2;

    verify_func(0, 0, 1, 1);
    reset_func();
}
