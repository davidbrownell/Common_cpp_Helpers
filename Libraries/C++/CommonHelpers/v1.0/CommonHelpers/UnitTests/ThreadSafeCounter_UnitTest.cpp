/////////////////////////////////////////////////////////////////////////
///
///  \file          ThreadSafeCounter_UnitTest.cpp
///  \brief         Unit test for ThreadSafeCounter.h
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-06-05 11:29:32
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
#include "../ThreadSafeCounter.h"
#include <catch.hpp>

#include <thread>

TEST_CASE("Standard") {
    CommonHelpers::ThreadSafeCounter        ctr(5);

    CHECK(ctr.GetValue() == 5);

    ctr.Increment();
    CHECK(ctr.GetValue() == 6);

    ctr.Decrement();
    CHECK(ctr.GetValue() == 5);

    std::thread                             thread(
        [&ctr](void) {
            while(ctr.GetValue() != 0)
                ctr.Decrement();
        }
    );

    ctr.wait_value(0);
    CHECK(ctr.GetValue() == 0);

    thread.join();
}
