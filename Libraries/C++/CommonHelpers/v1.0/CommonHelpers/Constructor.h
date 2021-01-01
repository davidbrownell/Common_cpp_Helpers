/////////////////////////////////////////////////////////////////////////
///
///  \file          Constructor.h
///  \brief         Contains macros that help implement constructors
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2019-03-27 16:27:01
///
///  \note
///
///  \bug
///
/////////////////////////////////////////////////////////////////////////
///
///  \attention
///  Copyright David Brownell 2019-21
///  Distributed under the Boost Software License, Version 1.0. See
///  accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt.
///
/////////////////////////////////////////////////////////////////////////
#pragma once

#include "TypeTraits.h"
#include "Details/PreprocessorObjectFunctionalityImpl.h"

namespace CommonHelpers {

// ----------------------------------------------------------------------
// |
// |  Preprocessor Definitions
// |
// ----------------------------------------------------------------------

// clang-format off

/////////////////////////////////////////////////////////////////////////
///  \def           CONSTRUCTOR
///  \brief         Implements a constructor for the object.
///
///                     template <typename ArgT>
///                     MyClass(ArgT && arg);
///
///                 The following class methods will be called if they exist:
///                     void FinalConstruct(void)
///
///                 Example                                                                 | Comment
///                 -------                                                                 | -------
///                 CONSTRUCTOR(MyClass)                                                    | No members or bases
///                 CONSTRUCTOR(MyClass, a, b, c)                                           | All members
///                 CONSTRUCTOR(MyClass, MEMBERS(a, b, c))                                  | Explicit members
///                 CONSTRUCTOR(MyClass, MEMBERS(a, b), BASES(c, d))                        | Explicit members and bases
///                 CONSTRUCTOR(MyClass, BASES(c, d))                                       | Explicit bases
///                 CONSTRUCTOR(MyClass, BASES(c, d), MEMBERS(a, b))                        | Explicit members and bases (alternate order)
///                 CONSTRUCTOR(MyClass, MEMBERS(a, b), FLAGS(CONSTRUCTOR_BASE_ARGS_0(3)))  | Creates a constructor with 5 arguments - 2 for a and b and 3 for the base class
///
#define CONSTRUCTOR(ClassName, ...)         PPOFImpl(CONSTRUCTOR_Impl, (ClassName), __VA_ARGS__)

// Flags
#define CONSTRUCTOR_BASES_BEFORE_MEMBERS    (0,0)       ///< By default, members are constructed before bases. Specifying this flag ensures that bases are constructed before members.

#define CONSTRUCTOR_BASE_ARGS_0(NumArgs)    (1,NumArgs) ///< Overrides the number of arguments necessary to construct the Nth base object
#define CONSTRUCTOR_BASE_ARGS_1(NumArgs)    (2,NumArgs) ///< Overrides the number of arguments necessary to construct the Nth base object
#define CONSTRUCTOR_BASE_ARGS_2(NumArgs)    (3,NumArgs) ///< Overrides the number of arguments necessary to construct the Nth base object
#define CONSTRUCTOR_BASE_ARGS_3(NumArgs)    (4,NumArgs) ///< Overrides the number of arguments necessary to construct the Nth base object
#define CONSTRUCTOR_BASE_ARGS_4(NumArgs)    (5,NumArgs) ///< Overrides the number of arguments necessary to construct the Nth base object
#define CONSTRUCTOR_BASE_ARGS_5(NumArgs)    (6,NumArgs) ///< Overrides the number of arguments necessary to construct the Nth base object
#define CONSTRUCTOR_BASE_ARGS_6(NumArgs)    (7,NumArgs) ///< Overrides the number of arguments necessary to construct the Nth base object
#define CONSTRUCTOR_BASE_ARGS_7(NumArgs)    (8,NumArgs) ///< Overrides the number of arguments necessary to construct the Nth base object
#define CONSTRUCTOR_BASE_ARGS_8(NumArgs)    (9,NumArgs) ///< Overrides the number of arguments necessary to construct the Nth base object
#define CONSTRUCTOR_BASE_ARGS_9(NumArgs)    (10,NumArgs) ///< Overrides the number of arguments necessary to construct the Nth base object

#define __NUM_CONSTRUCTOR_FLAGS             11

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// |
// |  Implementation
// |
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

#define CONSTRUCTOR_Impl(ClassNameTuple, Members, Bases, Methods, Flags)                                                                                                                                                                                                                                    \
    static_assert(BOOST_PP_IIF(BOOST_VMD_IS_EMPTY(Methods BOOST_PP_EMPTY()), true, false), "This macro does not support methods");                                                                                                                                                                           \
    CONSTRUCTOR_Impl_Delay(CONSTRUCTOR_Impl2)(BOOST_PP_TUPLE_ELEM(0, ClassNameTuple), BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(Members BOOST_PP_EMPTY())), Members, BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(Bases BOOST_PP_EMPTY())), Bases, Flags)

