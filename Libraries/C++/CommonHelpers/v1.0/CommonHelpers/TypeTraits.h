/////////////////////////////////////////////////////////////////////////
///
///  \file          TypeTraits.h
///  \brief         Contains the TypeTraits namespace
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2019-03-17 19:10:14
///
///  \note
///
///  \bug
///
/////////////////////////////////////////////////////////////////////////
///
///  \attention
///  Copyright David Brownell 2019-20
///  Distributed under the Boost Software License, Version 1.0. See
///  accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt.
///
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <memory>
#include <functional>
#include <string>
#include <type_traits>
#include <vector>
#include <utility>

#include "Details/TypeTraits.Details.h"

namespace CommonHelpers {

/////////////////////////////////////////////////////////////////////////
///  \namespace     TypeTraits
///  \brief         Types and methods that help when working with basic type properties.
///
namespace TypeTraits {

// ----------------------------------------------------------------------
// |
// |  Preprocessor Definitions
// |
// ----------------------------------------------------------------------

// clang-format off

/////////////////////////////////////////////////////////////////////////
///  \def           CREATE_HAS_TYPE_CHECKER
///  \brief         Creates a type that can be used to check if an object
///                 contains a specific nested type.
///
///                 Usage                                                               | Output
///                 -----                                                               | ------
///                 CREATE_HAS_TYPE_CHECKER(Foo);                                       | has_Foo<T>        => bool
///                 CREATE_HAS_TYPE_CHECKER(Foo, HAS_TYPE_CHECKER_NAME(DifferentName))  | DifferentName<T>  => bool
///
#define CREATE_HAS_TYPE_CHECKER(Typename, ...)          CREATE_HAS_TYPE_CHECKER_Impl(Typename, __VA_ARGS__)

// Context values that can be used with CREATE_HAS_TYPE_CHECKER
#define HAS_TYPE_CHECKER_NAME(Name)                     (0, Name)           ///< Name to use for the created checker; "has_<Typename>" is used if no custom name is provided.
#define HAS_TYPE_CHECKER_AS_STATIC_BOOL                 1                   ///< Define the expression as a static bool rather than constexpr; use this when invoking the macro from within a class definition.

#define __HAS_TYPE_CHECKER_NUM_CONTEXT_VALUES           2

/////////////////////////////////////////////////////////////////////////
///  \def           CREATE_HAS_METHOD_CHECKER
///  \brief         Creates a type that can be used to check if an object contains
///                 a method that matches a provided signature. Note that there
///                 are a number of different options that can be provided to this
///                 macro, each of which control how method detection and evaluation
///                 is controlled.
///
///                 When in strict mode, an exact function signatures must be
///                 provided. Strict mode ignores class hierarchy when looking for
///                 methods and only considers the current class.
///
///                     Comparison operators:   (bool (Q::*)(Q const &) const)
///                     Clone:                  (std::unique_ptr<Q> (Q::*)(void) const)
///
///                 When not in strict mode, simplified function signatures must be
///                 provided. This mode will search for the method in the entire
///                 class hierarchy.
///
///                     Comparison operators:   (bool (Q const &))              // static/const are specified via Context macro flags
///                     Clone:                  (std::unique_ptr<Q> (void))     // static/const are specified via Context macro flags
///
///                 Example                                                                                                                                     | Output
///                 -------                                                                                                                                     | ------
///                 CREATE_HAS_METHOD_CHECKER(Method1, (void (int, char)));                                                                                     | has_Method1<T>       => bool
///                 CREATE_HAS_METHOD_CHECKER(Method2, (string (Q::*)(void) const), HAS_METHOD_CHECKER_STRICT)                                                  | has_Method2<T>       => bool
///                 CREATE_HAS_METHOD_CHECKER(operator >, (bool (Q::*)(Q const &) const), HAS_METHOD_CHECKER_STRICT, HAS_METHOD_CHECKER_NAME(has_GreaterThan))  | has_GreaterThan<T>   => bool
///
#define CREATE_HAS_METHOD_CHECKER(MethodName, Signature, ...)               CREATE_HAS_METHOD_CHECKER_Impl(MethodName, Signature, __VA_ARGS__)

// Context values that can be used with CREATE_HAS_METHOD_CHECKER
#define HAS_METHOD_CHECKER_NAME(Name)                   (0, Name)           ///< Name to use for the created checker; "has_<MethodName>" is used if no custom name is provided.
#define HAS_METHOD_CHECKER_AS_STATIC_BOOL               1                   ///< Define the expression as a static bool rather than constexpr; use this when invoking the macro from within a class definition.
#define HAS_METHOD_CHECKER_TEMPLATE_TYPENAME(Typename)  (2, Typename)       ///< Use a new template name to avoid conflict with ancestor typenames; "Q" is used if not custom typename is provided.
#define HAS_METHOD_CHECKER_CONST                        3                   ///< The method is const, but can be defined by any class within the hierarchy.
#define HAS_METHOD_CHECKER_STATIC                       4                   ///< The method is static, but can be defined by any class within the hierarchy.
#define HAS_METHOD_CHECKER_STRICT                       5                   ///< The method signature must match exactly (both in terms of const-ness, static-ness, and the class that defines the method).

#define __HAS_METHOD_CHECKER_NUM_CONTEXT_VALUES         6

/////////////////////////////////////////////////////////////////////////
///  \def           make_mutable
///  \brief         Makes the target mutable.
///
///                 Example                         | Result
///                 -------                         | ------
///                 make_mutable(int const &)       | int &
///                 make_mutable(int const)         | int &
///                 make_mutable(int const *)       | int *&
///                 make_mutable(int)               | int &
///
#define make_mutable(Value)                 const_cast<std::add_lvalue_reference_t<CommonHelpers::TypeTraits::MakeTargetMutable<decltype(Value)>>>(Value)

/////////////////////////////////////////////////////////////////////////
///  \def           make_immutable
///  \brief         Makes the target immutable.
///
///                 Example                         | Result
///                 -------                         | ------
///                 make_immutable(int &)           | int const &
///                 make_immutable(int)             | int const &
///                 make_immutable(int*)            | int const *&
///                 make_immutable(int const)       | int const &
///
#define make_immutable(Value)               const_cast<std::add_lvalue_reference_t<CommonHelpers::TypeTraits::MakeTargetImmutable<decltype(Value)>>>(Value)

/////////////////////////////////////////////////////////////////////////
///  \def           UNUSED
///  \brief         Macro to avoid unused parameter style warnings.
///
#define UNUSED(X)                           (void)(X)

// clang-format on

// ----------------------------------------------------------------------
// |
// |  Public Types
// |
// ----------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////
///  \var           IsSmartPointer
///  \brief         True if the type is a smart pointer.
///
template <typename T>
constexpr bool const IsSmartPointer = false;

template <typename T>
constexpr bool const IsSmartPointer<std::unique_ptr<T>> = true;

template <typename T>
constexpr bool const IsSmartPointer<std::shared_ptr<T>> = true;

/////////////////////////////////////////////////////////////////////////
///  \var           IsContainer
///  \brief         True if the type is a container.
///
template <typename T>
constexpr bool const IsContainer = Details::IsContainerImpl<T>;

/////////////////////////////////////////////////////////////////////////
///  \var           IsVector
///  \brief         True if the type is a vector.
///
template <typename T>
constexpr bool const IsVector = false;

template <typename T, typename A>
constexpr bool const IsVector<std::vector<T, A>> = true;

/////////////////////////////////////////////////////////////////////////
///  \var           IsString
///  \brief         True if the type is a string.
///
template <typename T>
constexpr bool const IsString = false;

template <typename C, typename T, typename A>
constexpr bool const IsString<std::basic_string<C, T, A>> = true;

/////////////////////////////////////////////////////////////////////////
///  \var           IsContiguousContainer
///  \brief         True if the type is implemented in terms of contiguous
///                 memory.
///
template <typename T>
constexpr bool const IsContiguousContainer = IsVector<T> || IsString<T>;

/////////////////////////////////////////////////////////////////////////
///  \var           IsContiguousByteContainer
///  \brief         True if the type is implemented in terms of contiguous
///                 bytes.
///
template <typename T>
constexpr bool const IsContiguousByteContainer = IsContiguousContainer<T> && sizeof(typename T::value_type) == 1;

/////////////////////////////////////////////////////////////////////////
///  \var           IsIterator
///  \brief         True if the type is a container iterator.
///
template <typename T>
constexpr bool const IsIterator = Details::IsIteratorImpl<T>;

/////////////////////////////////////////////////////////////////////////
///  \var           IsIteratorOfType
///  \brief         True if the type of a container iterator with elements
///                 of type V.
///
template <typename T, typename V>
constexpr bool const IsIteratorOfType = IsIterator<T> &&std::is_same_v<typename T::value_type, V>;

/////////////////////////////////////////////////////////////////////////
///  \var           IsTargetImmutable
///  \brief         True if the target (type removed of pointers and references)
///                 is immutable (const).
///
template <typename T>
constexpr bool const IsTargetImmutable
    = (std::is_pointer_v<T>
        ? std::is_const_v<std::remove_pointer_t<T>>
        : (std::is_reference_v<T> ? std::is_const_v<std::remove_reference_t<T>> : std::is_const_v<T>));

/////////////////////////////////////////////////////////////////////////
///  \var           IsTargetMutable
///  \brief         True if the target (type removed of pointers and references)
///                 is mutable (non-const).
///
template <typename T>
constexpr bool const IsTargetMutable = IsTargetImmutable<T> == false;

/////////////////////////////////////////////////////////////////////////
///  \typedef       MakeTargetImmutable
///  \brief         Makes the target (type removed of pointers and references)
///                 immutable, then restores any pointers and references.
///
template <typename T>
using MakeTargetImmutable = std::conditional_t<
    std::is_pointer_v<T>,
    std::add_pointer_t<std::add_const_t<std::remove_pointer_t<T>>>,
    std::conditional_t<
        std::is_reference_v<T>,
        std::add_lvalue_reference_t<std::add_const_t<std::remove_reference_t<T>>>,
        std::add_const_t<T>
    >
>;

/////////////////////////////////////////////////////////////////////////
///  \typedef       MakeTargetMutable
///  \brief         Makes the target (type removed of pointers and references)
///                 mutable, then restores any pointers and references.
///
template <typename T>
using MakeTargetMutable = std::conditional_t<
    std::is_pointer_v<T>,
    std::add_pointer_t<std::remove_const_t<std::remove_pointer_t<T>>>,
    std::conditional_t<
        std::is_reference_v<T>,
        std::add_lvalue_reference_t<std::remove_const_t<std::remove_reference_t<T>>>,
        std::remove_const_t<T>
    >
>;

/////////////////////////////////////////////////////////////////////////
///  \class         Access
///  \brief         Object that is used to invoke custom functionality implemented
///                 by a class the leverages preprocessor-generated code. In most
///                 cases, this construct functionality can be considered private.
///                 As such, friendship can be granted to this class to ensure
///                 that the methods cannot be invoked by anyone else.
///
///                 Example:
///                     class MyClass {
///                     private:
///                         friend class CommonHelpers::TypeTraits::Access;
///
///                         void FinalConstruct(void) { ... }
///                         void CopyFinalConstruct(void) { ... }
///                         ...
///                     };
///
class Access {
public:
    // ----------------------------------------------------------------------
    // |  Public Methods
    template <typename T>
    static void FinalConstruct(T &obj);

