/////////////////////////////////////////////////////////////////////////
///
///  \file          Compare_UnitTest.cpp
///  \brief         Unit test for Compare.h
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2019-03-27 10:10:01
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
#include "../Compare.h"
#include <catch.hpp>

struct EmptyObj {
    EmptyObj(void) = default;
    COMPARE(EmptyObj);
};

TEST_CASE("EmptyObj") {
    CHECK(EmptyObj() == EmptyObj());
    CHECK((EmptyObj() != EmptyObj()) == false);
}

struct SingleMemberObj {
    int const a;

    SingleMemberObj(int a_param) : a(a_param) {}
    COMPARE(SingleMemberObj, a);
};

TEST_CASE("SingleMemberObj") {
    CHECK(SingleMemberObj(10) == SingleMemberObj(10));
    CHECK(SingleMemberObj(10) != SingleMemberObj(20));
    CHECK(SingleMemberObj(10) < SingleMemberObj(20));
    CHECK(SingleMemberObj(10) <= SingleMemberObj(20));
    CHECK(SingleMemberObj(10) <= SingleMemberObj(10));
    CHECK(SingleMemberObj(20) > SingleMemberObj(10));
    CHECK(SingleMemberObj(20) >= SingleMemberObj(20));
}

struct SingleBaseObj : public SingleMemberObj {
    SingleBaseObj(int a_param) : SingleMemberObj(a_param) {}
    COMPARE(SingleBaseObj, BASES(SingleMemberObj));
};

TEST_CASE("SingleBaseObj") {
    CHECK(SingleBaseObj(10) == SingleBaseObj(10));
    CHECK(SingleBaseObj(10) != SingleBaseObj(20));
    CHECK(SingleBaseObj(10) < SingleBaseObj(20));
    CHECK(SingleBaseObj(10) <= SingleBaseObj(20));
    CHECK(SingleBaseObj(10) <= SingleBaseObj(10));
    CHECK(SingleBaseObj(20) > SingleBaseObj(10));
    CHECK(SingleBaseObj(20) >= SingleBaseObj(20));
}

struct SingleMemberSingleBaseObj : public SingleMemberObj {
    bool const b;

    SingleMemberSingleBaseObj(int a_param, bool b_param) : SingleMemberObj(a_param), b(b_param) {}
    COMPARE(SingleMemberSingleBaseObj, BASES(SingleMemberObj), MEMBERS(b));
};

TEST_CASE("SingleMemberSingleBaseObj") {
    CHECK(SingleMemberSingleBaseObj(10, true) == SingleMemberSingleBaseObj(10, true));
    CHECK(SingleMemberSingleBaseObj(10, true) != SingleMemberSingleBaseObj(20, true));
    CHECK(SingleMemberSingleBaseObj(10, true) != SingleMemberSingleBaseObj(10, false));
    CHECK(SingleMemberSingleBaseObj(10, true) < SingleMemberSingleBaseObj(20, true));
    CHECK(SingleMemberSingleBaseObj(10, false) < SingleMemberSingleBaseObj(10, true));
    CHECK(SingleMemberSingleBaseObj(20, true) > SingleMemberSingleBaseObj(10, true));
    CHECK(SingleMemberSingleBaseObj(10, true) > SingleMemberSingleBaseObj(10, false));
}

struct MultiMemberObj {
    bool const b;
    char const c;

    MultiMemberObj(bool b_param, char c_param) : b(b_param), c(c_param) {}
    COMPARE(MultiMemberObj, b, c);
};

TEST_CASE("MultiMemberObj") {
    CHECK(MultiMemberObj(true, 'a') == MultiMemberObj(true, 'a'));
    CHECK(MultiMemberObj(true, 'a') != MultiMemberObj(false, 'a'));
    CHECK(MultiMemberObj(true, 'a') != MultiMemberObj(true, 'z'));
    CHECK(MultiMemberObj(false, 'a') < MultiMemberObj(true, 'a'));
    CHECK(MultiMemberObj(true, 'a') < MultiMemberObj(true, 'z'));
    CHECK(MultiMemberObj(true, 'a') > MultiMemberObj(false, 'a'));
    CHECK(MultiMemberObj(true, 'z') > MultiMemberObj(true, 'a'));
}

