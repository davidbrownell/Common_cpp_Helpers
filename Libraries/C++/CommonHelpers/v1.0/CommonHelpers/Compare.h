/////////////////////////////////////////////////////////////////////////
///
///  \file          Compare.h
///  \brief         Contains macros and functions that help in implementation
///                 of functionality that compares two objects.
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2019-03-27 08:31:19
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

#include <memory>
#include <tuple>

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
///  \def           COMPARE
///  \brief         Implements a static Compare method and all comparison operators.
///
///                 Creates the following methods:
///
///                     static int Compare(MyClass const &a, MyClass const &b);
///                     bool operator ==(MyClass const &other) const;
///                     bool operator !=(MyClass const &other) const;
///                     bool operator  <(MyClass const &other) const;
///                     bool operator <=(MyClass const &other) const;
///                     bool operator  >(MyClass const &other) const;
///                     bool operator >=(MyClass const &other) const;
///
///                 Example                                             | Comment
///                 -------                                             | -------
///                 COMPARE(MyClass);                                   | No members or bases
///                 COMPARE(MyClass, a, b, c);                          | All members
///                 COMPARE(MyClass, MEMBERS(a, b, c));                 | Explicit members
///                 COMPARE(MyClass, MEMBERS(a, b), BASES(c, d));       | Members and bases
///                 COMPARE(MyClass, BASES(Base1, Base2));              | Bases only
///                 COMPARE(MyClass, BASES(Base1), MEMBERS(a, b, c));   | Members and bases (alternate order)
///
#define COMPARE(ClassName, ...)             PPOFImpl(COMPARE_Impl, (ClassName), __VA_ARGS__)

// Flags
#define COMPARE_BASES_BEFORE_MEMBERS        0           ///< By default, members are compared before bases. Specifying this flag ensures that bases are compared before members.
#define COMPARE_NO_OPERATORS                1           ///< Disables the generation of comparison operators.

#define __NUM_COMPARE_FLAGS                 2

// clang-format on

// ----------------------------------------------------------------------
// |
// |  Public Methods
// |
// ----------------------------------------------------------------------
template <typename... ArgsT>
int Compare(std::tuple<ArgsT...> const &a, std::tuple<ArgsT...> const &b);

template <typename T>
int Compare(T const &a, T const &b, std::enable_if_t<TypeTraits::IsSmartPointer<T>> * =nullptr);

template <typename T>
int Compare(T const &a, T const &b, std::enable_if_t<TypeTraits::IsContainer<T>> * =nullptr);

template <typename T>
int Compare(T const &a, T const &b, std::enable_if_t<std::is_enum_v<T>> * =nullptr);

// In the following methods, the code will attempt to use the following signatures to
// implement comparison:
//
//      - int Compare(T const &, T const &)
//      - int (T::Compare)(T const &) const
//      - bool (T::operator <)(T const &) const
//      - bool operator <(T const &, T const &)
//
template <typename T>
int Compare(T const &a, T const &b, std::enable_if_t<TypeTraits::IsSmartPointer<T> == false && TypeTraits::IsContainer<T> == false && std::is_enum_v<T> == false && std::is_pointer_v<T> == false> * =nullptr);

// For pointers:
//
//      - valid pointer < nullptr
//      - nullptr == nullptr
//
int Compare(char const *a, char const *b);
int Compare(wchar_t const *a, wchar_t const *b);
int Compare(nullptr_t const, nullptr_t const);

template <typename T>
int Compare(T const *a, T const *b);

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// |
// |  Implementation
// |
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

// clang-format off

#define COMPARE_Impl(ClassNameTuple, Members, Bases, Methods, Flags)                                                                                                \
    static_assert(BOOST_PP_IIF(BOOST_VMD_IS_EMPTY(Methods BOOST_PP_EMPTY()), true, false), "This macro does not support methods");                                     \
    COMPARE_Impl_Delay(COMPARE_Impl2) (BOOST_PP_TUPLE_ELEM(0, ClassNameTuple), Members, Bases, CONTEXT_TUPLE_TO_INITIALIZED_TUPLE(__NUM_COMPARE_FLAGS, 0, Flags))

#define COMPARE_Impl_Delay(x)               BOOST_PP_CAT(x, COMPARE_Impl_Empty())
#define COMPARE_Impl_Empty()

#if (defined _MSC_VER and !defined __clang__)
    // MSVC doesn't like BOOST_PP_EXPAND