#define CONSTRUCTOR_Impl_Delay(x)           BOOST_PP_CAT(x, CONSTRUCTOR_Impl_Empty())
#define CONSTRUCTOR_Impl_Empty()

#define CONSTRUCTOR_Impl2(ClassName, HasMembers, Members, HasBases, Bases, Flags)                           CONSTRUCTOR_Impl3(ClassName, HasMembers, Members, HasBases, Bases, CONTEXT_TUPLE_TO_INITIALIZED_TUPLE(__NUM_CONSTRUCTOR_FLAGS, 1, Flags))
#define CONSTRUCTOR_Impl3(ClassName, HasMembers, Members, HasBases, Bases, FlagsData)                       CONSTRUCTOR_Impl_Invoke(ClassName, HasMembers, Members, HasBases, Bases, BOOST_PP_TUPLE_ELEM(0, FlagsData), BOOST_PP_TUPLE_POP_FRONT(FlagsData))

#define CONSTRUCTOR_Impl_Invoke(ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)              BOOST_PP_IIF(HasBases, CONSTRUCTOR_Impl_Invoke_Check, CONSTRUCTOR_Impl_Invoke2)(ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)
#define CONSTRUCTOR_Impl_Invoke_Check(ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)        BOOST_PP_IIF(BOOST_PP_GREATER(BOOST_PP_TUPLE_SIZE(Bases), __NUM_CONSTRUCTOR_FLAGS), CONSTRUCTOR_Impl_Invoke_Check_Error, CONSTRUCTOR_Impl_Invoke2)(ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)
#define CONSTRUCTOR_Impl_Invoke_Check_Error(ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)  static_assert(false, "A maximum of " BOOST_PP_STRINGIZE(__NUM_CONSTRUCTOR_Flags) " bases may be specified, but " BOOST_PP_STRINGIZE(BOOST_PP_TUPLE_SIZE(Bases)) " were provided for '" BOOST_PP_STRINGIZE(ClassName) "'.");

#define CONSTRUCTOR_Impl_Invoke2(ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)                                                                                                                                                                                                 \
    /* Special-case check for a class with no members but one or more bases, each that take 0 args */                                                                                                                                                                                                               \
    CONSTRUCTOR_Impl_Invoke3(BOOST_PP_ADD(CONSTRUCTOR_Impl_Num_Members(HasMembers, Members), CONSTRUCTOR_Impl_Num_Bases(HasBases, Bases, BasesNumArgs)), ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)

#define CONSTRUCTOR_Impl_Num_Members(HasMembers, Members)                   BOOST_PP_IIF(HasMembers, BOOST_PP_TUPLE_SIZE, BOOST_VMD_IDENTITY(0))(Members)
#define CONSTRUCTOR_Impl_Num_Bases(HasBases, Bases, BasesNumArgs)           BOOST_PP_IIF(HasBases, CONSTRUCTOR_Impl_Num_Bases2, BOOST_VMD_IDENTITY(0))(Bases, BasesNumArgs)
#define CONSTRUCTOR_Impl_Num_Bases2(Bases, BasesNumArgs)                    BOOST_PP_SEQ_FOLD_LEFT(CONSTRUCTOR_Impl_Num_Bases2_Macro, 0, BOOST_PP_SEQ_FIRST_N(BOOST_PP_TUPLE_SIZE(Bases), BOOST_PP_TUPLE_TO_SEQ(BasesNumArgs)))
#define CONSTRUCTOR_Impl_Num_Bases2_Macro(s, State, Value)                  BOOST_PP_ADD(State, Value)