    template <typename T>
    static void CopyFinalConstruct(T &obj);

    template <typename T>
    static void MoveFinalConstruct(T &obj);

    template <typename T>
    static void DeserializeFinalConstruct(T &obj);

    template <typename T>
    static void FinalAssign(T &obj);

    template <typename T>
    static void CopyFinalAssign(T &obj);

    template <typename T>
    static void MoveFinalAssign(T &obj);

private:
    // ----------------------------------------------------------------------
    // |  Private Types
    CREATE_HAS_METHOD_CHECKER(FinalConstruct, (void(void)), HAS_METHOD_CHECKER_AS_STATIC_BOOL);
    CREATE_HAS_METHOD_CHECKER(CopyFinalConstruct, (void(void)), HAS_METHOD_CHECKER_AS_STATIC_BOOL);
    CREATE_HAS_METHOD_CHECKER(MoveFinalConstruct, (void(void)), HAS_METHOD_CHECKER_AS_STATIC_BOOL);
    CREATE_HAS_METHOD_CHECKER(DeserializeFinalConstruct, (void(void)), HAS_METHOD_CHECKER_AS_STATIC_BOOL);
    CREATE_HAS_METHOD_CHECKER(FinalAssign, (void(void)), HAS_METHOD_CHECKER_AS_STATIC_BOOL);
    CREATE_HAS_METHOD_CHECKER(CopyFinalAssign, (void(void)), HAS_METHOD_CHECKER_AS_STATIC_BOOL);
    CREATE_HAS_METHOD_CHECKER(MoveFinalAssign, (void(void)), HAS_METHOD_CHECKER_AS_STATIC_BOOL);

