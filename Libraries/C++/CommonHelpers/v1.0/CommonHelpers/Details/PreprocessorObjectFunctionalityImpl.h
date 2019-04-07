/////////////////////////////////////////////////////////////////////////
///
///  \file          PreprocessorObjectFunctionalityImpl.h
///  \brief         Contains functionality used to implement common functionality
///                 via preprocessor macros.
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2019-03-16 22:55:10
///
///  \note
///
///  \bug
///
/////////////////////////////////////////////////////////////////////////
///
///  \attention
///  Copyright David Brownell 2019
///  Distributed under the Boost Software License, Version 1.0. See
///  accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt.
///
/////////////////////////////////////////////////////////////////////////
#pragma once

#include "boost_extract/preprocessor/array.hpp"
#include "boost_extract/preprocessor/cat.hpp"
#include "boost_extract/preprocessor/seq.hpp"
#include "boost_extract/preprocessor/stringize.hpp"
#include "boost_extract/preprocessor/tuple.hpp"

#include "boost_extract/preprocessor/arithmetic/add.hpp"
#include "boost_extract/preprocessor/comparison/equal.hpp"
#include "boost_extract/preprocessor/comparison/greater.hpp"
#include "boost_extract/preprocessor/comparison/not_equal.hpp"
#include "boost_extract/preprocessor/control/iif.hpp"
#include "boost_extract/preprocessor/facilities/expand.hpp"
#include "boost_extract/preprocessor/variadic/to_array.hpp"
#include "boost_extract/preprocessor/variadic/to_seq.hpp"

#include "boost_extract/vmd/empty.hpp"
#include "boost_extract/vmd/is_number.hpp"
#include "boost_extract/vmd/is_tuple.hpp"

// These header files aren't used directly in this file, but are common across
// all files that include this one
#include "boost_extract/preprocessor/logical/and.hpp"
#include "boost_extract/preprocessor/logical/or.hpp"
#include "boost_extract/preprocessor/punctuation/comma_if.hpp"

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
//
// Note that this code is INCREDIBLY brittle, and must be used exactly as
// designed to work as expected. Preprocessor implementations are notoriously
// buggy from compiler to compiler. This configuration has been found to work
// with MSVC 2015 Update 2 and Clang 3.8.0.
//
//      ***** MODIFY AT YOUR OWN RISK! *****
//
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

// clang-format off

// ----------------------------------------------------------------------
// |
// |  Preprocessor Definitions
// |
// ----------------------------------------------------------------------
#define MEMBERS_ID                          0
#define BASES_ID                            1
#define METHODS_ID                          2
#define CONTEXT_ID                          3
#define _PPOFImpl_MaxNumCategories          4

#define MEMBERS(...)                        (MEMBERS_ID,(__VA_ARGS__))
#define BASES(...)                          (BASES_ID,(__VA_ARGS__))
#define METHODS(...)                        (METHODS_ID,(__VA_ARGS__))
#define CONTEXT(...)                        (CONTEXT_ID,(__VA_ARGS__))
#define FLAGS(...)                          CONTEXT(__VA_ARGS__)