#define CONSTRUCTOR_Impl_Invoke3(NumArgs, ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)        BOOST_PP_IIF(BOOST_PP_EQUAL(NumArgs, 0), CONSTRUCTOR_Impl_Invoke_Empty, CONSTRUCTOR_Impl_Invoke_Standard)(NumArgs, ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)

#define CONSTRUCTOR_Impl_Invoke_Empty(NumArgs, ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)   \
    ClassName(void) { CommonHelpers::TypeTraits::Access::FinalConstruct(*this); }

#define CONSTRUCTOR_Impl_Invoke_Standard(NumArgs, ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)    \
    CONSTRUCTOR_Impl_Invoke_Template(NumArgs, ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)        \
    ClassName(                                                                                                                          \
        CONSTRUCTOR_Impl_Invoke_Args(ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)                 \
    )                                                                                                                                   \
        BOOST_PP_IIF(BOOST_PP_OR(HasMembers, HasBases), BOOST_PP_IDENTITY(:), BOOST_VMD_EMPTY)()                                        \
        BOOST_PP_IIF(HasBases, CONSTRUCTOR_Impl_Invoke_Init_Bases, BOOST_VMD_EMPTY)(Bases, BasesNumArgs)                                \
        BOOST_PP_COMMA_IF(BOOST_PP_AND(HasMembers, HasBases))                                                                           \
        BOOST_PP_IIF(HasMembers, CONSTRUCTOR_Impl_Invoke_Init_Members, BOOST_VMD_EMPTY)(Members)                                        \
    {                                                                                                                                   \
        CommonHelpers::TypeTraits::Access::FinalConstruct(*this);                                                                       \
    }

#define CONSTRUCTOR_Impl_Invoke_Template(NumArgs, ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)                                                                    \
    template <                                                                                                                                                                                          \
        BOOST_PP_IIF(MembersBeforeBases, CONSTRUCTOR_Impl_Invoke_Template_MembersFirst, CONSTRUCTOR_Impl_Invoke_Template_BasesFirst)(ClassName, HasMembers, Members, HasBases, Bases, BasesNumArgs)     \
        BOOST_PP_IIF(BOOST_PP_EQUAL(NumArgs, 1), CONSTRUCTOR_Impl_Invoke_Template_SingleArgCheck, BOOST_VMD_EMPTY)(ClassName, HasMembers, Members, HasBases, Bases)                                                                                                \
    >

#define CONSTRUCTOR_Impl_Invoke_Template_MembersFirst(ClassName, HasMembers, Members, HasBases, Bases, BasesNumArgs)    \
    BOOST_PP_IIF(HasMembers, CONSTRUCTOR_Impl_Invoke_Template_Members, BOOST_VMD_EMPTY)(Members)                        \
    BOOST_PP_COMMA_IF(BOOST_PP_AND(HasMembers, HasBases))                                                               \
    BOOST_PP_IIF(HasBases, CONSTRUCTOR_Impl_Invoke_Template_Bases, BOOST_VMD_EMPTY)(Bases, BasesNumArgs)

#define CONSTRUCTOR_Impl_Invoke_Template_BasesFirst(ClassName, HasMembers, Members, HasBases, Bases, BasesNumArgs)  \
    BOOST_PP_IIF(HasBases, CONSTRUCTOR_Impl_Invoke_Template_Bases, BOOST_VMD_EMPTY)(Bases, BasesNumArgs)            \
    BOOST_PP_COMMA_IF(BOOST_PP_AND(HasMembers, HasBases))                                                           \
    BOOST_PP_IIF(HasMembers, CONSTRUCTOR_Impl_Invoke_Template_Members, BOOST_VMD_EMPTY)(Members)

#define CONSTRUCTOR_Impl_Invoke_Template_Members(Members)                   BOOST_PP_TUPLE_FOR_EACH_ENUM(CONSTRUCTOR_Impl_Invoke_Template_Members_Macro, _, Members)
#define CONSTRUCTOR_Impl_Invoke_Template_Members_Macro(r, _, Member)        typename BOOST_PP_CAT(Member, T)