struct MultiBaseObj : public SingleMemberObj, public MultiMemberObj {
    MultiBaseObj(int a_param, bool b_param, char c_param) : SingleMemberObj(a_param), MultiMemberObj(b_param, c_param) {}
    COMPARE(MultiBaseObj, BASES(SingleMemberObj, MultiMemberObj));
};

TEST_CASE("MultiBaseObj") {
    CHECK(MultiBaseObj(10, true, 'c') == MultiBaseObj(10, true, 'c'));
    CHECK(MultiBaseObj(20, true, 'c') != MultiBaseObj(10, true, 'c'));
    CHECK(MultiBaseObj(10, false, 'c') != MultiBaseObj(10, true, 'c'));
    CHECK(MultiBaseObj(10, true, 'm') != MultiBaseObj(10, true, 'c'));
    CHECK(MultiBaseObj(-10, true, 'c') < MultiBaseObj(10, true, 'c'));
    CHECK(MultiBaseObj(10, false, 'c') < MultiBaseObj(10, true, 'c'));
    CHECK(MultiBaseObj(10, true, 'a') < MultiBaseObj(10, true, 'c'));
    CHECK(MultiBaseObj(100, true, 'c') > MultiBaseObj(10, true, 'c'));
    CHECK(MultiBaseObj(10, true, 'c') > MultiBaseObj(10, false, 'c'));
    CHECK(MultiBaseObj(10, true, 'z') > MultiBaseObj(10, true, 'c'));
}

struct MultiMemberMultiBaseObj : public SingleMemberObj, public MultiMemberObj {
    double const d;
    float const f;

    MultiMemberMultiBaseObj(int a_param, bool b_param, char c_param, double d_param, float f_param) : SingleMemberObj(a_param), MultiMemberObj(b_param, c_param), d(d_param), f(f_param) {}
    COMPARE(MultiMemberMultiBaseObj, MEMBERS(d, f), BASES(SingleMemberObj, MultiMemberObj));
};

