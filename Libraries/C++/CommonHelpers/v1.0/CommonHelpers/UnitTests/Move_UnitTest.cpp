/////////////////////////////////////////////////////////////////////////
///
///  \file      Move_UnitTest.cpp
///  \brief     Unit test for Move.h
///
///  \author    David Brownell <db@DavidBrownell.com>
///  \date      2019-03-22 22:03:55
///
///  \note
///
///  \bug
///
/////////////////////////////////////////////////////////////////////////
///
///  \attention
///  Copyright David Brownell 2019
///  Distributed under the Boost Software License, Version 1.0. See
///  accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt.
///
/////////////////////////////////////////////////////////////////////////
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_CONSOLE_WIDTH 200
#include "../Move.h"
#include <catch.hpp>

struct EmptyObj {
    EmptyObj(void) = default;
    MOVE(EmptyObj);
};

struct SingleMemberObj {
    SingleMemberObj(int a_param) : a(a_param) {}
    MOVE(SingleMemberObj, MEMBERS(a));

    int const a;
};

struct SingleBaseObj : public SingleMemberObj {
    SingleBaseObj(int i_param) : SingleMemberObj(i_param) {}
    MOVE(SingleBaseObj, BASES(SingleMemberObj));
};

struct MultiMemberObj {
    MultiMemberObj(bool b_param, char c_param) : b(b_param), c(c_param) {}
    MOVE(MultiMemberObj, MEMBERS(b, c));

    bool const b;
    char const c;
};

struct MultiBaseObj : public SingleMemberObj, public MultiMemberObj {
    MultiBaseObj(int i_param, bool b_param, char c_param) : SingleMemberObj(i_param), MultiMemberObj(b_param, c_param) {}
    MOVE(MultiBaseObj, BASES(SingleMemberObj, MultiMemberObj));
};

struct SingleMemberSingleBaseObj : public SingleMemberObj {
    SingleMemberSingleBaseObj(int i, char c_param) : SingleMemberObj(i), c(c_param) {}
    // Typically, MEMBERS will appear before BASES, but they can be
    // applied in any order.
    MOVE(SingleMemberSingleBaseObj, BASES(SingleMemberObj), MEMBERS(c));

    char const c;
};

struct MultiMemberMultiBaseObj : public SingleMemberObj, MultiMemberObj {
    MultiMemberMultiBaseObj(int i_param, bool b_param, char c_param, double d_param, float f_param) :
            SingleMemberObj(i_param),
            MultiMemberObj(b_param, c_param),
            d(d_param),
            f(f_param) {}
    MOVE(MultiMemberMultiBaseObj, MEMBERS(d, f), BASES(SingleMemberObj, MultiMemberObj));

    double const d;
    float const f;
};

struct NoAssignObj {
    NoAssignObj(int a_param, int b_param) : a(a_param), b(b_param) {}
    MOVE(NoAssignObj, MEMBERS(a, b), FLAGS(MOVE_NO_ASSIGNMENT));

    int const a;
    int const b;
};

struct NoConstructObj {
    NoConstructObj(int a_param, int b_param) : a(a_param), b(b_param) {}
    MOVE(NoConstructObj, MEMBERS(a, b), FLAGS(MOVE_NO_CONSTRUCTOR));
    NoConstructObj(NoConstructObj &&) = delete;

    int const a;
    int const b;
};

struct NonMovableObj {
    NonMovableObj(int a_param) : a(a_param) {}
    NON_MOVABLE(NonMovableObj);

    int const a;
};

class EventObj {
private:
    unsigned int *_final_construct;
    unsigned int *_move_final_construct;
    unsigned int *_final_assign;
    unsigned int *_move_final_assign;

public:
    EventObj(
        unsigned int &final_construct,
        unsigned int &move_final_construct,
        unsigned int &final_assign,
        unsigned int &move_final_assign
    ) :
            _final_construct(&final_construct),
            _move_final_construct(&move_final_construct),
            _final_assign(&final_assign),
            _move_final_assign(&move_final_assign) {}
    MOVE(EventObj, _final_construct, _move_final_construct, _final_assign, _move_final_assign);

private:
    friend class CommonHelpers::TypeTraits::Access;

    void FinalConstruct(void) { ++(*_final_construct); }

    void MoveFinalConstruct(void) { ++(*_move_final_construct); }

    void FinalAssign(void) { ++(*_final_assign); }

    void MoveFinalAssign(void) { ++(*_move_final_assign); }
};

#if(defined COMPILER_ERRORS)
struct ObjWithReference {
    ObjWithReference(int &a_param) : a(a_param) {}
    MOVE(ObjWithReference, MEMBERS(a));

    int &a;
};
#endif

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
TEST_CASE("Ctor and Assign <Empty>") {
    EmptyObj o1;
    EmptyObj o2(std::move(o1));
    EmptyObj o3;

    o3 = std::move(o2);
}

TEST_CASE("Ctor and Assign <Single Member>") {
    SingleMemberObj o1(10);

    CHECK(o1.a == 10);

    SingleMemberObj o2(std::move(o1));

    CHECK(o2.a == 10);

    SingleMemberObj o3(0);

    o3 = std::move(o2);

    CHECK(o3.a == 10);
}