/////////////////////////////////////////////////////////////////////////
///  \def           PPOFImpl
///  \brief         Preprocessor object functionality implementation. This is 
///                 a macro that is used by other macros when implementing
///                 preprocessor object functionality. This macro allows for a
///                 variety of different calling conventions, reconciling each
///                 into a consistent macro invocation.
/// 
///                 Note that all groups are converted into preprocessor sequences
///                 for ease in enumeration in the implementation macro.
/// 
///                 Examples 
///                 ========
///                     Macro with no special params
///                     ----------------------------
///
///                         // Implementation
///                         #define SIMPLE_MACRO(...)                                       PPOFImpl(SIMPLE_MACRO_Impl, , __VA_ARGS__) // Note nothing passed for PrefixArgs
///                         #define SIMPLE_MACRO_Impl(members, bases, methods, flags)       ...
///
///                     Usage                                                                                   | Output
///                     -----                                                                                   | ------
///                     SIMPLE_MACRO(10)                                                                        | SIMPLE_MACRO_Impl((10), , , )
///                     SIMPLE_MACRO(MEMBERS(100, 200))                                                         | SIMPLE_MACRO_Impl((100)(200), , , )
///                     SIMPLE_MACRO(MEMBERS(30))                                                               | SIMPLE_MACRO_Impl((30), , , )
///                     SIMPLE_MACRO(BASES(1, 2, 3))                                                            | SIMPLE_MACRO_Impl(, (1)(2)(3), , )
///                     SIMPLE_MACRO(10, 20)                                                                    | SIMPLE_MACRO_Impl((10)(20), , , )
///                     SIMPLE_MACRO(MEMBERS(100, 200), BASES(300, 400))                                        | SIMPLE_MACRO_Impl((100)(200), (300)(400), , )
///                     SIMPLE_MACRO(BASES(10), MEMBERS(30))                                                    | SIMPLE_MACRO_Impl((30), (10), , )
///                     SIMPLE_MACRO(1, 2, 3)                                                                   | SIMPLE_MACRO_Impl((1)(2)(3), , , )
///                     SIMPLE_MACRO(BASES(300, 400), FLAGS(500), MEMBERS(100, 200))                            | SIMPLE_MACRO_Impl((100)(200), (300)(400), (500), )
///                     SIMPLE_MACRO(10, 20, 30, 40, 50)                                                        | SIMPLE_MACRO_Impl((10)(20)(30)(40)(50), , , )
///                     SIMPLE_MACRO(METHODS(int Foo(void)))                                                    | SIMPLE_MACRO_Impl(, , (int Foo(void)), )
///                     SIMPLE_MACRO(METHODS(int Foo(void), char Bar(int a, bool b)))                           | SIMPLE_MACRO_Impl(, , (int Foo(void), char Bar(int a, bool b)), )
///                     SIMPLE_MACRO(MEMBERS(1, 2), MEMBERS(3, 4, 5))                                           | Compile error
///                     SIMPLE_MACRO(BASES(1, 2), BASES(2, 3, 4))                                               | Compile error
///                     SIMPLE_MACRO(FLAGS(1, 2), FLAGS(3, 4, 5))                                               | Compile error
///                     SIMPLE_MACRO(METHODS(int Foo(void), char Bar(void)), METHODS(double Baz(void)))         | Compile error
///
///                     Macro with special prefix params that should be forwarded
///                     ---------------------------------------------------------
///
///                         // Implementation
///                         #define MACRO(Value1, Value2, ...)                              PPOFImpl(MACRO_Impl, (Value1, Value2), __VA_ARGS__) // Note (Value1, Value2) passed for PrefixArgs
///                         #define MACRO_Impl(Values, members, bases, methods, flags)      ...
///
///                     Usage                                                           | Output
///                     -----                                                           | ------
///                     MACRO(a, b, 10)                                                 | MACRO_Impl((a, b), (10), , , )
///                     MACRO(a, b, FLAGS(f), BASES(b1, b2), MEMBERS(m1, m2, m3))       | MACRO_Impl((a, b), (m1)(m2)(m3), (b1)(b2), (f), )
///
#define PPOFImpl(Macro, PrefixArgs, ...)    PPOFImpl_Stage0(Macro, PrefixArgs, __VA_ARGS__)

// ----------------------------------------------------------------------
// |  Stage 0: Handle empty args

// Note that both PrefixArgs and __VA_ARGS__ may be empty
#define PPOFImpl_Stage0(Macro, PrefixArgs, ...)         PPOFImpl_Stage0_Arrays(Macro, BOOST_PP_IIF(BOOST_VMD_IS_EMPTY(PrefixArgs BOOST_PP_EMPTY()), BOOST_VMD_IDENTITY((0,())), BOOST_PP_TUPLE_TO_ARRAY)(PrefixArgs), BOOST_PP_IIF(BOOST_VMD_IS_EMPTY(__VA_ARGS__ BOOST_PP_EMPTY()), BOOST_VMD_IDENTITY((0,())), BOOST_PP_TUPLE_TO_ARRAY)((__VA_ARGS__)))

#define PPOFImpl_Stage0_Arrays(Macro, PrefixArgsArray, ArgsArray)               BOOST_PP_IIF(BOOST_PP_EQUAL(0, BOOST_PP_ARRAY_SIZE(ArgsArray)), PPOFImpl_Stage0_Arrays_EmptyArgs, PPOFImpl_Stage0_Arrays_Args)(Macro, PrefixArgsArray, ArgsArray)
#define PPOFImpl_Stage0_Arrays_EmptyArgs(Macro, PrefixArgsArray, ArgsArray)     PPOFImpl_Invoke(Macro, PrefixArgsArray, , , , )
#define PPOFImpl_Stage0_Arrays_Args(Macro, PrefixArgsArray, ArgsArray)          BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_ARRAY_SIZE(ArgsArray), _PPOFImpl_MaxNumCategories), PPOFImpl_Stage0_Arrays_Args_Greater, PPOFImpl_Stage0_Arrays_Args_LessEqual)(Macro, PrefixArgsArray, ArgsArray)