TEST_CASE("MultiMemberMultiBaseObj") {
    CHECK(MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0) == MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    
    CHECK(MultiMemberMultiBaseObj(100, true, 'c', 1.0, 2.0) != MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    CHECK(MultiMemberMultiBaseObj(10, false, 'c', 1.0, 2.0) != MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    CHECK(MultiMemberMultiBaseObj(10, true, 'm', 1.0, 2.0) != MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    CHECK(MultiMemberMultiBaseObj(10, true, 'c', 10.0, 2.0) != MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    CHECK(MultiMemberMultiBaseObj(10, true, 'c', 1.0, 20.0) != MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    
    CHECK(MultiMemberMultiBaseObj(-10, true, 'c', 1.0, 2.0) < MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    CHECK(MultiMemberMultiBaseObj(10, false, 'c', 1.0, 2.0) < MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    CHECK(MultiMemberMultiBaseObj(10, true, 'a', 1.0, 2.0) < MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    CHECK(MultiMemberMultiBaseObj(10, true, 'c', -1.0, 2.0) < MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    CHECK(MultiMemberMultiBaseObj(10, true, 'c', 1.0, -2.0) < MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));

    CHECK(MultiMemberMultiBaseObj(100, true, 'c', 1.0, 2.0) > MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    CHECK(MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0) > MultiMemberMultiBaseObj(10, false, 'c', 1.0, 2.0));
    CHECK(MultiMemberMultiBaseObj(10, true, 'z', 1.0, 2.0) > MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    CHECK(MultiMemberMultiBaseObj(10, true, 'c', 10.0, 2.0) > MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
    CHECK(MultiMemberMultiBaseObj(10, true, 'c', 1.0, 20.0) > MultiMemberMultiBaseObj(10, true, 'c', 1.0, 2.0));
}

struct NoOperatorsObj {
    int const a;
    bool const b;

    NoOperatorsObj(int a_param, bool b_param) : a(a_param), b(b_param) {}
    COMPARE(NoOperatorsObj, MEMBERS(a, b), FLAGS(COMPARE_NO_OPERATORS));
};

TEST_CASE("NoOperatorsObj") {
    CHECK(NoOperatorsObj::Compare(NoOperatorsObj(10, true), NoOperatorsObj(10, true)) == 0);
    CHECK(NoOperatorsObj::Compare(NoOperatorsObj(-10, true), NoOperatorsObj(10, true)) < 0);
    CHECK(NoOperatorsObj::Compare(NoOperatorsObj(10, false), NoOperatorsObj(10, true)) < 0);
    CHECK(NoOperatorsObj::Compare(NoOperatorsObj(100, true), NoOperatorsObj(10, true)) > 0);
    CHECK(NoOperatorsObj::Compare(NoOperatorsObj(10, true), NoOperatorsObj(10, false)) > 0);

#if (defined COMPILE_ERRORS)
    CHECK(NoOperatorsObj(10, true) == NoOperatorsObj(10, true));
#endif
}

struct StandardBasesBeforeMembersObj : public SingleMemberObj {
    bool const b;

    StandardBasesBeforeMembersObj(int a_param, bool b_param) : SingleMemberObj(a_param), b(b_param) {}
    COMPARE(StandardBasesBeforeMembersObj, MEMBERS(b), BASES(SingleMemberObj));
};

struct SpecialBasesBeforeMembersObj : public SingleMemberObj {
    bool const b;

    SpecialBasesBeforeMembersObj(int a_param, bool b_param) : SingleMemberObj(a_param), b(b_param) {}
    COMPARE(SpecialBasesBeforeMembersObj, MEMBERS(b), BASES(SingleMemberObj), FLAGS(COMPARE_BASES_BEFORE_MEMBERS));
};

TEST_CASE("BasesBeforeMembers") {
    CHECK(StandardBasesBeforeMembersObj(10, true) == StandardBasesBeforeMembersObj(10, true));
    CHECK(StandardBasesBeforeMembersObj(100, false) < StandardBasesBeforeMembersObj(10, true));

    CHECK(SpecialBasesBeforeMembersObj(10, true) == SpecialBasesBeforeMembersObj(10, true));
    CHECK(SpecialBasesBeforeMembersObj(100, false) > SpecialBasesBeforeMembersObj(10, true));
}

TEST_CASE("Tuples") {
    CHECK(CommonHelpers::Compare(std::make_tuple(1, 2, 3), std::make_tuple(1, 2, 3)) == 0);
    CHECK(CommonHelpers::Compare(std::make_tuple(10, 2, 3), std::make_tuple(1, 2, 3)) > 0);
    CHECK(CommonHelpers::Compare(std::make_tuple(-1, 2, 3), std::make_tuple(1, 2, 3)) < 0);
    CHECK(CommonHelpers::Compare(std::make_tuple(1, 20, 3), std::make_tuple(1, 2, 3)) > 0);
    CHECK(CommonHelpers::Compare(std::make_tuple(1, -2, 3), std::make_tuple(1, 2, 3)) < 0);
    CHECK(CommonHelpers::Compare(std::make_tuple(1, 2, 30), std::make_tuple(1, 2, 3)) > 0);
    CHECK(CommonHelpers::Compare(std::make_tuple(1, 2, -3), std::make_tuple(1, 2, 3)) < 0);
}

TEST_CASE("std::unique_ptr") {
    CHECK(CommonHelpers::Compare(std::unique_ptr<int>(), std::unique_ptr<int>()) == 0);
    CHECK(CommonHelpers::Compare(std::make_unique<int>(1), std::unique_ptr<int>()) < 0);
    CHECK(CommonHelpers::Compare(std::unique_ptr<int>(), std::make_unique<int>(1)) > 0);
    CHECK(CommonHelpers::Compare(std::make_unique<int>(1), std::make_unique<int>(1)) == 0);
    CHECK(CommonHelpers::Compare(std::make_unique<int>(10), std::make_unique<int>(1)) > 0);
    CHECK(CommonHelpers::Compare(std::make_unique<int>(1), std::make_unique<int>(10)) < 0);
}

TEST_CASE("std::shared_ptr") {
    CHECK(CommonHelpers::Compare(std::shared_ptr<int>(), std::shared_ptr<int>()) == 0);
    CHECK(CommonHelpers::Compare(std::make_shared<int>(1), std::shared_ptr<int>()) < 0);
    CHECK(CommonHelpers::Compare(std::shared_ptr<int>(), std::make_shared<int>(1)) > 0);
    CHECK(CommonHelpers::Compare(std::make_shared<int>(1), std::make_shared<int>(1)) == 0);
    CHECK(CommonHelpers::Compare(std::make_shared<int>(10), std::make_shared<int>(1)) > 0);
    CHECK(CommonHelpers::Compare(std::make_shared<int>(1), std::make_shared<int>(10)) < 0);
}

TEST_CASE("std::vector<int>") {
    CHECK(CommonHelpers::Compare(std::vector<int>{1, 2, 3}, std::vector<int>{1, 2, 3}) == 0);
    CHECK(CommonHelpers::Compare(std::vector<int>{10, 2, 3}, std::vector<int>{1, 2, 3}) > 0);
    CHECK(CommonHelpers::Compare(std::vector<int>{-1, 2, 3}, std::vector<int>{1, 2, 3}) < 0);
    CHECK(CommonHelpers::Compare(std::vector<int>{1, 20, 3}, std::vector<int>{1, 2, 3}) > 0);
    CHECK(CommonHelpers::Compare(std::vector<int>{1, -2, 3}, std::vector<int>{1, 2, 3}) < 0);
    CHECK(CommonHelpers::Compare(std::vector<int>{1, 2, 30}, std::vector<int>{1, 2, 3}) > 0);
    CHECK(CommonHelpers::Compare(std::vector<int>{1, 2, -3}, std::vector<int>{1, 2, 3}) < 0);
    CHECK(CommonHelpers::Compare(std::vector<int>{1, 2}, std::vector<int>{1, 2, 3}) < 0);
    CHECK(CommonHelpers::Compare(std::vector<int>{1, 2, 3}, std::vector<int>{1, 2}) > 0);
    CHECK(CommonHelpers::Compare(std::vector<int>{10}, std::vector<int>{1, 2, 3}) > 0);
    CHECK(CommonHelpers::Compare(std::vector<int>{-1, 2, 3, 4, 5, 6}, std::vector<int>{1, 2, 3}) < 0);
}

TEST_CASE("std::vector<SingleMemberObj>") {
    CHECK(CommonHelpers::Compare(std::vector<SingleMemberObj>{ {1}, {2}, {3} }, std::vector<SingleMemberObj>{ {1}, {2}, {3} }) == 0);
    CHECK(CommonHelpers::Compare(std::vector<SingleMemberObj>{ {10}, {2}, {3} }, std::vector<SingleMemberObj>{ {1}, {2}, {3} }) > 0);
    CHECK(CommonHelpers::Compare(std::vector<SingleMemberObj>{ {-1}, {2}, {3} }, std::vector<SingleMemberObj>{ {1}, {2}, {3} }) < 0);
    CHECK(CommonHelpers::Compare(std::vector<SingleMemberObj>{ {1}, {20}, {3} }, std::vector<SingleMemberObj>{ {1}, {2}, {3} }) > 0);
    CHECK(CommonHelpers::Compare(std::vector<SingleMemberObj>{ {1}, {-2}, {3} }, std::vector<SingleMemberObj>{ {1}, {2}, {3} }) < 0);
    CHECK(CommonHelpers::Compare(std::vector<SingleMemberObj>{ {1}, {2}, {30} }, std::vector<SingleMemberObj>{ {1}, {2}, {3} }) > 0);
    CHECK(CommonHelpers::Compare(std::vector<SingleMemberObj>{ {1}, {2}, {-3} }, std::vector<SingleMemberObj>{ {1}, {2}, {3} }) < 0);
    CHECK(CommonHelpers::Compare(std::vector<SingleMemberObj>{ {1}, {2} }, std::vector<SingleMemberObj>{ {1}, {2}, {3} }) < 0);
    CHECK(CommonHelpers::Compare(std::vector<SingleMemberObj>{ {1}, {2}, {3} }, std::vector<SingleMemberObj>{ {1}, {2} }) > 0);
    CHECK(CommonHelpers::Compare(std::vector<SingleMemberObj>{ {-1}, {2}, {3} }, std::vector<SingleMemberObj>{ {1}, {2} }) < 0);
    CHECK(CommonHelpers::Compare(std::vector<SingleMemberObj>{ {10} }, std::vector<SingleMemberObj>{ {1}, {2}, {3} }) > 0);
}

enum class Enum {
    one,
    two,
    Three
};

TEST_CASE("Enums") {
    CHECK(CommonHelpers::Compare(Enum::one, Enum::one) == 0);
    CHECK(CommonHelpers::Compare(Enum::one, Enum::two) != 0);
    CHECK(CommonHelpers::Compare(Enum::one, Enum::two) < 0);
    CHECK(CommonHelpers::Compare(Enum::two, Enum::one) > 0);
}

TEST_CASE("Raw MultiByte String") {
    CHECK(CommonHelpers::Compare("foo", "foo") == 0);
    CHECK(CommonHelpers::Compare("Foo", "foo") != 0);
    CHECK(CommonHelpers::Compare("Foo", "foo") < 0);
    CHECK(CommonHelpers::Compare("Foo__________", "foo") < 0);
    CHECK(CommonHelpers::Compare("foo", "Foo") > 0);
    CHECK(CommonHelpers::Compare("foo", "Foo___________") > 0);
    CHECK(CommonHelpers::Compare("foo", nullptr) < 0);
    CHECK(CommonHelpers::Compare(nullptr, "foo") > 0);
    CHECK(CommonHelpers::Compare(nullptr, nullptr) == 0);
}

TEST_CASE("Raw Wide String") {
    CHECK(CommonHelpers::Compare(L"foo", L"foo") == 0);
    CHECK(CommonHelpers::Compare(L"Foo", L"foo") != 0);
    CHECK(CommonHelpers::Compare(L"Foo", L"foo") < 0);
    CHECK(CommonHelpers::Compare(L"Foo_____", L"foo") < 0);
    CHECK(CommonHelpers::Compare(L"foo", L"Foo") > 0);
    CHECK(CommonHelpers::Compare(L"foo", L"Foo____") > 0);
    CHECK(CommonHelpers::Compare(L"foo", nullptr) < 0);
    CHECK(CommonHelpers::Compare(nullptr, L"foo") > 0);
    CHECK(CommonHelpers::Compare(nullptr, nullptr) == 0);
}

TEST_CASE("Raw ptr") {
    int const a(10);
    int const b(20);
    int const * const null(nullptr);

    CHECK(CommonHelpers::Compare(&a, &a) == 0);
    CHECK(CommonHelpers::Compare(&a, &b) != 0);
    CHECK(CommonHelpers::Compare(&a, &b) < 0);
    CHECK(CommonHelpers::Compare(&b, &a) > 0);
    CHECK(CommonHelpers::Compare(&a, null) < 0);
    CHECK(CommonHelpers::Compare(null, &a) > 0);
    CHECK(CommonHelpers::Compare(null, null) == 0);
}

struct CompareMethodObj {
    int const a;

    CompareMethodObj(int a_param) : a(a_param) {}

    int Compare(CompareMethodObj const &other) const {
        return a - other.a;
    }
};

struct OperatorMethodObj {
    int const a;

    OperatorMethodObj(int a_param) : a(a_param) {}

    bool operator <(OperatorMethodObj const &other) const {
        return a < other.a;
    }
};

struct GlobalMethodObj {
    int const a;

    GlobalMethodObj(int a_param) : a(a_param) {}
};

#if (defined __clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wmissing-prototypes"
#endif

bool operator <(GlobalMethodObj const &a, GlobalMethodObj const &b) {
    return a.a < b.a;
}

#if (defined __clang__)
#   pragma clang diagnostic pop
#endif

TEST_CASE("Different Compare Techniques") {
    CHECK(CommonHelpers::Compare(CompareMethodObj(10), CompareMethodObj(10)) == 0);
    CHECK(CommonHelpers::Compare(CompareMethodObj(-10), CompareMethodObj(10)) != 0);
    CHECK(CommonHelpers::Compare(CompareMethodObj(100), CompareMethodObj(10)) > 0);
    CHECK(CommonHelpers::Compare(CompareMethodObj(-10), CompareMethodObj(10)) < 0);

    CHECK(CommonHelpers::Compare(OperatorMethodObj(10), OperatorMethodObj(10)) == 0);
    CHECK(CommonHelpers::Compare(OperatorMethodObj(-10), OperatorMethodObj(10)) != 0);
    CHECK(CommonHelpers::Compare(OperatorMethodObj(100), OperatorMethodObj(10)) > 0);
    CHECK(CommonHelpers::Compare(OperatorMethodObj(-10), OperatorMethodObj(10)) < 0);

    CHECK(CommonHelpers::Compare(GlobalMethodObj(10), GlobalMethodObj(10)) == 0);
    CHECK(CommonHelpers::Compare(GlobalMethodObj(-10), GlobalMethodObj(10)) != 0);
    CHECK(CommonHelpers::Compare(GlobalMethodObj(100), GlobalMethodObj(10)) > 0);
    CHECK(CommonHelpers::Compare(GlobalMethodObj(-10), GlobalMethodObj(10)) < 0);
}
