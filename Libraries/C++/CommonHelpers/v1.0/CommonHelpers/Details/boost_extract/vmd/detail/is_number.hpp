
//  (C) Copyright Edward Diener 2011-2015
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).

#if !defined(BOOST_VMD_DETAIL_IS_NUMBER_HPP)
#define BOOST_VMD_DETAIL_IS_NUMBER_HPP

#include "../../preprocessor/control/iif.hpp"
#include "../identity.hpp"
#include "equal_type.hpp"
#include "identifier_type.hpp"
#include "is_identifier.hpp"
#include "number_registration.hpp"

#define BOOST_VMD_DETAIL_IS_NUMBER_TYPE(vseq) \
    BOOST_VMD_DETAIL_EQUAL_TYPE \
        ( \
        BOOST_VMD_TYPE_NUMBER, \
        BOOST_VMD_DETAIL_IDENTIFIER_TYPE(vseq) \
        ) \
/**/

#define BOOST_VMD_DETAIL_IS_NUMBER(vseq) \
    BOOST_VMD_IDENTITY_RESULT \
        ( \
        BOOST_PP_IIF \
            ( \
            BOOST_VMD_DETAIL_IS_IDENTIFIER_SINGLE(vseq), \
            BOOST_VMD_DETAIL_IS_NUMBER_TYPE, \
            BOOST_VMD_IDENTITY(0) \
            ) \
        (vseq) \
        ) \
/**/

#endif /* BOOST_VMD_DETAIL_IS_NUMBER_HPP */