// If the number of args is greater than the number of categories, assume that
// the arguments are member items.
#define PPOFImpl_Stage0_Arrays_Args_Greater(Macro, PrefixArgsArray, ArgsArray)      PPOFImpl_Stage2(Macro, PrefixArgsArray, (1, ((MEMBERS_ID,BOOST_PP_ARRAY_TO_TUPLE(ArgsArray)))))

// If the number of args is less than or equal to the number of categories, we could
// be looking at values organized into categories or member items.
#define PPOFImpl_Stage0_Arrays_Args_LessEqual(Macro, PrefixArgsArray, ArgsArray)    BOOST_PP_CAT(PPOFImpl_Stage1_, BOOST_PP_ARRAY_SIZE(ArgsArray))(Macro, PrefixArgsArray, ArgsArray)

// ----------------------------------------------------------------------
// |  Stage 1: Dispatch arbitrarily ordered args into members, bases, methods, and context items
#define PPOFImpl_Stage1_IsComplexArg(Arg)               BOOST_PP_IIF(BOOST_VMD_IS_TUPLE(Arg), PPOFImpl_Stage1_IsComplexArg_Tuple, PPOFImpl_Stage1_IsComplexArg_NotTuple)(Arg)
#define PPOFImpl_Stage1_IsComplexArg_Tuple(Arg)         BOOST_VMD_IS_NUMBER(BOOST_PP_TUPLE_ELEM(0, Arg))
#define PPOFImpl_Stage1_IsComplexArg_NotTuple(Arg)      0

// Invoked if the caller provided 1 arg
#define PPOFImpl_Stage1_1(Macro, PrefixArgsArray, ArgsArray)                            PPOFImpl_Stage1_1Ex(Macro, PrefixArgsArray, BOOST_PP_ARRAY_ELEM(0, ArgsArray))
#define PPOFImpl_Stage1_1Ex(Macro, PrefixArgsArray, Arg1)                               BOOST_PP_IIF(PPOFImpl_Stage1_IsComplexArg(Arg1), PPOFImpl_Stage1_1Ex_Complex, PPOFImpl_Stage1_1Ex_Members)(Macro, PrefixArgsArray, Arg1)
#define PPOFImpl_Stage1_1Ex_Complex(Macro, PrefixArgsArray, Arg1)                       PPOFImpl_Stage2(Macro, PrefixArgsArray, (1, (Arg1)))
#define PPOFImpl_Stage1_1Ex_Members(Macro, PrefixArgsArray, Arg1)                       PPOFImpl_Stage2(Macro, PrefixArgsArray, (1, ((MEMBERS_ID,(Arg1)))))

// Invoked if the caller provided 2 args
#define PPOFImpl_Stage1_2(Macro, PrefixArgsArray, ArgsArray)                            PPOFImpl_Stage1_2Ex(Macro, PrefixArgsArray, BOOST_PP_ARRAY_ELEM(0, ArgsArray), BOOST_PP_ARRAY_ELEM(1, ArgsArray))
#define PPOFImpl_Stage1_2Ex(Macro, PrefixArgsArray, Arg1, Arg2)                         BOOST_PP_IIF(BOOST_PP_AND(PPOFImpl_Stage1_IsComplexArg(Arg1), PPOFImpl_Stage1_IsComplexArg(Arg2)), PPOFImpl_Stage1_2Ex_Complex, PPOFImpl_Stage1_2Ex_Members)(Macro, PrefixArgsArray, Arg1, Arg2)
#define PPOFImpl_Stage1_2Ex_Complex(Macro, PrefixArgsArray, Arg1, Arg2)                 PPOFImpl_Stage2(Macro, PrefixArgsArray, (2, (Arg1, Arg2)))
#define PPOFImpl_Stage1_2Ex_Members(Macro, PrefixArgsArray, Arg1, Arg2)                 PPOFImpl_Stage2(Macro, PrefixArgsArray, (1, ((MEMBERS_ID,(Arg1, Arg2)))))