    // ----------------------------------------------------------------------
    // |  Private Methods
    template <typename T>
    static void FinalConstructImpl(T &obj, std::true_type);

    template <typename T>
    static void FinalConstructImpl(T &, std::false_type);

    template <typename T>
    static void CopyFinalConstructImpl(T &obj, std::true_type);

    template <typename T>
    static void CopyFinalConstructImpl(T &, std::false_type);

    template <typename T>
    static void MoveFinalConstructImpl(T &obj, std::true_type);

    template <typename T>
    static void MoveFinalConstructImpl(T &, std::false_type);

    template <typename T>
    static void DeserializeFinalConstructImpl(T &obj, std::true_type);

    template <typename T>
    static void DeserializeFinalConstructImpl(T &, std::false_type);

    template <typename T>
    static void FinalAssignImpl(T &obj, std::true_type);

    template <typename T>
    static void FinalAssignImpl(T &, std::false_type);

    template <typename T>
    static void CopyFinalAssignImpl(T &obj, std::true_type);

    template <typename T>
    static void CopyFinalAssignImpl(T &, std::false_type);

    template <typename T>
    static void MoveFinalAssignImpl(T &obj, std::true_type);

    template <typename T>
    static void MoveFinalAssignImpl(T &, std::false_type);
};

/////////////////////////////////////////////////////////////////////////
///  \class         FunctionTraits
///  \brief         Extracts information about different callable types
///                 (functions, methods, std::function, lambda, etc).
///
template <typename T>
struct FunctionTraits : public Details::FunctionTraitsImpl<T, Details::FunctionTraitsHasCallOperator<T>>
{
    // Defines the following types:
    //      return_type
    //      args (as a tuple)
    //
    // Defines the following values:
    //      static bool const is_method;
    //      static bool const is_const;
};

template <typename ReturnT, typename... ArgTs>
struct FunctionTraits<std::function<ReturnT (ArgTs...)>> {
    using return_type                       = ReturnT;
    using args                              = std::tuple<ArgTs...>;
    static bool const is_method             = false;
    static bool const is_const              = false;
};

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// |
// |  Implementation
// |
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// |
// |  Access
// |
// ----------------------------------------------------------------------
template <typename T>
// static
void Access::FinalConstruct(T &obj) {
    FinalConstructImpl(obj, std::integral_constant<bool, has_FinalConstruct<T>::value>());
}

template <typename T>
// static
void Access::CopyFinalConstruct(T &obj) {
    CopyFinalConstructImpl(obj, std::integral_constant<bool, has_CopyFinalConstruct<T>::value>());
}

template <typename T>
// static
void Access::MoveFinalConstruct(T &obj) {
    MoveFinalConstructImpl(obj, std::integral_constant<bool, has_MoveFinalConstruct<T>::value>());
}

template <typename T>
// static
void Access::DeserializeFinalConstruct(T &obj) {
    DeserializeFinalConstructImpl(obj, std::integral_constant<bool, has_DeserializeFinalConstruct<T>::value>());
}

template <typename T>
// static
void Access::FinalAssign(T &obj) {
    FinalAssignImpl(obj, std::integral_constant<bool, has_FinalAssign<T>::value>());
}

template <typename T>
// static
void Access::CopyFinalAssign(T &obj) {
    CopyFinalAssignImpl(obj, std::integral_constant<bool, has_CopyFinalAssign<T>::value>());
}

template <typename T>
// static
void Access::MoveFinalAssign(T &obj) {
    MoveFinalAssignImpl(obj, std::integral_constant<bool, has_MoveFinalAssign<T>::value>());
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
template <typename T>
// static
void Access::FinalConstructImpl(T &obj, std::true_type) {
    obj.FinalConstruct();
}

template <typename T>
// static
void Access::FinalConstructImpl(T &, std::false_type) {
}

template <typename T>
// static
void Access::CopyFinalConstructImpl(T &obj, std::true_type) {
    obj.CopyFinalConstruct();
}

template <typename T>
// static
void Access::CopyFinalConstructImpl(T &, std::false_type) {
}

template <typename T>
// static
void Access::MoveFinalConstructImpl(T &obj, std::true_type) {
    obj.MoveFinalConstruct();
}

template <typename T>
// static
void Access::MoveFinalConstructImpl(T &, std::false_type) {
}

template <typename T>
// static
void Access::DeserializeFinalConstructImpl(T &obj, std::true_type) {
    obj.DeserializeFinalConstruct();
}

template <typename T>
// static
void Access::DeserializeFinalConstructImpl(T &, std::false_type) {
}

template <typename T>
// static
void Access::FinalAssignImpl(T &obj, std::true_type) {
    obj.FinalAssign();
}

template <typename T>
// static
void Access::FinalAssignImpl(T &, std::false_type) {
}

template <typename T>
// static
void Access::CopyFinalAssignImpl(T &obj, std::true_type) {
    obj.CopyFinalAssign();
}

template <typename T>
// static
void Access::CopyFinalAssignImpl(T &, std::false_type) {
}

template <typename T>
// static
void Access::MoveFinalAssignImpl(T &obj, std::true_type) {
    obj.MoveFinalAssign();
}

template <typename T>
// static
void Access::MoveFinalAssignImpl(T &, std::false_type) {
}

}  // namespace TypeTraits
}  // namespace CommonHelpers