TEST_CASE("Ctor and Assign <Single Base>") {
    SingleBaseObj o1(10);

    CHECK(o1.a == 10);

    SingleBaseObj o2(std::move(o1));

    CHECK(o2.a == 10);

    SingleBaseObj o3(0);

    o3 = std::move(o2);

    CHECK(o3.a == 10);
}

TEST_CASE("Ctor and Assign <Multi Member>") {
    MultiMemberObj o1(true, 'q');

    CHECK(o1.b == true);
    CHECK(o1.c == 'q');

    MultiMemberObj o2(std::move(o1));

    CHECK(o2.b == true);
    CHECK(o2.c == 'q');

    MultiMemberObj o3(false, 'a');

    o3 = std::move(o2);

    CHECK(o3.b == true);
    CHECK(o3.c == 'q');
}

TEST_CASE("Ctor and Assign <Multi Base>") {
    MultiBaseObj o1(10, true, 'q');

    CHECK(o1.a == 10);
    CHECK(o1.b == true);
    CHECK(o1.c == 'q');

    MultiBaseObj o2(std::move(o1));

    CHECK(o2.a == 10);
    CHECK(o2.b == true);
    CHECK(o2.c == 'q');

    MultiBaseObj o3(0, false, 'a');

    o3 = std::move(o2);

    CHECK(o3.a == 10);
    CHECK(o3.b == true);
    CHECK(o3.c == 'q');
}

TEST_CASE("Ctor and Assign <SingleMember/SingleBase>") {
    SingleMemberSingleBaseObj o1(10, 'q');

    CHECK(o1.a == 10);
    CHECK(o1.c == 'q');

    SingleMemberSingleBaseObj o2(std::move(o1));

    CHECK(o2.a == 10);
    CHECK(o2.c == 'q');

    SingleMemberSingleBaseObj o3(0, 'a');

    o3 = std::move(o2);

    CHECK(o3.a == 10);
    CHECK(o3.c == 'q');
}

TEST_CASE("Ctor and Assign <Multi Member/Multi Base>") {
    MultiMemberMultiBaseObj o1(10, true, 'q', 1.0, 2.0);

    CHECK(o1.a == 10);
    CHECK(o1.b == true);
    CHECK(o1.c == 'q');
    CHECK(o1.d == 1.0);
    CHECK(o1.f == 2.0f);

    MultiMemberMultiBaseObj o2(std::move(o1));

    CHECK(o2.a == 10);
    CHECK(o2.b == true);
    CHECK(o2.c == 'q');
    CHECK(o2.d == 1.0);
    CHECK(o2.f == 2.0f);

    MultiMemberMultiBaseObj o3(0, false, 'z', 0.0, 1000.0);

    o3 = std::move(o2);

    CHECK(o3.a == 10);
    CHECK(o3.b == true);
    CHECK(o3.c == 'q');
    CHECK(o3.d == 1.0);
    CHECK(o3.f == 2.0f);
}

TEST_CASE("No Assign Obj") {
    NoAssignObj o1(10, 20);

    CHECK(o1.a == 10);
    CHECK(o1.b == 20);

    NoAssignObj o2(std::move(o1));

    CHECK(o2.a == 10);
    CHECK(o2.b == 20);

#if(defined COMPILE_ERRORS)
    NoAssignObj o3(0, 0);

    o3 = std::move(o2);  // This should fail
    REQUIRE(false);
#endif
}

TEST_CASE("No Construct Obj") {
    NoConstructObj o1(10, 20);

    CHECK(o1.a == 10);
    CHECK(o1.b == 20);

#if(defined COMPILE_ERRORS)
    NoConstructObj o2(std::move(o1));  // This shoud fail
    REQUIRE(false);
#endif

    NoConstructObj o3(0, 0);

    o3 = std::move(o1);

    CHECK(o3.a == 10);
    CHECK(o3.b == 20);
}

TEST_CASE("Non Movable") {
    NonMovableObj o1(10);

    CHECK(o1.a == 10);

#if(defined COMPILE_ERRORS)
    NonMovableObj o2(std::move(o1));  // This should fail
    REQUIRE(false);
#endif

#if(defined COMPILE_ERRORS)
    NonMovableObj o3(0);

    o3 = std::move(o1);  // This should fail
    REQUIRE(false);
#endif
}

TEST_CASE("Access Events") {
    unsigned int final_construct(0);
    unsigned int move_final_construct(0);
    unsigned int final_assign(0);
    unsigned int move_final_assign(0);

    auto const reset_func([&](void) {
        final_construct = 0;
        move_final_construct = 0;
        final_assign = 0;
        move_final_assign = 0;
    });
    auto const verify_func([&](unsigned int fc, unsigned int mfc, unsigned int fa, unsigned int mfa) {
        CHECK(fc == final_construct);
        CHECK(mfc == move_final_construct);
        CHECK(fa == final_assign);
        CHECK(mfa == move_final_assign);
    });

    verify_func(0, 0, 0, 0);

    EventObj o1(final_construct, move_final_construct, final_assign, move_final_assign);

    verify_func(0, 0, 0, 0);
    reset_func();

    EventObj o2(std::move(o1));

    verify_func(1, 1, 0, 0);
    reset_func();

    EventObj o3(final_construct, move_final_construct, final_assign, move_final_assign);

    verify_func(0, 0, 0, 0);
    reset_func();

    o3 = std::move(o2);

    verify_func(0, 0, 1, 1);
    reset_func();
}