// Invoked if the caller provided 3 args
#define PPOFImpl_Stage1_3(Macro, PrefixArgsArray, ArgsArray)                            PPOFImpl_Stage1_3Ex(Macro, PrefixArgsArray, BOOST_PP_ARRAY_ELEM(0, ArgsArray), BOOST_PP_ARRAY_ELEM(1, ArgsArray), BOOST_PP_ARRAY_ELEM(2, ArgsArray))
#define PPOFImpl_Stage1_3Ex(Macro, PrefixArgsArray, Arg1, Arg2, Arg3)                   BOOST_PP_IIF(BOOST_PP_AND(PPOFImpl_Stage1_IsComplexArg(Arg1), BOOST_PP_AND(PPOFImpl_Stage1_IsComplexArg(Arg2), PPOFImpl_Stage1_IsComplexArg(Arg3))), PPOFImpl_Stage1_3Ex_Complex, PPOFImpl_Stage1_3Ex_Members)(Macro, PrefixArgsArray, Arg1, Arg2, Arg3)
#define PPOFImpl_Stage1_3Ex_Complex(Macro, PrefixArgsArray, Arg1, Arg2, Arg3)           PPOFImpl_Stage2(Macro, PrefixArgsArray, (3, (Arg1, Arg2, Arg3)))
#define PPOFImpl_Stage1_3Ex_Members(Macro, PrefixArgsArray, Arg1, Arg2, Arg3)           PPOFImpl_Stage2(Macro, PrefixArgsArray, (1, ((MEMBERS_ID,(Arg1, Arg2, Arg3)))))

// Invoked if the caller provided 4 args
#define PPOFImpl_Stage1_4(Macro, PrefixArgsArray, ArgsArray)                            PPOFImpl_Stage1_4Ex(Macro, PrefixArgsArray, BOOST_PP_ARRAY_ELEM(0, ArgsArray), BOOST_PP_ARRAY_ELEM(1, ArgsArray), BOOST_PP_ARRAY_ELEM(2, ArgsArray), BOOST_PP_ARRAY_ELEM(3, ArgsArray))
#define PPOFImpl_Stage1_4Ex(Macro, PrefixArgsArray, Arg1, Arg2, Arg3, Arg4)             BOOST_PP_IIF(BOOST_PP_AND(PPOFImpl_Stage1_IsComplexArg(Arg1), BOOST_PP_AND(PPOFImpl_Stage1_IsComplexArg(Arg2), BOOST_PP_AND(PPOFImpl_Stage1_IsComplexArg(Arg3), PPOFImpl_Stage1_IsComplexArg(Arg4)))), PPOFImpl_Stage1_4Ex_Complex, PPOFImpl_Stage1_4Ex_Members)(Macro, PrefixArgsArray, Arg1, Arg2, Arg3, Arg4)
#define PPOFImpl_Stage1_4Ex_Complex(Macro, PrefixArgsArray, Arg1, Arg2, Arg3, Arg4)     PPOFImpl_Stage2(Macro, PrefixArgsArray, (4, (Arg1, Arg2, Arg3, Arg4)))
#define PPOFImpl_Stage1_4Ex_Members(Macro, PrefixArgsArray, Arg1, Arg2, Arg3, Arg4)     PPOFImpl_Stage2(Macro, PrefixArgsArray, (1, ((MEMBERS_ID,(Arg1, Arg2, Arg3, Arg4)))))

// ----------------------------------------------------------------------
// |  Stage 2: Reorder args based on type; also ensure that types are only provided once.
#define PPOFImpl_Stage2(Macro, PrefixArgsArray, ArgsArray)                  PPOFImpl_Stage2_Delay(PPOFImpl_Stage3)(Macro, PrefixArgsArray, BOOST_PP_SEQ_FOLD_LEFT(PPOFImpl_Stage2_Macro, (0)((0,()))((0,()))((0,()))((0,())), BOOST_PP_ARRAY_TO_SEQ(ArgsArray)))
#define PPOFImpl_Stage2_Delay(x)                                            BOOST_PP_CAT(x, PPOFImpl_Stage2_Empty())
#define PPOFImpl_Stage2_Empty()

