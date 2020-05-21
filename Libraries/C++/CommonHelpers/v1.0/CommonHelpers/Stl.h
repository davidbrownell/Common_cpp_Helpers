/////////////////////////////////////////////////////////////////////////
///
///  \file          Stl.h
///  \brief         Contains the Stl namespace
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-05-21 14:36:14
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

#include <vector>

namespace CommonHelpers {

/////////////////////////////////////////////////////////////////////////
///  \namespace     Stl
///  \brief         Various helpers for the stl
///
namespace Stl {

/////////////////////////////////////////////////////////////////////////
///  \fn            CreateVector
///  \brief         Non-copyable objects cannot be used with initializer lists,
///                 which is why this method is necessary.
///
template <typename T, typename... ArgsT>
std::vector<T> CreateVector(ArgsT &&... args);

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// |
// |  Implementation
// |
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
namespace Details {

template <typename T>
void CreateVectorImpl(std::vector<T> &)
{}

template <typename T, typename FirstArgT, typename... ArgsT>
void CreateVectorImpl(std::vector<T> &results, FirstArgT && first, ArgsT &&... args) {
    results.emplace_back(std::forward<FirstArgT>(first));
    CreateVectorImpl(results, std::forward<ArgsT>(args)...);
}

} // namespace Details

template <typename T, typename... ArgsT>
std::vector<T> CreateVector(ArgsT &&... args) {
    std::vector<T>                          results;

    Details::CreateVectorImpl(results, std::forward<ArgsT>(args)...);

    return results;
}

} // namespace Stl
} // namespace CommonHelpers
