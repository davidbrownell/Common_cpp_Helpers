/////////////////////////////////////////////////////////////////////////
///
///  \file          Misc.h
///  \brief         Miscellaneous utilities
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-05-09 21:56:05
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
#pragma once

#include "TypeTraits.h"

#include "Details/boost_extract/preprocessor/control/iif.hpp"
#include "Details/boost_extract/preprocessor/empty.hpp"

#include "Details/boost_extract/vmd/empty.hpp"
#include "Details/boost_extract/vmd/is_empty.hpp"

#include <stdexcept>

namespace CommonHelpers {

// ----------------------------------------------------------------------
// |
// |  Preprocessor Definitions
// |
// ----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
///  \def           ENSURE_ARGUMENT
///  \brief         Ensures that the provided statement evaluates to true
///                 for the variable provided.
///
#define ENSURE_ARGUMENT(var, ...)           ENSURE_ARGUMENT_Impl(var, __VA_ARGS__)
#define ENSURE_ARGUMENT_STR(str, stmt)      ENSURE_ARGUMENT_STR_Impl(str, stmt)

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// |
// |  Implementation
// |
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
#define ENSURE_ARGUMENT_Impl(var, ...)                  BOOST_PP_IIF(BOOST_VMD_IS_EMPTY(__VA_ARGS__ BOOST_PP_EMPTY()), ENSURE_ARGUMENT_Impl_Empty, ENSURE_ARGUMENT_Impl_NotEmpty)(var, __VA_ARGS__)
#define ENSURE_ARGUMENT_Impl_Empty(var, _)              ENSURE_ARGUMENT_Impl_NotEmpty(var, var)
#define ENSURE_ARGUMENT_Impl_NotEmpty(var, stmt)        { UNUSED(var); ENSURE_ARGUMENT_STR_Impl(#var, stmt) }

#define ENSURE_ARGUMENT_STR_Impl(str, stmt)             { if(!(stmt)) throw std::invalid_argument(str); }

} // namespace CommonHelpers