#define PPOFImpl_Stage2_Macro(s, State, Arg)                                BOOST_PP_IIF(BOOST_PP_SEQ_ELEM(0, State), PPOFImpl_Stage2_Macro_Error, PPOFImpl_Stage2_Macro2)(State, Arg)
#define PPOFImpl_Stage2_Macro_Error(State, Arg)                             State
#define PPOFImpl_Stage2_Macro2(State, Arg)                                  PPOFImpl_Stage2_Macro3(BOOST_PP_SEQ_POP_FRONT(State), BOOST_PP_TUPLE_ELEM(0, Arg), BOOST_PP_TUPLE_ELEM(1, Arg))
#define PPOFImpl_Stage2_Macro3(ExistingData, Index, Value)                  BOOST_PP_IIF(BOOST_PP_ARRAY_SIZE(BOOST_PP_SEQ_ELEM(Index, ExistingData)), PPOFImpl_Stage2_Macro3_Error, PPOFImpl_Stage2_Macro4)(ExistingData, Index, Value)
#define PPOFImpl_Stage2_Macro3_Error(ExistingData, Index, Value)            (1)(BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_ELEM(Index, (Members, Bases, Methods, Context))))
#define PPOFImpl_Stage2_Macro4(ExistingData, Index, Value)                  (0) BOOST_PP_SEQ_REPLACE(ExistingData, Index, (1,(Value)))

// ----------------------------------------------------------------------
// |  Stage 3: Display an error message or continue processing
#define PPOFImpl_Stage3(Macro, PrefixArgsArray, ArgsSeq)                    PPOFImpl_Stage3Ex(Macro, PrefixArgsArray, BOOST_PP_SEQ_ELEM(0, ArgsSeq), BOOST_PP_SEQ_POP_FRONT(ArgsSeq))
#define PPOFImpl_Stage3Ex(Macro, PrefixArgsArray, IsError, ArgsSeq)         BOOST_PP_IIF(IsError, PPOFImpl_Stage3_Error, PPOFImpl_Stage3_NoError)(Macro, PrefixArgsArray, ArgsSeq)
#define PPOFImpl_Stage3_Error(Macro, PrefixArgsArray, ArgsSeq)              static_assert(false, "Multiple '" BOOST_PP_SEQ_ELEM(0, ArgsSeq) "' tuples were provided.");
#define PPOFImpl_Stage3_NoError(Macro, PrefixArgsArray, ArgsSeq)            PPOFImpl_Stage4(Macro, PrefixArgsArray, BOOST_PP_SEQ_ELEM(0, ArgsSeq), BOOST_PP_SEQ_ELEM(1, ArgsSeq), BOOST_PP_SEQ_ELEM(2, ArgsSeq), BOOST_PP_SEQ_ELEM(3, ArgsSeq))

// ----------------------------------------------------------------------
// |  Stage 4: Convert the arg arrays to tuples
#define PPOFImpl_Stage4(Macro, PrefixArgsArray, MembersArray, BasesArray, MethodsArray, ContextArray)                                                                                                                          \
    PPOFImpl_Stage4_Delay(PPOFImpl_Invoke) (Macro, PrefixArgsArray, PPOFImpl_Stage4_Convert(MembersArray), PPOFImpl_Stage4_Convert(BasesArray), PPOFImpl_Stage4_Convert(MethodsArray), PPOFImpl_Stage4_Convert(ContextArray))

#define PPOFImpl_Stage4_Delay(x)                        BOOST_PP_CAT(x, PPOFImpl_Stage4_Empty())
#define PPOFImpl_Stage4_Empty()

#define PPOFImpl_Stage4_Convert(Array)                  BOOST_PP_IF(BOOST_PP_ARRAY_SIZE(Array), PPOFImpl_Stage4_Convert_Valid, BOOST_VMD_EMPTY)(Array)
#define PPOFImpl_Stage4_Convert_Valid(Array)            BOOST_PP_ARRAY_ELEM(0, Array)

// ----------------------------------------------------------------------
// |  Stage 5: Invoke
#define PPOFImpl_Invoke(Macro, PrefixArgsArray, Members, Bases, Methods, Context)           BOOST_PP_IF(BOOST_PP_ARRAY_SIZE(PrefixArgsArray), PPOFImpl_Invoke_Args, PPOFImpl_Invoke_NoArgs)(Macro, PrefixArgsArray, Members, Bases, Methods, Context)
#define PPOFImpl_Invoke_NoArgs(Macro, PrefixArgsArray, Members, Bases, Methods, Context)    Macro(Members, Bases, Methods, Context)
#define PPOFImpl_Invoke_Args(Macro, PrefixArgsArray, Members, Bases, Methods, Context)      Macro(BOOST_PP_ARRAY_TO_TUPLE(PrefixArgsArray), Members, Bases, Methods, Context)