#   define COMPARE_Impl2(ClassName, Members, Bases, Flags)                  COMPARE_Impl3 BOOST_PP_LPAREN() ClassName BOOST_PP_COMMA() BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(Members BOOST_PP_EMPTY())) BOOST_PP_COMMA() Members BOOST_PP_COMMA() BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(Bases BOOST_PP_EMPTY())) BOOST_PP_COMMA() Bases BOOST_PP_COMMA() BOOST_PP_TUPLE_ENUM(Flags) BOOST_PP_RPAREN()
#else
#   define COMPARE_Impl2(ClassName, Members, Bases, Flags)                  BOOST_PP_EXPAND(COMPARE_Impl3 BOOST_PP_LPAREN() ClassName BOOST_PP_COMMA() BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(Members BOOST_PP_EMPTY())) BOOST_PP_COMMA() Members BOOST_PP_COMMA() BOOST_PP_NOT(BOOST_VMD_IS_EMPTY(Bases BOOST_PP_EMPTY())) BOOST_PP_COMMA() Bases BOOST_PP_COMMA() BOOST_PP_TUPLE_ENUM(Flags) BOOST_PP_RPAREN())
#endif

#define COMPARE_Impl3(ClassName, HasMembers, Members, HasBases, Bases, BasesBeforeMembers, NoOperators)                             \
    static int Compare(ClassName const &a, ClassName const &b) {                                                                    \
        if(&a == &b)                                                                                                                \
            return 0;                                                                                                               \
                                                                                                                                    \
        int                                 iResult(0);                                                                             \
                                                                                                                                    \
        BOOST_PP_IIF(BasesBeforeMembers, COMPARE_Impl_BasesFirst, COMPARE_Impl_MembersFirst)(HasMembers, Members, HasBases, Bases)  \
                                                                                                                                    \
        return iResult;                                                                                                             \
    }                                                                                                                               \
                                                                                                                                    \
    BOOST_PP_IIF(NoOperators, BOOST_VMD_EMPTY, COMPARE_Impl_Operators)(ClassName)

#define COMPARE_Impl_BasesFirst(HasMembers, Members, HasBases, Bases)           \
    BOOST_PP_IIF(HasBases, COMPARE_Impl_Bases, BOOST_VMD_EMPTY)(Bases)          \
    BOOST_PP_IIF(HasMembers, COMPARE_Impl_Members, BOOST_VMD_EMPTY)(Members)

#define COMPARE_Impl_MembersFirst(HasMembers, Members, HasBases, Bases)         \
    BOOST_PP_IIF(HasMembers, COMPARE_Impl_Members, BOOST_VMD_EMPTY)(Members)    \
    BOOST_PP_IIF(HasBases, COMPARE_Impl_Bases, BOOST_VMD_EMPTY)(Bases)

#define COMPARE_Impl_Bases(Bases)                       BOOST_PP_TUPLE_FOR_EACH(COMPARE_Impl_Bases_Macro, _, Bases)
#define COMPARE_Impl_Bases_Macro(r, _, Base)            iResult = CommonHelpers::Compare(static_cast<Base const &>(a), static_cast<Base const &>(b)); if(iResult != 0) return iResult;

#define COMPARE_Impl_Members(Members)                   BOOST_PP_TUPLE_FOR_EACH(COMPARE_Impl_Members_Macro, _, Members)
#define COMPARE_Impl_Members_Macro(r, _, Member)        iResult = CommonHelpers::Compare(a.Member, b.Member); if(iResult != 0) return iResult;

#define COMPARE_Impl_Operators(ClassName)                                                   \
    bool operator <(ClassName const &other) const { return Compare(*this, other) < 0; }     \
    bool operator <=(ClassName const &other) const { return Compare(*this, other) <= 0; }   \
    bool operator >(ClassName const &other) const { return Compare(*this, other) > 0; }     \
    bool operator >=(ClassName const &other) const { return Compare(*this, other) >= 0; }   \
    bool operator ==(ClassName const &other) const { return Compare(*this, other) == 0; }   \
    bool operator !=(ClassName const &other) const { return Compare(*this, other) != 0; }

// clang-format on

