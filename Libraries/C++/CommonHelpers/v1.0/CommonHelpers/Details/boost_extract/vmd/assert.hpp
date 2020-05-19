
//  (C) Copyright Edward Diener 2011-2015
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if !defined(BOOST_VMD_ASSERT_HPP)
#define BOOST_VMD_ASSERT_HPP

#include "detail/setup.hpp"

#if BOOST_PP_VARIADICS

#include "detail/assert.hpp"

/*

  The succeeding comments in this file are in doxygen format.

*/

/** \file
*/

/** \def BOOST_VMD_ASSERT(...)

    \brief Conditionally causes an error to be generated.

    ...  = variadic parameters, maximum of 2 will be considered.
           Any variadic parameters beyond the maximum of 2 are just ignored.

    The first variadic parameter is:

    cond = A condition that determines whether an assertion occurs.  Valid values range from 0 to BOOST_PP_LIMIT_MAG.

       The second variadic parameter (optional) is:

       errstr = An error string for generating a compiler error when using the VC++ compiler.
                The VC++ compiler is incapable of producing a preprocessor error so when the 'cond'
                is 0, a compiler error is generated by outputting C++ code in the form of:

                typedef char errstr[-1];

                The errstr defaults to BOOST_VMD_ASSERT_ERROR if not supplied.
                It is only relevant for VC++.

    returns = If cond expands to 0, this macro causes an error.  Otherwise, it expands to nothing.
              For all compilers other than Visual C++ the error is a preprocessing error.
              For Visual C++ the error is caused by output invalid C++: this error could be
              masked if the invalid output is ignored by a macro which invokes this macro.

*/
# define BOOST_VMD_ASSERT(...) \
    BOOST_VMD_DETAIL_ASSERT(__VA_ARGS__) \
/**/

#endif /* BOOST_PP_VARIADICS */
#endif /* BOOST_VMD_ASSERT_HPP */
