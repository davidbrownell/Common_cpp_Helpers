/////////////////////////////////////////////////////////////////////////
///
///  \file          Finally.h
///  \brief         Contains the final_action class, finally method, and
///                 FINALLY macro.
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-05-30 16:22:44
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

#include "Details/boost_extract/preprocessor/cat.hpp"

namespace CommonHelpers {

// ----------------------------------------------------------------------
// |
// |  Preprocessor Definitions
// |
// ----------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////
///  \fn            FINALLY
///  \brief         Calls the provided lambda expression on exit using
///                 `Finally`.
///
///                 Example:
///                     FINALLY([](void) { std::cout << "Done!\n"; });
///
#define FINALLY(...)                        auto const FINALLY_ANONYMOUS_VARIABLE_Impl() (CommonHelpers::Finally(__VA_ARGS__))

// ----------------------------------------------------------------------
// |
// |  Public Types
// |
// ----------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////
///  \class         FinalAction
///  \brief         This is the same functionality found in `gsl::final_action`.
///                 The code is copied here because the current version of gsl
///                 has problems with clang on Windows.
///
template <typename FuncT>
class FinalAction {
public:
    // ----------------------------------------------------------------------
    // |  Public Methods
    explicit FinalAction(FuncT f) noexcept : f_(std::move(f)) {}

    FinalAction(FinalAction && other) noexcept : f_(std::move(other.f_)) {
        other.invoke_ = false;
    }

    FinalAction(FinalAction const &) = delete;
    FinalAction & operator=(FinalAction const &) = delete;
    FinalAction & operator=(FinalAction &&) = delete;

    ~FinalAction(void) noexcept {
        if(invoke_) f_();
    }

private:
    // ----------------------------------------------------------------------
    // |  Private Data
    FuncT                                   f_;
    bool                                    invoke_{true};
};

// ----------------------------------------------------------------------
// |
// |  Public Methods
// |
// ----------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////
///  \fn            Finally
///  \brief         Convenience method to create a `FinalAction` object.
///
template <typename FuncT>
FinalAction<FuncT> Finally(FuncT const &f) noexcept {
    return FinalAction<FuncT>(f);
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// |
// |  Implementation
// |
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
#if (defined _MSC_VER)
#   define FINALLY_ANONYMOUS_VARIABLE_Impl_Suffix()     __COUNTER__
#else
#   define FINALLY_ANONYMOUS_VARIABLE_Impl_Suffix()     __LINE__
#endif

#define FINALLY_ANONYMOUS_VARIABLE_Impl()               BOOST_PP_CAT(FinallyTemporary, FINALLY_ANONYMOUS_VARIABLE_Impl_Suffix())

} // namespace CommonHelpers