/////////////////////////////////////////////////////////////////////////
///  \def           CONTEXT_TO_TUPLE
///  \brief         Creates a tuple with values generated from the provided sequence.
/// 
///                 Examples:
///                     #define ZERO_FLAG       0
///                     #define ONE_FLAG        1
///                     #define TWO_FLAG        2
///                     #define THREE_FLAG      3
///                     #define FOUR_NAME(name) (4, name)
///                     #define FIVE_FLAG       5
///
///                     Example                                             | Output
///                     -------                                             | -------
///                     CONTEXT_TO_TUPLE(5, );                              | ( , , , , )
///                     CONTEXT_TO_TUPLE(5, (ZERO_FLAG));                   | ( 1, , , , )
///                     CONTEXT_TO_TUPLE(5, (ZERO_FLAG)(THREE_FLAG));       | ( 1, , 1, , )
///                     CONTEXT_TO_TUPLE(5, (TWO_FLAG)(FOUR_NAME(Foo)));    | ( , 1, , Foo, )
///
#define CONTEXT_TO_TUPLE(MaxNumItems, ...)  CONTEXT_TO_TUPLE_Impl(MaxNumItems, , __VA_ARGS__)

/////////////////////////////////////////////////////////////////////////
///  \def           CONTEXT_TO_INITIALIZED_TUPLE
///  \brief         See CONTEXT_TO_TUPLE
/// 
///                 Example                                                             | Output
///                 -------                                                             | ------
///                 CONTEXT_TO_INITIALIZED_TUPLE(5, 0, );                               | ( 0, 0, 0, 0, 0 )
///                 CONTEXT_TO_INITIALIZED_TUPLE(5, 0, (ZERO_FLAG));                    | ( 1, 0, 0, 0, 0 )
///                 CONTEXT_TO_INITIALIZED_TUPLE(5, 0, (ZERO_FLAG)(THREE_FLAG));        | ( 1, 0, 1, 0, 0 )
///                 CONTEXT_TO_INITIALIZED_TUPLE(5, 0, (TWO_FLAG)(FOUR_NAME(Foo)));     | ( 0, 1, 0, Foo, 0 )
///
#define CONTEXT_TO_INITIALIZED_TUPLE(MaxNumItems, DefaultValue, ...)            CONTEXT_TO_TUPLE_Impl(MaxNumItems, DefaultValue, BOOST_PP_IIF(BOOST_VMD_IS_EMPTY(__VA_ARGS__ BOOST_PP_EMPTY()), BOOST_VMD_IDENTITY((0,)), BOOST_PP_VARIADIC_TO_ARRAY)(__VA_ARGS__))
#define CONTEXT_TUPLE_TO_INITIALIZED_TUPLE(MaxNumItems, DefaultValue, Tuple)    CONTEXT_TO_TUPLE_Impl(MaxNumItems, DefaultValue, BOOST_PP_IIF(BOOST_VMD_IS_EMPTY(Tuple BOOST_PP_EMPTY()), BOOST_PP_IDENTITY((0,())), BOOST_PP_TUPLE_TO_ARRAY)(Tuple))
#define CONTEXT_SEQ_TO_INITIALIZED_TUPLE(MaxNumItems, DefaultValue, Sequence)   CONTEXT_TO_TUPLE_Impl(MaxNumItems, DefaultValue, BOOST_PP_SEQ_TO_ARRAY(Sequence))

#define CONTEXT_TO_TUPLE_Impl(MaxNumItems, InitializationValue, ContextArray)   CONTEXT_TO_TUPLE_Impl2(ContextArray, BOOST_PP_ENUM(MaxNumItems, BOOST_PP_IIF(BOOST_VMD_IS_EMPTY(InitializationValue BOOST_PP_EMPTY()), BOOST_VMD_EMPTY, BOOST_VMD_IDENTITY(InitializationValue)), _) )
#define CONTEXT_TO_TUPLE_Impl2(ContextArray, ...)                               CONTEXT_TO_TUPLE_Impl3(ContextArray, BOOST_PP_VARIADIC_TO_TUPLE(__VA_ARGS__))
#define CONTEXT_TO_TUPLE_Impl3(ContextArray, Args)                              BOOST_PP_IF(BOOST_PP_ARRAY_SIZE(ContextArray), CONTEXT_TO_TUPLE_Impl_Args, CONTEXT_TO_TUPLE_Impl_NoArgs)(ContextArray, Args)

#define CONTEXT_TO_TUPLE_Impl_Args(ContextArray, Args)                      BOOST_PP_SEQ_FOLD_LEFT(CONTEXT_TO_TUPLE_Impl_Macro, Args, BOOST_PP_ARRAY_TO_SEQ(ContextArray))
#define CONTEXT_TO_TUPLE_Impl_NoArgs(ContextArray, Args)                    Args