#define CONSTRUCTOR_Impl_Invoke_Template_Bases(Bases, BasesNumArgs)                     CONSTRUCTOR_Impl_Invoke_EnumBaseInfo(CONSTRUCTOR_Impl_Invoke_Template_Bases_Macro, Bases, BasesNumArgs)
#define CONSTRUCTOR_Impl_Invoke_Template_Bases_Macro(BaseIndex, BaseName, ArgIndex)     typename CONSTRUCTOR_Impl_Invoke_BaseTypeName(BaseIndex, ArgIndex)

#define CONSTRUCTOR_Impl_Invoke_Template_SingleArgCheck(ClassName, HasMembers, Members, HasBases, Bases)  CONSTRUCTOR_Impl_Invoke_Template_SingleArgCheck2(ClassName, BOOST_PP_IIF(HasMembers, CONSTRUCTOR_Impl_Invoke_Template_SingleArgCheck_Members, CONSTRUCTOR_Impl_Invoke_Template_SingleArgCheck_Bases)(Members, Bases))
#define CONSTRUCTOR_Impl_Invoke_Template_SingleArgCheck2(ClassName, Typename)                             , std::enable_if_t<std::is_same_v<ClassName, std::decay_t<std::remove_reference_t<Typename>>> == false, std::add_pointer_t< Typename >> = nullptr
#define CONSTRUCTOR_Impl_Invoke_Template_SingleArgCheck_Members(Members, _)                               BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(0, Members), T)
#define CONSTRUCTOR_Impl_Invoke_Template_SingleArgCheck_Bases(_, Bases)                                   CONSTRUCTOR_Impl_Invoke_BaseTypeName(0, 0)

#define CONSTRUCTOR_Impl_Invoke_Args(ClassName, HasMembers, Members, HasBases, Bases, MembersBeforeBases, BasesNumArgs)     BOOST_PP_IIF(MembersBeforeBases, CONSTRUCTOR_Impl_Invoke_Args_MembersFirst, CONSTRUCTOR_Impl_Invoke_Args_BasesFirst)(ClassName, HasMembers, Members, HasBases, Bases, BasesNumArgs)

#define CONSTRUCTOR_Impl_Invoke_Args_MembersFirst(ClassName, HasMembers, Members, HasBases, Bases, BasesNumArgs)    \
    BOOST_PP_IIF(HasMembers, CONSTRUCTOR_Impl_Invoke_Args_Members, BOOST_VMD_EMPTY)(Members)                        \
    BOOST_PP_COMMA_IF(BOOST_PP_AND(HasMembers, HasBases))                                                           \
    BOOST_PP_IIF(HasBases, CONSTRUCTOR_Impl_Invoke_Args_Bases, BOOST_VMD_EMPTY)(Bases, BasesNumArgs)

#define CONSTRUCTOR_Impl_Invoke_Args_BasesFirst(ClassName, HasMembers, Members, HasBases, Bases, BasesNumArgs)  \
    BOOST_PP_IIF(HasBases, CONSTRUCTOR_Impl_Invoke_Args_Bases, BOOST_VMD_EMPTY)(Bases, BasesNumArgs)            \
    BOOST_PP_COMMA_IF(BOOST_PP_AND(HasMembers, HasBases))                                                       \
    BOOST_PP_IIF(HasMembers, CONSTRUCTOR_Impl_Invoke_Args_Members, BOOST_VMD_EMPTY)(Members)

#define CONSTRUCTOR_Impl_Invoke_Args_Members(Members)                       BOOST_PP_TUPLE_FOR_EACH_ENUM(CONSTRUCTOR_Impl_Invoke_Args_Members_Macro, _, Members)
#define CONSTRUCTOR_Impl_Invoke_Args_Members_Macro(r, _, Member)            BOOST_PP_CAT(Member, T) && BOOST_PP_CAT(Member, Param)

#define CONSTRUCTOR_Impl_Invoke_Args_Bases(Bases, BasesNumArgs)                     CONSTRUCTOR_Impl_Invoke_EnumBaseInfo(CONSTRUCTOR_Impl_Invoke_Args_Bases_Macro, Bases, BasesNumArgs)
#define CONSTRUCTOR_Impl_Invoke_Args_Bases_Macro(BaseIndex, BaseName, ArgIndex)     CONSTRUCTOR_Impl_Invoke_BaseTypeName(BaseIndex, ArgIndex) && CONSTRUCTOR_Impl_Invoke_BaseArgName(BaseIndex, ArgIndex)

