/////////////////////////////////////////////////////////////////////////
///
///  \file          TypeTraits.Details.h
///  \brief         Contains the CommonHelpers::TypeTraits::Details namespace
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2019-03-17 19:14:46
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

#include "PreprocessorObjectFunctionalityImpl.h"

namespace CommonHelpers {
namespace TypeTraits {
namespace Details {

// ----------------------------------------------------------------------
// |
// |  Preprocessor Objects
// |
// ----------------------------------------------------------------------

// clang-format off

#define CREATE_HAS_STATIC_BOOL_DECLARATION(StructName, AsStaticBool)        BOOST_PP_IIF(AsStaticBool, CREATE_HAS_STATIC_BOOL_DECLARATION_Struct, CREATE_HAS_STATIC_BOOL_DECLARATION_Constexpr)(StructName)

#define CREATE_HAS_STATIC_BOOL_DECLARATION_Struct(StructName)                                        \
    template <typename HasStaticBoolDeclarationT>                                                    \
    struct StructName {                                                                              \
        static bool const value = BOOST_PP_CAT(StructName, Impl)<HasStaticBoolDeclarationT>::value;  \
    }

#define CREATE_HAS_STATIC_BOOL_DECLARATION_Constexpr(StructName)                                         \
    template <typename HasStaticBoolDeclarationT>                                                        \
    constexpr bool const StructName = BOOST_PP_CAT(StructName, Impl)<HasStaticBoolDeclarationT>::value;

// ----------------------------------------------------------------------
// |  CREATE_HAS_TYPE_CHECKER
#define CREATE_HAS_TYPE_CHECKER_Impl(Typename, ...)                         CREATE_HAS_TYPE_CHECKER_Impl2(Typename, CONTEXT_TO_INITIALIZED_TUPLE(__HAS_TYPE_CHECKER_NUM_CONTEXT_VALUES, 0, __VA_ARGS__))
#define CREATE_HAS_TYPE_CHECKER_Impl2(Typename, Args)                       CREATE_HAS_TYPE_CHECKER_Impl3 BOOST_PP_LPAREN() Typename BOOST_PP_COMMA() BOOST_PP_TUPLE_REM_CTOR(Args) BOOST_PP_RPAREN()
#define CREATE_HAS_TYPE_CHECKER_Impl3(Typename, OptStructName, AsStaticBool)    CREATE_HAS_TYPE_CHECKER_Impl4(Typename, BOOST_PP_IIF(BOOST_VMD_IS_NUMBER(OptStructName BOOST_PP_EMPTY()), BOOST_PP_IDENTITY(BOOST_PP_CAT(has_, Typename)), BOOST_PP_IDENTITY(OptStructName))(), AsStaticBool)

#define CREATE_HAS_TYPE_CHECKER_Impl4(Typename, StructName, AsStaticBool)                                                                                     \
    template <typename HasTypeCheckerImplArgumentT>                                                                                                           \
    struct BOOST_PP_CAT(StructName, Impl) {                                                                                                                   \
        template <typename HasTypeCheckerImplInternalArgumentT> static std::true_type Check(typename HasTypeCheckerImplInternalArgumentT::Typename const *);  \
        template <typename HasTypeCheckerImplInternalArgumentT> static std::false_type Check(...);                                                            \
                                                                                                                                                              \
        static bool const value = std::is_same<std::true_type, decltype(Check<HasTypeCheckerImplArgumentT>(nullptr))>::value;                                 \
    };                                                                                                                                                        \
    CREATE_HAS_STATIC_BOOL_DECLARATION(StructName, AsStaticBool);

// ----------------------------------------------------------------------
// |  CREATE_HAS_METHOD_CHECKER
#define CREATE_HAS_METHOD_CHECKER_Impl(MethodName, Signature, ...)                                                                              CREATE_HAS_METHOD_CHECKER_Impl2(MethodName, Signature, CONTEXT_TO_INITIALIZED_TUPLE(__HAS_METHOD_CHECKER_NUM_CONTEXT_VALUES, 0, __VA_ARGS__))
#define CREATE_HAS_METHOD_CHECKER_Impl2(MethodName, Signature, Args)                                                                            CREATE_HAS_METHOD_CHECKER_Impl3 BOOST_PP_LPAREN() MethodName BOOST_PP_COMMA() Signature BOOST_PP_COMMA() BOOST_PP_TUPLE_REM_CTOR(Args) BOOST_PP_RPAREN()
#define CREATE_HAS_METHOD_CHECKER_Impl3(MethodName, Signature, OptStructName, AsStaticBool, OptTemplateTypename, IsConst, IsStatic, IsStrict)   CREATE_HAS_METHOD_CHECKER_Impl4(MethodName, Signature, BOOST_PP_IIF(BOOST_VMD_IS_NUMBER(OptStructName), BOOST_PP_IDENTITY(BOOST_PP_CAT(has_, MethodName)), BOOST_PP_IDENTITY(OptStructName))(), AsStaticBool, BOOST_PP_IIF(BOOST_VMD_IS_NUMBER(OptTemplateTypename), BOOST_PP_IDENTITY(Q), BOOST_PP_IDENTITY(OptTemplateTypename))(), IsConst, IsStatic, IsStrict)
#define CREATE_HAS_METHOD_CHECKER_Impl4(MethodName, Signature, StructName, AsStaticBool, TemplateTypename, IsConst, IsStatic, IsStrict)         BOOST_PP_IIF(IsStrict, CREATE_HAS_METHOD_CHECKER_Impl_Strict, CREATE_HAS_METHOD_CHECKER_Impl_Standard)(MethodName, Signature, StructName, AsStaticBool, TemplateTypename, IsConst, IsStatic)

// This is pretty straight-forward, as we are only looking in this class.
#define CREATE_HAS_METHOD_CHECKER_Impl_Strict(MethodName, Signature, StructName, AsStaticBool, TemplateTypename, IsConst, IsStatic)                                                                         \
    static_assert(!IsConst, "The const flag should not be provided in strict mode, as the signature should include that information.");                                                                     \
    static_assert(!IsStatic, "The static flag should not be provided in strict mode, as the signature should include that information.");                                                                   \
                                                                                                                                                                                                            \
    template <typename HasMethodCheckerImplArgumentT>                                                                                                                                                       \
    struct BOOST_PP_CAT(StructName, Impl) {                                                                                                                                                                 \
        template <typename TemplateTypename, BOOST_PP_IDENTITY Signature ()> struct dummy;                                                                                                                  \
                                                                                                                                                                                                            \
        template <typename HasMethodCheckerImplInternalArgumentT> static std::true_type Check(dummy<HasMethodCheckerImplInternalArgumentT, &(HasMethodCheckerImplInternalArgumentT::MethodName)> const *);  \
        template <typename HasMethodCheckerImplInternalArgumentT> static std::false_type Check(...);                                                                                                        \
                                                                                                                                                                                                            \
        static bool const value = std::is_same<std::true_type, decltype(Check<HasMethodCheckerImplArgumentT>(nullptr))>::value;                                                                             \
    };                                                                                                                                                                                                      \
    CREATE_HAS_STATIC_BOOL_DECLARATION(StructName, AsStaticBool);

// This method is not, as we have to make sure that the method picked up can be found
// within any class in the class hierarchy.
#define CREATE_HAS_METHOD_CHECKER_Impl_Standard(MethodName, Signature, StructName, AsStaticBool, TemplateTypename, IsConst, IsStatic)                                                                                                                                                                                                       \
    template <typename HasMethodCheckerImplArgumentT>                                                                                                                                                                                                                                                                                       \
    class BOOST_PP_CAT(StructName, Impl) {                                                                                                                                                                                                                                                                                                  \
    private:                                                                                                                                                                                                                                                                                                                                \
        /* Determine if the method can be called with the provided signature (return value, args, static/const) */                                                                                                                                                                                                                          \
        template <typename TemplateTypename>                                                                                                                                                                                                                                                                                                \
        struct DeduceCanCall {                                                                                                                                                                                                                                                                                                              \
            /* Non-specialized template - this is never used directly, but is here to provide the default template required in partial template specializations. */                                                                                                                                                                         \
            /* Partial template specialization is used to extract the function result and arguments from the provided signature. */                                                                                                                                                                                                         \
            template <typename DeduceCanCallImplMethodSigT>                                                                                                                                                                                                                                                                                 \
            struct DeduceCanCallImpl {                                                                                                                                                                                                                                                                                                      \
            };                                                                                                                                                                                                                                                                                                                              \
            template <typename R, typename... ArgsT>                                                                                                                                                                                                                                                                                        \
            struct DeduceCanCallImpl<R (ArgsT...)> {                                                                                                                                                                                                                                                                                        \
                BOOST_PP_IIF(IsStatic, BOOST_PP_EMPTY, BOOST_PP_IDENTITY(template <typename DeduceCanCallImplClassNameT>))() static std::true_type Evaluate(R (BOOST_PP_IIF(IsStatic, BOOST_PP_EMPTY, BOOST_PP_IDENTITY(DeduceCanCallImplClassNameT::))() *)(ArgsT...) BOOST_PP_IIF(IsConst, BOOST_PP_IDENTITY(const), BOOST_PP_EMPTY)());  \
                static std::false_type Evaluate(...);                                                                                                                                                                                                                                                                                       \
                                                                                                                                                                                                                                                                                                                                            \
                using result_type = decltype(Evaluate(&TemplateTypename::MethodName));                                                                                                                                                                                                                                                      \
                static bool const value = std::is_same<std::true_type, result_type>::value;                                                                                                                                                                                                                                                 \
            };                                                                                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                                                                                                            \
            /* This indirect type seems to be necessary to work around some compiler issues */                                                                                                                                                                                                                                              \
            using ConcreteDeduceCanCallImpl = DeduceCanCallImpl<BOOST_PP_IDENTITY Signature ()>;                                                                                                                                                                                                                                            \
            static bool const value = ConcreteDeduceCanCallImpl::value;                                                                                                                                                                                                                                                                     \
        };                                                                                                                                                                                                                                                                                                                                  \
                                                                                                                                                                                                                                                                                                                                            \
                                                                                                                                                                                                                                                                                                                                            \
        /* Determine if the class hierarchy has a member with the given name */                                                                                                                                                                                                                                                             \
        template <typename T>                                                                                                                                                                                                                                                                                                               \
        struct DeduceHasMember {                                                                                                                                                                                                                                                                                                            \
            struct DeduceHasMemberDummy {                                                                                                                                                                                                                                                                                                   \
            };                                                                                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                                                                                                            \
            struct Mixin {                                                                                                                                                                                                                                                                                                                  \
                BOOST_PP_IIF(IsStatic, BOOST_PP_IDENTITY(static), BOOST_PP_EMPTY)() void MethodName(DeduceHasMemberDummy) BOOST_PP_IIF(IsConst, BOOST_PP_IDENTITY(const), BOOST_PP_EMPTY)();                                                                                                                                                \
            };                                                                                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                                                                                                            \
            struct Derived : public T, public Mixin {                                                                                                                                                                                                                                                                                       \
            };                                                                                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                                                                                                            \
            /* This helps to work around issues associated with void return types */                                                                                                                                                                                                                                                        \
            template <typename T0, T0>                                                                                                                                                                                                                                                                                                      \
            struct SFINAEHelper {                                                                                                                                                                                                                                                                                                           \
            };                                                                                                                                                                                                                                                                                                                              \
                                                                                                                                                                                                                                                                                                                                            \
            template <typename T0> static std::false_type Evaluate(T0 const *, SFINAEHelper<void (BOOST_PP_IIF(IsStatic, BOOST_PP_EMPTY, BOOST_PP_IDENTITY(Mixin::))() *)(DeduceHasMemberDummy) BOOST_PP_IIF(IsConst, BOOST_PP_IDENTITY(const), BOOST_PP_EMPTY)(), &(T0::MethodName)> const * =nullptr);                                    \
            static std::true_type Evaluate(...);                                                                                                                                                                                                                                                                                            \
                                                                                                                                                                                                                                                                                                                                            \
            static bool const value = std::is_same<std::true_type, decltype(Evaluate(reinterpret_cast<Derived BOOST_PP_IIF(IsConst, BOOST_PP_IDENTITY(const), BOOST_PP_EMPTY)() *>(nullptr)))>::value;                                                                                                                                      \
        };                                                                                                                                                                                                                                                                                                                                  \
                                                                                                                                                                                                                                                                                                                                            \
        /* Short-circuit type checkers */                                                                                                                                                                                                                                                                                                   \
        template <typename T, bool Value> struct CanCall : public std::true_type {                                                                                                                                                                                                                                                          \
        };                                                                                                                                                                                                                                                                                                                                  \
        template <typename T> struct CanCall<T, false> : public std::false_type {                                                                                                                                                                                                                                                           \
        };                                                                                                                                                                                                                                                                                                                                  \
                                                                                                                                                                                                                                                                                                                                            \
        template <typename T, bool Value> struct HasMember : public CanCall<T, DeduceCanCall<T>::value> {                                                                                                                                                                                                                                   \
        };                                                                                                                                                                                                                                                                                                                                  \
        template <typename T> struct HasMember<T, false> : public std::false_type {                                                                                                                                                                                                                                                         \
        };                                                                                                                                                                                                                                                                                                                                  \
        template <typename T, bool Value> struct IsClass : public HasMember<T, DeduceHasMember<T>::value> {                                                                                                                                                                                                                                 \
        };                                                                                                                                                                                                                                                                                                                                  \
        template <typename T> struct IsClass<T, false> : public std::false_type {                                                                                                                                                                                                                                                           \
        };                                                                                                                                                                                                                                                                                                                                  \
                                                                                                                                                                                                                                                                                                                                            \
    public:                                                                                                                                                                                                                                                                                                                                 \
        static bool const value = IsClass<HasMethodCheckerImplArgumentT, std::is_class_v<HasMethodCheckerImplArgumentT>>::value;                                                                                                                                                                                                            \
    };                                                                                                                                                                                                                                                                                                                                      \
    CREATE_HAS_STATIC_BOOL_DECLARATION(StructName, AsStaticBool);

// clang-format on

// ----------------------------------------------------------------------
// |
// |  Public Types
// |
// ----------------------------------------------------------------------
namespace {

template <typename T>
std::true_type IsContainerImplHelper(
    typename T::value_type *,
    typename T::pointer *,
    typename T::const_pointer *,
    typename T::iterator *,
    typename T::const_iterator *,
    typename T::difference_type *,
    typename T::size_type *
);

template <typename T>
std::false_type IsContainerImplHelper(...);

template <typename T>
std::true_type IsIteratorImplHelper(
    typename T::iterator_category *,
    typename T::value_type *,
    typename T::difference_type *,
    typename T::pointer *
);

template <typename T>
std::false_type IsIteratorImplHelper(...);

}  // namespace

template <typename T>
constexpr bool const IsContainerImpl = std::is_same_v<
    std::true_type,
    decltype(IsContainerImplHelper<T>(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr))
>;

template <typename T>
constexpr bool const IsIteratorImpl
    = std::is_same_v<std::true_type, decltype(IsIteratorImplHelper<T>(nullptr, nullptr, nullptr, nullptr))>;

}  // namespace Details
}  // namespace TypeTraits
}  // namespace CommonHelpers
