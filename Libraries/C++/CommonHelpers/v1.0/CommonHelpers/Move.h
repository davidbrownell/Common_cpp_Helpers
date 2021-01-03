/////////////////////////////////////////////////////////////////////////
///
///  \file          Move.h
///  \brief         Contains move-related macros
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2019-03-22 22:06:13
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
///  \def           MOVE
///  \brief         Implements the move constructor and move assignment operator.
///
///                 Creates the following methods:
///
///                     MyClass(MyClass && other);
///                     MyClass & operator =(MyClass && other);
///
///                 The following class methods will be called if they exist:
///
///                     Constructor:
///                         void MoveFinalConstruct(void)
///                         void FinalConstruct(void)
///
///                     Assignment operator:
///                         void MoveFinalAssign(void)
///                         void FinalAssign(void)
///
///                 Example                                                     | Comment
///                 -------                                                     | -------
///                 MOVE(MyClass)                                               | No members or bases
///                 MOVE(MyClass, a, b, c)                                      | All members
///                 MOVE(MyClass, MEMBERS(a, b, c))                             | Explicit members
///                 MOVE(MyClass, MEMBERS(a, b), BASES(c, d))                   | Explicit members and bases
///                 MOVE(MyClass, BASES(c, d))                                  | Explicit bases
///                 MOVE(MyClass, BASES(c, d), MEMBERS(a, b))                   | Explicit members and bases (alternate order)
///                 MOVE(MyClass, MEMBERS(a, b), FLAGS(MOVE_NO_CONSTRUCTOR))    | Creates a move assignment by not a move constructor
///
#define MOVE(ClassName, ...)                PPOFImpl(MOVE_Impl, (ClassName), __VA_ARGS__)

// Flags
#define MOVE_NO_CONSTRUCTOR                 0           ///< Used with MOVE to prevent the generation of a move constructor
#define MOVE_NO_ASSIGNMENT                  1           ///< Used with MOVE to prevent the generation of a move assignment operator

#define __NUM_MOVE_FLAGS                    2

/////////////////////////////////////////////////////////////////////////
///  \def           NON_MOVABLE
///  \brief         Deletes the move constructor and move assignment operator
///                 for the class.
///
#define NON_MOVABLE(ClassName)              NON_MOVABLE_Impl(ClassName)

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// |
// |  Implementation
// |
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

#define MOVE_Impl(ClassNameTuple, Members, Bases, Methods, Flags)                                                                                         \
    static_assert(BOOST_PP_IIF(BOOST_VMD_IS_EMPTY(Methods BOOST_PP_EMPTY()), true, false), "The macro does not support methods");                         \
    MOVE_Impl_Delay(MOVE_Impl2) (BOOST_PP_TUPLE_ELEM(0, ClassNameTuple), Members, Bases, CONTEXT_TUPLE_TO_INITIALIZED_TUPLE(__NUM_MOVE_FLAGS, 0, Flags))

#define MOVE_Impl_Delay(x)                  BOOST_PP_CAT(x, MOVE_Impl_Empty())
#define MOVE_Impl_Empty()

#if (defined _MSC_VER && !defined __clang__)
    // MSVC doesn't like BOOST_PP_EXPAND
#   define MOVE_Impl2(ClassName, Members, Bases, Flags)                     MOVE_Impl3 BOOST_PP_LPAREN() ClassName BOOST_PP_COMMA() BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(Members BOOST_PP_EMPTY())) BOOST_PP_COMMA() Members BOOST_PP_COMMA() BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(Bases BOOST_PP_EMPTY())) BOOST_PP_COMMA() Bases BOOST_PP_COMMA() BOOST_PP_TUPLE_ENUM(Flags) BOOST_PP_RPAREN()
#else
#   define MOVE_Impl2(ClassName, Members, Bases, Flags)                     BOOST_PP_EXPAND(MOVE_Impl3 BOOST_PP_LPAREN() ClassName BOOST_PP_COMMA() BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(Members BOOST_PP_EMPTY())) BOOST_PP_COMMA() Members BOOST_PP_COMMA() BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(Bases BOOST_PP_EMPTY())) BOOST_PP_COMMA() Bases BOOST_PP_COMMA() BOOST_PP_TUPLE_ENUM(Flags) BOOST_PP_RPAREN())
#endif

#define MOVE_Impl2_Delay(x)                 BOOST_PP_CAT(x, MOVE_Impl2_Empty())
#define MOVE_Impl2_Empty()