#if (defined __clang__)
    // Clang seems to have trouble with BOOST_VMD_IS_TUPLE when the value is a number, so check for number-ness instead.
#   define CONTEXT_TO_TUPLE_Impl_Macro(r, Args, Element)                       BOOST_PP_IIF(BOOST_VMD_IS_NUMBER(Element), CONTEXT_TO_TUPLE_Impl_Flag, CONTEXT_TO_TUPLE_Impl_Macro_Tuple)(r, Args, Element)
#else
#   define CONTEXT_TO_TUPLE_Impl_Macro(r, Args, Element)                       BOOST_PP_IIF(BOOST_VMD_IS_TUPLE(Element), CONTEXT_TO_TUPLE_Impl_Macro_Tuple, CONTEXT_TO_TUPLE_Impl_Flag)(r, Args, Element)
#endif

#define CONTEXT_TO_TUPLE_Impl_Flag(r, Args, Index)                          BOOST_PP_TUPLE_REPLACE_D(r, Args, Index, 1)
#define CONTEXT_TO_TUPLE_Impl_Macro_Tuple(r, Args, Element)                 CONTEXT_TO_TUPLE_Impl_Macro_Tuple2(r, Args, BOOST_PP_TUPLE_ELEM(0, Element), BOOST_PP_TUPLE_ELEM(1, Element))
#define CONTEXT_TO_TUPLE_Impl_Macro_Tuple2(r, Args, Index, Value)           BOOST_PP_TUPLE_REPLACE_D(r, Args, Index, Value)

/////////////////////////////////////////////////////////////////////////
///  \def           BOOST_PP_SEQ_FOR_EACH_ENUM
///  \brief         Combines the benefits of BOOST_PP_SEQ_FOR_EACH and 
///                 BOOST_PP_SEQ_ENUM by delimiting (with commas) the result
///                 of invoking the given macro on each element of the 
///                 sequence.
///
#define BOOST_PP_SEQ_FOR_EACH_ENUM(Macro, Data, Sequence)                   BOOST_PP_SEQ_FOR_EACH_I(BOOST_PP_SEQ_FOR_EACH_ENUM_Macro, (Macro, BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(Sequence)), Data, 0), Sequence)
#define BOOST_PP_SEQ_FOR_EACH_ENUM_I(Macro, Data, Sequence)                 BOOST_PP_SEQ_FOR_EACH_I(BOOST_PP_SEQ_FOR_EACH_ENUM_Macro, (Macro, BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(Sequence)), Data, 1), Sequence)

#define BOOST_PP_SEQ_FOR_EACH_ENUM_Macro(r, Data, Index, Element)                                   BOOST_PP_SEQ_FOR_EACH_ENUM_Macro2(r, BOOST_PP_TUPLE_ELEM(0, Data), BOOST_PP_TUPLE_ELEM(1, Data), BOOST_PP_TUPLE_ELEM(2, Data), BOOST_PP_TUPLE_ELEM(3, Data), Index, Element)
#define BOOST_PP_SEQ_FOR_EACH_ENUM_Macro2(r, Macro, LastIndex, Data, IncludeIndex, Index, Element)  BOOST_PP_IIF(IncludeIndex, BOOST_PP_SEQ_FOR_EACH_ENUM_Macro2_Index, BOOST_PP_SEQ_FOR_EACH_ENUM_Macro2_NoIndex)(r, Macro, Data, Index, Element) BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(Index, LastIndex))

#define BOOST_PP_SEQ_FOR_EACH_ENUM_Macro2_Index(r, Macro, Data, Index, Element)     Macro(r, Data, Index, Element)
#define BOOST_PP_SEQ_FOR_EACH_ENUM_Macro2_NoIndex(r, Macro, Data, Index, Element)   Macro(r, Data, Element)

/////////////////////////////////////////////////////////////////////////
///  \def           BOOST_PP_TUPLE_FOR_EACH
///  \brief         Provides missing boost functionality.
///
#define BOOST_PP_TUPLE_FOR_EACH(Macro, Data, Tuple)                         BOOST_PP_FOR((0)(BOOST_PP_TUPLE_SIZE(Tuple))(Macro)(Data)(Tuple)(0), BOOST_PP_TUPLE_FOR_EACH_Pred, BOOST_PP_TUPLE_FOR_EACH_Op, BOOST_PP_TUPLE_FOR_EACH_Macro)
#define BOOST_PP_TUPLE_FOR_EACH_I(Macro, Data, Tuple)                       BOOST_PP_FOR((0)(BOOST_PP_TUPLE_SIZE(Tuple))(Macro)(Data)(Tuple)(1), BOOST_PP_TUPLE_FOR_EACH_Pred, BOOST_PP_TUPLE_FOR_EACH_Op, BOOST_PP_TUPLE_FOR_EACH_Macro)