#define CONSTRUCTOR_Impl_Invoke_Init_Members(Members)                       BOOST_PP_TUPLE_FOR_EACH_ENUM(CONSTRUCTOR_Impl_Invoke_Init_Members_Macro, _, Members)
#define CONSTRUCTOR_Impl_Invoke_Init_Members_Macro(r, _, Member)            Member(std::forward<BOOST_PP_CAT(Member, T)>(BOOST_PP_CAT(Member, Param)))

#define CONSTRUCTOR_Impl_Invoke_Init_Bases(Bases, BasesNumArgs)                     BOOST_PP_TUPLE_FOR_EACH_ENUM_I(CONSTRUCTOR_Impl_Invoke_Init_Bases_Macro, BasesNumArgs, Bases)
#define CONSTRUCTOR_Impl_Invoke_Init_Bases_Macro(r, BasesNumArgs, Index, Base)      CONSTRUCTOR_Impl_Invoke_Init_Bases_Macro2(r, BOOST_PP_TUPLE_ELEM(Index, BasesNumArgs), Index, Base)
#define CONSTRUCTOR_Impl_Invoke_Init_Bases_Macro2(r, NumArgs, Index, Base)          Base( BOOST_PP_ENUM(NumArgs, CONSTRUCTOR_Impl_Invoke_Init_Bases_Macro2_Macro, Index) )
#define CONSTRUCTOR_Impl_Invoke_Init_Bases_Macro2_Macro(r, ArgIndex, BaseIndex)     std::forward<CONSTRUCTOR_Impl_Invoke_BaseTypeName(BaseIndex, ArgIndex)>(CONSTRUCTOR_Impl_Invoke_BaseArgName(BaseIndex, ArgIndex))

#define CONSTRUCTOR_Impl_Invoke_BaseTypeName(BaseIndex, ArgIndex)           BOOST_PP_CAT(Base, BOOST_PP_CAT(BaseIndex, BOOST_PP_CAT(_, BOOST_PP_CAT(T, ArgIndex))))
#define CONSTRUCTOR_Impl_Invoke_BaseArgName(BaseIndex, ArgIndex)            BOOST_PP_CAT(base, BOOST_PP_CAT(BaseIndex, BOOST_PP_CAT(_, BOOST_PP_CAT(Param, ArgIndex))))

// Macro(BaseIndex, BaseName, ArgIndex)
#define CONSTRUCTOR_Impl_Invoke_EnumBaseInfo(Macro, Bases, BasesNumArgs)                            BOOST_PP_TUPLE_FOR_EACH_ENUM_I(CONSTRUCTOR_Impl_Invoke_EnumBaseInfo_Macro, (Macro, BasesNumArgs), Bases)
#define CONSTRUCTOR_Impl_Invoke_EnumBaseInfo_Macro(r, Data, Index, Base)                            CONSTRUCTOR_Impl_Invoke_EnumBaseInfo_Macro2(r, BOOST_PP_TUPLE_ELEM(0, Data), BOOST_PP_TUPLE_ELEM(1, Data), Index, Base)
#define CONSTRUCTOR_Impl_Invoke_EnumBaseInfo_Macro2(r, Macro, BasesNumArgs, Index, Base)            BOOST_PP_ENUM(BOOST_PP_TUPLE_ELEM(Index, BasesNumArgs), CONSTRUCTOR_Impl_Invoke_EnumBaseInfo_Macro2_Macro, (Macro, Index, Base))
#define CONSTRUCTOR_Impl_Invoke_EnumBaseInfo_Macro2_Macro(z, Index, Data)                           CONSTRUCTOR_Impl_Invoke_EnumBaseInfo_Macro2_Macro2(z, Index, BOOST_PP_TUPLE_ELEM(0, Data), BOOST_PP_TUPLE_ELEM(1, Data), BOOST_PP_TUPLE_ELEM(2, Data))
#define CONSTRUCTOR_Impl_Invoke_EnumBaseInfo_Macro2_Macro2(z, ArgIndex, Macro, BaseIndex, Base)     Macro(BaseIndex, Base, ArgIndex)

// clang-format on

}  // namespace CommonHelpers