#define MOVE_Impl3(ClassName, HasMembers, Members, HasBases, Bases, NoCtor, NoAssignment)                               \
    BOOST_PP_IIF(NoCtor, BOOST_VMD_EMPTY, MOVE_Impl_Ctor)(ClassName, HasMembers, Members, HasBases, Bases)              \
    BOOST_PP_IIF(NoAssignment, BOOST_VMD_EMPTY, MOVE_Impl_Assignment)(ClassName, HasMembers, Members, HasBases, Bases)

// ----------------------------------------------------------------------
#define MOVE_Impl_Ctor(ClassName, HasMembers, Members, HasBases, Bases)                          \
    ClassName(ClassName && other)                                                                \
        BOOST_PP_IIF(BOOST_PP_OR(HasMembers, HasBases), BOOST_PP_IDENTITY(:), BOOST_PP_EMPTY)()  \
        BOOST_PP_IIF(HasBases, MOVE_Impl_Ctor_Bases, BOOST_VMD_EMPTY)(Bases)                     \
        BOOST_PP_COMMA_IF(BOOST_PP_AND(HasMembers, HasBases))                                    \
        BOOST_PP_IIF(HasMembers, MOVE_Impl_Ctor_Members, BOOST_VMD_EMPTY)(Members)               \
        {                                                                                        \
            UNUSED(other);                                                                       \
            CommonHelpers::TypeTraits::Access::MoveFinalConstruct(*this);                        \
            CommonHelpers::TypeTraits::Access::FinalConstruct(*this);                            \
        }

#define MOVE_Impl_Ctor_Bases(Bases)                     BOOST_PP_TUPLE_FOR_EACH_ENUM(MOVE_Impl_Ctor_Bases_Macro, _, Bases)
#define MOVE_Impl_Ctor_Bases_Macro(r, _, Base)          Base(std::move(static_cast<Base &>(other)))

#define MOVE_Impl_Ctor_Members(Members)                 BOOST_PP_TUPLE_FOR_EACH_ENUM(MOVE_Impl_Ctor_Members_Macro, _, Members)
#define MOVE_Impl_Ctor_Members_Macro(r, _, Member)      Member(CommonHelpers::Details::ConstructMover(make_mutable(other.Member), std::integral_constant<bool, std::is_reference_v<decltype(other.Member)>>()))

// ----------------------------------------------------------------------
#define MOVE_Impl_Assignment(ClassName, HasMembers, Members, HasBases, Bases)               \
    ClassName & operator =(ClassName && other) {                                            \
        UNUSED(other);                                                                      \
        BOOST_PP_IIF(HasBases, MOVE_Impl_Assignment_Bases, BOOST_VMD_EMPTY)(Bases)          \
        BOOST_PP_IIF(HasMembers, MOVE_Impl_Assignment_Members, BOOST_VMD_EMPTY)(Members)    \
                                                                                            \
        CommonHelpers::TypeTraits::Access::MoveFinalAssign(*this);                          \
        CommonHelpers::TypeTraits::Access::FinalAssign(*this);                              \
                                                                                            \
        return *this;                                                                       \
    }

#define MOVE_Impl_Assignment_Bases(Bases)               BOOST_PP_TUPLE_FOR_EACH(MOVE_Impl_Assignment_Bases_Macro, _, Bases)
#define MOVE_Impl_Assignment_Bases_Macro(r, _, Base)    Base::operator =(std::move(other));

#define MOVE_Impl_Assignment_Members(Members)                               BOOST_PP_TUPLE_FOR_EACH(MOVE_Impl_Assignment_Members_Macro, _, Members)
#define MOVE_Impl_Assignment_Members_Macro(r, _, Member)                    CommonHelpers::Details::AssignMover(make_mutable(Member), make_mutable(other.Member), std::integral_constant<bool, std::is_reference_v<decltype(Member)>>());

// ----------------------------------------------------------------------
#define NON_MOVABLE_Impl(ClassName)                       \
    ClassName(ClassName && other) = delete;               \
    ClassName & operator =(ClassName && other) = delete;

// clang-format on

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
namespace Details {

template <typename T>
T &ConstructMover(T &value, std::true_type) {
    return value;
}

template <typename T>
T &&ConstructMover(T &value, std::false_type) {
    return std::move(value);
}

template <typename T>
void AssignMover(T &, T &, std::true_type) {
    static_assert(
        // Use the template to ensure that the compiler doesn't eagerly evaluate the expression; the result
        // should always be false (meaning the assertion will fail) when this method is compiled.
        std::is_same_v<T, T> == false,
        "References are not supported in move assignments; consider applying the 'MOVE_NO_ASSIGNMENT' flag or removing the reference."
    );
}

template <typename T>
void AssignMover(T &lhs, T &rhs, std::false_type) {
    lhs = std::move(rhs);
}

}  // namespace Details
}  // namespace CommonHelpers