namespace Details {

CREATE_HAS_METHOD_CHECKER(Compare, (int (Q const &, Q const &)), HAS_METHOD_CHECKER_NAME(has_StaticCompare), HAS_METHOD_CHECKER_STRICT);
CREATE_HAS_METHOD_CHECKER(Compare, (int (Q::*)(Q const &) const), HAS_METHOD_CHECKER_NAME(has_Compare), HAS_METHOD_CHECKER_STRICT);

template <typename T>
struct CompareImpl_LessThanOperator {
    static int Execute(T const &a, T const &b) {
        return (a < b ? -1 : (b < a ? 1: 0));
    }
};

template <typename T, bool>
struct CompareImpl_Compare {
    static int Execute(T const &a, T const &b) {
        return a.Compare(b);
    }
};

template <typename T>
struct CompareImpl_Compare<T, false> : public CompareImpl_LessThanOperator<T> {
};

template <typename T, bool>
struct CompareImpl_StaticCompare {
    static int Execute(T const &a, T const &b) {
        return T::Compare(a, b);
    }
};

template <typename T>
struct CompareImpl_StaticCompare<T, false> : public CompareImpl_Compare<T, has_Compare<T>> {
};

template <typename T, bool>
struct CompareImpl_IsClass : public CompareImpl_StaticCompare<T, has_StaticCompare<T>> {
};

template <typename T>
struct CompareImpl_IsClass<T, false> : public CompareImpl_LessThanOperator<T> {
};

template <size_t IndexV, typename... ArgsT>
int CompareTupleImpl(std::tuple<ArgsT...> const &, std::tuple<ArgsT...>const &, std::enable_if_t<IndexV == sizeof...(ArgsT)> * =nullptr) {
    return 0;
}

template <size_t IndexV, typename... ArgsT>
int CompareTupleImpl(std::tuple<ArgsT...> const &a, std::tuple<ArgsT...> const &b, std::enable_if_t<IndexV < sizeof...(ArgsT)> * =nullptr) {
    int const                               iResult(Compare(std::get<IndexV>(a), std::get<IndexV>(b)));

    if(iResult != 0)
        return iResult;

    return CompareTupleImpl<IndexV + 1>(a, b);
}

template <typename T>
int ComparePtrImpl(T const *a, T const *b, int (*func)(T const *, T const *)) {
    if(a == nullptr && b == nullptr)
        return 0;

    // Valid pointers < nullptr
    if(b == nullptr)
        return -1;

    if(a == nullptr)
        return 1;

    return (*func)(a, b);
}

}  // namespace Details

template <typename... ArgsT>
int Compare(std::tuple<ArgsT...> const &a, std::tuple<ArgsT...> const &b) {
    return Details::CompareTupleImpl<0>(a, b);
}

template <typename T>
int Compare(T const &a, T const &b, std::enable_if_t<TypeTraits::IsSmartPointer<T>> *) {
    return Compare(a.get(), b.get());
}

template <typename T>
int Compare(T const &a, T const &b, std::enable_if_t<TypeTraits::IsContainer<T>> *) {
    typename T::const_iterator              iterA(a.begin());
    typename T::const_iterator const        endA(a.end());
    typename T::const_iterator              iterB(b.begin());
    typename T::const_iterator const        endB(b.end());

    while(iterA != endA && iterB != endB) {
        int const                           iResult(Compare(*iterA, *iterB));

        if(iResult != 0)
            return iResult;

        ++iterA;
        ++iterB;
    }

    if(iterA == endA && iterB == endB)
        return 0;

    return iterA == endA ? -1 : 1;
}

template <typename T>
int Compare(T const &a, T const &b, std::enable_if_t<std::is_enum_v<T>> *) {
    return Compare(static_cast<std::underlying_type_t<T>>(a), static_cast<std::underlying_type_t<T>>(b));
}

template <typename T>
int Compare(T const &a, T const &b, std::enable_if_t<TypeTraits::IsSmartPointer<T> == false && TypeTraits::IsContainer<T> == false && std::is_enum_v<T> == false && std::is_pointer_v<T> == false> *) {
    if(&a == &b)
        return 0;

    return Details::CompareImpl_IsClass<T, std::is_class_v<T>>::Execute(a, b);
}

inline int Compare(char const *a, char const *b) {
    return Details::ComparePtrImpl(a, b, strcmp);
}

inline int Compare(wchar_t const *a, wchar_t const *b) {
    return Details::ComparePtrImpl(a, b, wcscmp);
}

inline int Compare(nullptr_t const, nullptr_t const) {
    return 0;
}

template <typename T>
int Compare(T const *a, T const *b) {
    // ----------------------------------------------------------------------
    struct Internal {
        static int Execute(T const *a, T const *b) {
            return CommonHelpers::Compare(*a, *b);
        }
    };
    // ----------------------------------------------------------------------

    return Details::ComparePtrImpl(a, b, Internal::Execute);
}

}  // namespace CommonHelpers
