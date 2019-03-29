/////////////////////////////////////////////////////////////////////////
///
///  \file          Copy.h
///  \brief         Contains copy-related macros
///
///  \author        David Brownell <david.brownell@microsoft.com>
///  \date          2019-03-16 22:54:17
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
///  \def           COPY
///  \brief         Implements the copy constructor and copy assignment operator.
///
///                     MyClass(MyClass const &other);
///                     MyClass & operator =(MyClass const &other);
///
///                 Example                                                     | Comment
///                 -------                                                     | -------
///                 COPY(MyClass)                                               | No members or bases
///                 COPY(MyClass, a, b, c)                                      | All members
///                 COPY(MyClass, MEMBERS(a, b, c))                             | Explicit members
///                 COPY(MyClass, MEMBERS(a, b), BASES(c, d))                   | Explicit members and bases
///                 COPY(MyClass, BASES(c, d))                                  | Explicit bases
///                 COPY(MyClass, BASES(c, d), MEMBERS(a, b))                   | Explicit members and bases (alternate order)
///                 COPY(MyClass, MEMBERS(a, b), FLAGS(COPY_NO_CONSTRUCTOR))    | Creates a copy assignment by not a copy constructor
///
#define COPY(ClassName, ...)                PPOFImpl(COPY_Impl, (ClassName), __VA_ARGS__)

// Flags
#define COPY_NO_CONSTRUCTOR                 0           ///< Used with COPY to prevent the generation of a copy constructor
#define COPY_NO_ASSIGNMENT                  1           ///< Used with COPY to prevent the generation of a copy assignment operator

#define __NUM_COPY_FLAGS                    2

/////////////////////////////////////////////////////////////////////////
///  \def           NON_COPYABLE
///  \brief         Deletes the copy constructor and copy assignment operator
///                 for the class.
///
#define NON_COPYABLE(ClassName)             NON_COPYABLE_Impl(ClassName)

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// |
// |  Implementation
// |
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

#define COPY_Impl(ClassNameTuple, Members, Bases, Methods, Flags)                                                                                         \
    static_assert(BOOST_PP_IIF(BOOST_VMD_IS_EMPTY(Methods BOOST_PP_EMPTY()), true, false), "This macro does not support methods");                        \
    COPY_Impl_Delay(COPY_Impl2) (BOOST_PP_TUPLE_ELEM(0, ClassNameTuple), Members, Bases, CONTEXT_TUPLE_TO_INITIALIZED_TUPLE(__NUM_COPY_FLAGS, 0, Flags))

#define COPY_Impl_Delay(x)                  BOOST_PP_CAT(x, COPY_Impl_Empty())
#define COPY_Impl_Empty()

#define COPY_Impl2(ClassName, Members, Bases, Flags)    COPY_Impl2_Delay(COPY_Impl3) BOOST_PP_LPAREN() ClassName, BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(Members BOOST_PP_EMPTY())), Members, BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(Bases BOOST_PP_EMPTY())), Bases, BOOST_PP_TUPLE_ENUM(Flags) BOOST_PP_RPAREN()

#define COPY_Impl2_Delay(x)                 BOOST_PP_CAT(x, COPY_Impl2_Empty())
#define COPY_Impl2_Empty()

#define COPY_Impl3(ClassName, HasMembers, Members, HasBases, Bases, NoCtor, NoAssignment)                               \
    BOOST_PP_IIF(NoCtor, BOOST_VMD_EMPTY, COPY_Impl_Ctor)(ClassName, HasMembers, Members, HasBases, Bases)              \
    BOOST_PP_IIF(NoAssignment, BOOST_VMD_EMPTY, COPY_Impl_Assignment)(ClassName, HasMembers, Members, HasBases, Bases)

// ----------------------------------------------------------------------
#define COPY_Impl_Ctor(ClassName, HasMembers, Members, HasBases, Bases)                          \
    ClassName(ClassName const &other)                                                            \
        BOOST_PP_IIF(BOOST_PP_OR(HasMembers, HasBases), BOOST_PP_IDENTITY(:), BOOST_PP_EMPTY)()  \
        BOOST_PP_IIF(HasBases, COPY_Impl_Ctor_Bases, BOOST_VMD_EMPTY)(Bases)                     \
        BOOST_PP_COMMA_IF(BOOST_PP_AND(HasMembers, HasBases))                                    \
        BOOST_PP_IIF(HasMembers, COPY_Impl_Ctor_Members, BOOST_VMD_EMPTY)(Members)               \
        {                                                                                        \
            (other);                                                                             \
            CommonHelpers::TypeTraits::Access::CopyFinalConstruct(*this);                        \
            CommonHelpers::TypeTraits::Access::FinalConstruct(*this);                            \
        }

#define COPY_Impl_Ctor_Bases(Bases)                     BOOST_PP_TUPLE_FOR_EACH_ENUM(COPY_Impl_Ctor_Bases_Macro, _, Bases)
#define COPY_Impl_Ctor_Bases_Macro(r, _, Base)          Base(static_cast<Base const &>(other))

#define COPY_Impl_Ctor_Members(Members)                 BOOST_PP_TUPLE_FOR_EACH_ENUM(COPY_Impl_Ctor_Members_Macro, _, Members)
#define COPY_Impl_Ctor_Members_Macro(r, _, Member)      Member(other.Member)

// ----------------------------------------------------------------------
#define COPY_Impl_Assignment(ClassName, HasMembers, Members, HasBases, Bases)             \
    ClassName & operator =(ClassName const &other) {                                      \
        (other);                                                                          \
        BOOST_PP_IIF(HasBases, COPY_Impl_Assignment_Bases, BOOST_VMD_EMPTY)(Bases)        \
        BOOST_PP_IIF(HasMembers, COPY_Impl_Assignment_Members, BOOST_VMD_EMPTY)(Members)  \
                                                                                          \
        CommonHelpers::TypeTraits::Access::CopyFinalAssign(*this);                        \
        CommonHelpers::TypeTraits::Access::FinalAssign(*this);                            \
                                                                                          \
        return *this;                                                                     \
    }

#define COPY_Impl_Assignment_Bases(Bases)              BOOST_PP_TUPLE_FOR_EACH(COPY_Impl_Assignment_Bases_Macro, _, Bases)
#define COPY_Impl_Assignment_Bases_Macro(r, _, Base)   Base::operator =(static_cast<Base const &>(other));

#define COPY_Impl_Assignment_Members(Members)                               BOOST_PP_TUPLE_FOR_EACH(COPY_Impl_Assignment_Members_Macro, _, Members)
#define COPY_Impl_Assignment_Members_Macro(r, _, Member)                    make_mutable(Member) = other.Member;

// ----------------------------------------------------------------------
#define NON_COPYABLE_Impl(ClassName)                     \
    ClassName(ClassName const &) = delete;               \
    ClassName & operator =(ClassName const &) = delete;

// clang-format on

}  // namespace CommonHelpers
