/////////////////////////////////////////////////////////////////////////
///
///  \file          TestHelpers.h
///  \brief         Contains the TestHelpers namespace
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-05-14 21:06:45
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
#pragma once

#include "Compare.h"

namespace CommonHelpers {
namespace TestHelpers {

/////////////////////////////////////////////////////////////////////////
///  \fn            CompareTest
///  \brief         Test that compares two objects. Returns 0 if all tests
///                 pass, or an index value corresponding to any failures.
///
///
template <typename T>
unsigned short CompareTest(T const &a, T const &b, bool equal=false) {
    if(equal) {
        if(a == b == false)
            return 1;
        if(a != b)
            return 2;
        if(a < b != false)
            return 3;
        if(a <= b == false)
            return 4;
        if(a > b != false)
            return 5;
        if(a >= b == false)
            return 6;
    } else {
        if(a == b)
            return 7;
        if(a != b == false)
            return 8;
        if(a < b == false)
            return 9;
        if(a <= b == false)
            return 10;
        if(a > b)
            return 11;
        if(a >= b)
            return 12;
    }

    return 0;
}

} // namespace TestHelpers
} // namespace CommonHelpers
