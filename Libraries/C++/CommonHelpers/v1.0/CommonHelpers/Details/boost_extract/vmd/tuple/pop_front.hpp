
//  (C) Copyright Edward Diener 2015
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if !defined(BOOST_VMD_TUPLE_POP_FRONT_HPP)
#define BOOST_VMD_TUPLE_POP_FRONT_HPP

#include "../detail/setup.hpp"

#if BOOST_PP_VARIADICS

#include "../../preprocessor/comparison/equal.hpp"
#include "../../preprocessor/control/iif.hpp"
#include "../../preprocessor/tuple/pop_front.hpp"
#include "../../preprocessor/tuple/size.hpp"
#include "../empty.hpp"

/*

  The succeeding comments in this file are in doxygen format.

*/

/** \file
*/

/** \def BOOST_VMD_TUPLE_POP_FRONT(tuple)

    \brief pops an element from the front of a tuple.

    tuple = tuple to pop an element from.

    If the tuple is an empty tuple the result is undefined.
    If the tuple is a single element the result is an empty tuple.
    Otherwise the result is a tuple after removing the first element.
*/

#define BOOST_VMD_TUPLE_POP_FRONT(tuple) \
    BOOST_PP_IIF \
        ( \
        BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(tuple),1), \
        BOOST_VMD_EMPTY, \
        BOOST_PP_TUPLE_POP_FRONT \
        ) \
    (tuple) \
/**/

/** \def BOOST_VMD_TUPLE_POP_FRONT_Z(z,tuple)

    \brief pops an element from the front of a tuple. It reenters BOOST_PP_REPEAT with maximum efficiency.

    z     = the next available BOOST_PP_REPEAT dimension.
    tuple = tuple to pop an element from.

    If the tuple is an empty tuple the result is undefined.
    If the tuple is a single element the result is an empty tuple.
    Otherwise the result is a tuple after removing the first element.
*/

#define BOOST_VMD_TUPLE_POP_FRONT_Z(z,tuple) \
    BOOST_PP_IIF \
        ( \
        BOOST_PP_EQUAL(BOOST_PP_TUPLE_SIZE(tuple),1), \
        BOOST_VMD_EMPTY, \
        BOOST_PP_TUPLE_POP_FRONT_Z \
        ) \
    (z,tuple) \
/**/

#endif /* BOOST_PP_VARIADICS */
#endif /* BOOST_VMD_TUPLE_POP_FRONT_HPP */
