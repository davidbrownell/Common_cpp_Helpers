
//  (C) Copyright Edward Diener 2015
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if !defined(BOOST_VMD_SEQ_PUSH_BACK_HPP)
#define BOOST_VMD_SEQ_PUSH_BACK_HPP

#include <boost_extract/vmd/detail/setup.hpp>

#if BOOST_PP_VARIADICS

#include <boost_extract/preprocessor/control/iif.hpp>
#include <boost_extract/preprocessor/seq/push_back.hpp>
#include <boost_extract/vmd/identity.hpp>
#include <boost_extract/vmd/is_empty.hpp>

/*

  The succeeding comments in this file are in doxygen format.

*/

/** \file
*/

/** \def BOOST_VMD_SEQ_PUSH_BACK(seq,elem)

    \brief appends an element to the end of a seq. 

    seq = seq to to append an element to.
    elem  = element to append.

    If the seq is an empty seq the result is a seq with the single element.
    Otherwise the result is a seq after adding the element to the end.
*/

#define BOOST_VMD_SEQ_PUSH_BACK(seq,elem) \
    BOOST_VMD_IDENTITY_RESULT \
        ( \
        BOOST_PP_IIF \
            ( \
            BOOST_VMD_IS_EMPTY(seq), \
            BOOST_VMD_IDENTITY((elem)), \
            BOOST_PP_SEQ_PUSH_BACK \
            ) \
        (seq,elem) \
        ) \
/**/

#endif /* BOOST_PP_VARIADICS */
#endif /* BOOST_VMD_SEQ_PUSH_BACK_HPP */