#define BOOST_PP_TUPLE_FOR_EACH_Pred(r, State)                                      BOOST_PP_NOT_EQUAL(BOOST_PP_SEQ_ELEM(0, State), BOOST_PP_SEQ_ELEM(1, State))
#define BOOST_PP_TUPLE_FOR_EACH_Op(r, State)                                        (BOOST_PP_INC(BOOST_PP_SEQ_HEAD(State))) BOOST_PP_SEQ_TAIL(State)
#define BOOST_PP_TUPLE_FOR_EACH_Macro(r, State)                                     BOOST_PP_TUPLE_FOR_EACH_Macro2(r, BOOST_PP_SEQ_ELEM(0, State), BOOST_PP_SEQ_ELEM(2, State), BOOST_PP_SEQ_ELEM(3, State), BOOST_PP_SEQ_ELEM(4, State), BOOST_PP_SEQ_ELEM(5, State))
#define BOOST_PP_TUPLE_FOR_EACH_Macro2(r, Index, Macro, Data, Tuple, IncludeIndex)  BOOST_PP_IIF(IncludeIndex, BOOST_PP_TUPLE_FOR_EACH_Macro2_Index, BOOST_PP_TUPLE_FOR_EACH_Macro2_NoIndex)(r, Index, Macro, Data, BOOST_PP_TUPLE_ELEM(Index, Tuple))
#define BOOST_PP_TUPLE_FOR_EACH_Macro2_NoIndex(r, Index, Macro, Data, Element)      Macro(r, Data, Element)
#define BOOST_PP_TUPLE_FOR_EACH_Macro2_Index(r, Index, Macro, Data, Element)        Macro(r, Data, Index, Element)

/////////////////////////////////////////////////////////////////////////
///  \def           BOOST_PP_TUPLE_FOR_EACH_ENUM
///  \brief         See BOOST_PP_SEQ_FOR_EACH_ENUM (but for tuples)
///
#define BOOST_PP_TUPLE_FOR_EACH_ENUM(Macro, Data, Tuple)                    BOOST_PP_TUPLE_FOR_EACH_I(BOOST_PP_TUPLE_FOR_EACH_ENUM_Macro, (BOOST_PP_DEC(BOOST_PP_TUPLE_SIZE(Tuple)))(Macro)(Data)(0), Tuple)
#define BOOST_PP_TUPLE_FOR_EACH_ENUM_I(Macro, Data, Tuple)                  BOOST_PP_TUPLE_FOR_EACH_I(BOOST_PP_TUPLE_FOR_EACH_ENUM_Macro, (BOOST_PP_DEC(BOOST_PP_TUPLE_SIZE(Tuple)))(Macro)(Data)(1), Tuple)

#define BOOST_PP_TUPLE_FOR_EACH_ENUM_Macro(r, Data, Index, Element)                                     BOOST_PP_TUPLE_FOR_EACH_ENUM_Macro2(r, BOOST_PP_SEQ_ELEM(0, Data), BOOST_PP_SEQ_ELEM(1, Data), BOOST_PP_SEQ_ELEM(2, Data), BOOST_PP_SEQ_ELEM(3, Data), Index, Element)
#define BOOST_PP_TUPLE_FOR_EACH_ENUM_Macro2(r, LastIndex, Macro, Data, IncludeIndex, Index, Element)    BOOST_PP_IIF(IncludeIndex, BOOST_PP_TUPLE_FOR_EACH_ENUM_Macro2_Index, BOOST_PP_TUPLE_FOR_EACH_ENUM_Macro2_NoIndex)(r, Macro, Data, Index, Element) BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(Index, LastIndex))
#define BOOST_PP_TUPLE_FOR_EACH_ENUM_Macro2_NoIndex(r, Macro, Data, Index, Element)                     Macro(r, Data, Element) 
#define BOOST_PP_TUPLE_FOR_EACH_ENUM_Macro2_Index(r, Macro, Data, Index, Element)                       Macro(r, Data, Index, Element)

// clang-format on
