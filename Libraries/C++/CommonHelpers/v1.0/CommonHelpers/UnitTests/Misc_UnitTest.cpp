/////////////////////////////////////////////////////////////////////////
///
///  \file          Misc_UnitTest.cpp
///  \brief         Unit test for Misc.h
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-05-09 21:55:30
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
#include "../Misc.h"
#include <catch.hpp>

void Func1(bool arg) { ENSURE_ARGUMENT(arg); }
void Func2(bool arg) { ENSURE_ARGUMENT(arg, arg == false); }
void Func3(bool arg) { ENSURE_ARGUMENT_STR("Failed!", arg); }

TEST_CASE("Func1") {
    Func1(true);
    CHECK_THROWS_MATCHES(Func1(false), std::invalid_argument, Catch::Matchers::Exception::ExceptionMessageMatcher("arg"));
}

TEST_CASE("Func2") {
    CHECK_THROWS_MATCHES(Func2(true), std::invalid_argument, Catch::Matchers::Exception::ExceptionMessageMatcher("arg"));
    Func2(false);
}

TEST_CASE("Func3") {
    Func3(true);
    CHECK_THROWS_MATCHES(Func3(false), std::invalid_argument, Catch::Matchers::Exception::ExceptionMessageMatcher("Failed!"));
}
