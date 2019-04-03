# /* Copyright (C) 2001
#  * Housemarque Oy
#  * http://www.housemarque.com
#  *
#  * Distributed under the Boost Software License, Version 1.0. (See
#  * accompanying file LICENSE_1_0.txt or copy at
#  * http://www.boost.org/LICENSE_1_0.txt)
#  */
#
# /* Revised by Paul Mensonides (2002) */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef BOOST_PREPROCESSOR_LIST_FOR_EACH_PRODUCT_HPP
# define BOOST_PREPROCESSOR_LIST_FOR_EACH_PRODUCT_HPP
#
# include <boost_extract/preprocessor/config/config.hpp>
# include <boost_extract/preprocessor/control/if.hpp>
# include <boost_extract/preprocessor/list/adt.hpp>
# include <boost_extract/preprocessor/list/to_tuple.hpp>
# include <boost_extract/preprocessor/repetition/for.hpp>
# include <boost_extract/preprocessor/tuple/elem.hpp>
# include <boost_extract/preprocessor/tuple/to_list.hpp>
# include <boost_extract/preprocessor/tuple/rem.hpp>
# include <boost_extract/preprocessor/tuple/reverse.hpp>
#
# /* BOOST_PP_LIST_FOR_EACH_PRODUCT */
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_EDG()
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT(macro, size, tuple) BOOST_PP_LIST_FOR_EACH_PRODUCT_E(BOOST_PP_FOR, macro, size, BOOST_PP_TUPLE_TO_LIST(size, tuple))
# else
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT(macro, size, tuple) BOOST_PP_LIST_FOR_EACH_PRODUCT_Q(macro, size, tuple)
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_Q(macro, size, tuple) BOOST_PP_LIST_FOR_EACH_PRODUCT_E(BOOST_PP_FOR, macro, size, BOOST_PP_TUPLE_TO_LIST(size, tuple))
# endif
#
# /* BOOST_PP_LIST_FOR_EACH_PRODUCT_R */
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_EDG()
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_R(r, macro, size, tuple) BOOST_PP_LIST_FOR_EACH_PRODUCT_E(BOOST_PP_FOR_ ## r, macro, size, BOOST_PP_TUPLE_TO_LIST(size, tuple))
# else
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_R(r, macro, size, tuple) BOOST_PP_LIST_FOR_EACH_PRODUCT_R_Q(r, macro, size, tuple)
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_R_Q(r, macro, size, tuple) BOOST_PP_LIST_FOR_EACH_PRODUCT_E(BOOST_PP_FOR_ ## r, macro, size, BOOST_PP_TUPLE_TO_LIST(size, tuple))
# endif
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_EDG()
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_E(impl, macro, size, lists) impl((BOOST_PP_LIST_FIRST(lists), BOOST_PP_LIST_REST(lists), BOOST_PP_NIL, macro, size), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_0)
# else
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_E(impl, macro, size, lists) BOOST_PP_LIST_FOR_EACH_PRODUCT_E_D(impl, macro, size, lists)
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_E_D(impl, macro, size, lists) impl((BOOST_PP_LIST_FIRST(lists), BOOST_PP_LIST_REST(lists), BOOST_PP_NIL, macro, size), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_0)
# endif
#
# if BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_STRICT()
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_P(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_P_I data
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_P_I(a, b, res, macro, size) BOOST_PP_LIST_IS_CONS(a)
# else
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_P(r, data) BOOST_PP_LIST_IS_CONS(BOOST_PP_TUPLE_ELEM(5, 0, data))
# endif
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_MWCC()
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_O(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_O_I data
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_O_I(a, b, res, macro, size) (BOOST_PP_LIST_REST(a), b, res, macro, size)
# else
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_O(r, data) (BOOST_PP_LIST_REST(BOOST_PP_TUPLE_ELEM(5, 0, data)), BOOST_PP_TUPLE_ELEM(5, 1, data), BOOST_PP_TUPLE_ELEM(5, 2, data), BOOST_PP_TUPLE_ELEM(5, 3, data), BOOST_PP_TUPLE_ELEM(5, 4, data))
# endif
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_EDG()
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_I(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_I_I(r, BOOST_PP_TUPLE_ELEM(5, 0, data), BOOST_PP_TUPLE_ELEM(5, 1, data), BOOST_PP_TUPLE_ELEM(5, 2, data), BOOST_PP_TUPLE_ELEM(5, 3, data), BOOST_PP_TUPLE_ELEM(5, 4, data))
# else
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_I(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_I_D(r, BOOST_PP_TUPLE_REM_5 data)
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_I_D(r, data_e) BOOST_PP_LIST_FOR_EACH_PRODUCT_I_I(r, data_e)
# endif
#
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_I_I(r, a, b, res, macro, size) BOOST_PP_LIST_FOR_EACH_PRODUCT_I_II(r, macro, BOOST_PP_LIST_TO_TUPLE_R(r, (BOOST_PP_LIST_FIRST(a), res)), size)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_I_II(r, macro, args, size) BOOST_PP_LIST_FOR_EACH_PRODUCT_I_III(r, macro, args, size)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_I_III(r, macro, args, size) macro(r, BOOST_PP_TUPLE_REVERSE(size, args))
#
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, i) BOOST_PP_IF(BOOST_PP_LIST_IS_CONS(BOOST_PP_TUPLE_ELEM(5, 1, data)), BOOST_PP_LIST_FOR_EACH_PRODUCT_N_ ## i, BOOST_PP_LIST_FOR_EACH_PRODUCT_I)
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_MWCC()
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data) BOOST_PP_LIST_FOR_EACH_PRODUCT_H_I data
# else
#    define BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data) BOOST_PP_LIST_FOR_EACH_PRODUCT_H_I(BOOST_PP_TUPLE_ELEM(5, 0, data), BOOST_PP_TUPLE_ELEM(5, 1, data), BOOST_PP_TUPLE_ELEM(5, 2, data), BOOST_PP_TUPLE_ELEM(5, 3, data), BOOST_PP_TUPLE_ELEM(5, 4, data))
# endif
#
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_H_I(a, b, res, macro, size) (BOOST_PP_LIST_FIRST(b), BOOST_PP_LIST_REST(b), (BOOST_PP_LIST_FIRST(a), res), macro, size)
#
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_0(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 0)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_1(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 1)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_2(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 2)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_3(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 3)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_4(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 4)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_5(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 5)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_6(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 6)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_7(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 7)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_8(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 8)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_9(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 9)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_10(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 10)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_11(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 11)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_12(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 12)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_13(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 13)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_14(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 14)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_15(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 15)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_16(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 16)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_17(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 17)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_18(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 18)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_19(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 19)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_20(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 20)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_21(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 21)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_22(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 22)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_23(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 23)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_24(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 24)(r, data)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_M_25(r, data) BOOST_PP_LIST_FOR_EACH_PRODUCT_C(data, 25)(r, data)
#
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_0(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_1)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_1(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_2)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_2(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_3)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_3(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_4)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_4(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_5)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_5(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_6)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_6(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_7)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_7(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_8)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_8(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_9)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_9(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_10)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_10(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_11)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_11(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_12)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_12(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_13)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_13(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_14)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_14(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_15)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_15(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_16)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_16(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_17)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_17(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_18)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_18(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_19)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_19(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_20)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_20(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_21)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_21(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_22)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_22(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_23)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_23(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_24)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_24(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_25)
# define BOOST_PP_LIST_FOR_EACH_PRODUCT_N_25(r, data) BOOST_PP_FOR_ ## r(BOOST_PP_LIST_FOR_EACH_PRODUCT_H(data), BOOST_PP_LIST_FOR_EACH_PRODUCT_P, BOOST_PP_LIST_FOR_EACH_PRODUCT_O, BOOST_PP_LIST_FOR_EACH_PRODUCT_M_26)
#
# endif
