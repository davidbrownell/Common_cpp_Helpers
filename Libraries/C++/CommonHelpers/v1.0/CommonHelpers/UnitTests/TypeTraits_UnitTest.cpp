/////////////////////////////////////////////////////////////////////////
///
///  \file      TypeTraits_UnitTest.cpp
///  \brief     Unit test for TypeTraits.h
///
///  \author    David Brownell <db@DavidBrownell.com>
///  \date      2019-03-17 22:12:23
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
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_CONSOLE_WIDTH 200
#include "../TypeTraits.h"
#include <catch.hpp>

// ----------------------------------------------------------------------
// |
// |  CREATE_HAS_TYPE_CHECKER
// |
// ----------------------------------------------------------------------
struct One {};
struct Two {
    struct Foo {};
};
struct Three {
    using Foo = unsigned char;
};
struct Four {
    struct Foo1 {};
};
struct Five {
    using Foo1 = double;
};

CREATE_HAS_TYPE_CHECKER(Foo);
CREATE_HAS_TYPE_CHECKER(Foo1, HAS_TYPE_CHECKER_NAME(FooChecker));

static_assert(has_Foo<One> == false, "");
static_assert(has_Foo<Two>, "");
static_assert(has_Foo<Three>, "");
static_assert(has_Foo<Four> == false, "");
static_assert(has_Foo<Five> == false, "");

static_assert(FooChecker<One> == false, "");
static_assert(FooChecker<Two> == false, "");
static_assert(FooChecker<Three> == false, "");
static_assert(FooChecker<Four>, "");
static_assert(FooChecker<Five>, "");

// ----------------------------------------------------------------------
// |
// |  CREATE_HAS_METHOD_CHECKER
// |
// ----------------------------------------------------------------------
struct A {
    int Method1(void);
};
struct B {
    int Method1(void) const;
};
struct C {
    static int Method1(void);
};
struct D {
    int Method1(int, char);
};
struct E {
    int Method1(int, char) const;
};
struct F {
    static int Method1(int, char);
};
struct G {
    void Method1(int, char);
};
struct H {
    void Method1(int, char) const;
};
struct I {
    static void Method1(int, char);
};
struct J {};

struct DA : public A {};
struct DB : public B {};
struct DC : public C {};
struct DD : public D {};
struct DE : public E {};
struct DF : public F {};
struct DG : public G {};
struct DH : public H {};
struct DI : public I {};
struct DJ : public J {};

// ----------------------------------------------------------------------
// |  Anonymous Object
namespace {

struct AnonymousObj : public G {};

} // anonymous method

// Standard
CREATE_HAS_METHOD_CHECKER(Method1, (int(int, char)));
CREATE_HAS_METHOD_CHECKER(Method1, (void(int, char)), HAS_METHOD_CHECKER_NAME(has_Method1VoidReturn));
CREATE_HAS_METHOD_CHECKER(Method1, (int(void)), HAS_METHOD_CHECKER_NAME(has_Method1VoidArgs));

CREATE_HAS_METHOD_CHECKER(
    Method1,
    (int(int, char)),
    HAS_METHOD_CHECKER_CONST,
    HAS_METHOD_CHECKER_NAME(has_Method1Const)
)
CREATE_HAS_METHOD_CHECKER(
    Method1,
    (void(int, char)),
    HAS_METHOD_CHECKER_CONST,
    HAS_METHOD_CHECKER_NAME(has_Method1ConstVoidReturn)
)
CREATE_HAS_METHOD_CHECKER(
    Method1,
    (int(void)),
    HAS_METHOD_CHECKER_CONST,
    HAS_METHOD_CHECKER_NAME(has_Method1ConstVoidArgs)
)

CREATE_HAS_METHOD_CHECKER(
    Method1,
    (int(int, char)),
    HAS_METHOD_CHECKER_STATIC,
    HAS_METHOD_CHECKER_NAME(has_Method1Static)
)
CREATE_HAS_METHOD_CHECKER(
    Method1,
    (void(int, char)),
    HAS_METHOD_CHECKER_STATIC,
    HAS_METHOD_CHECKER_NAME(has_Method1StaticVoidReturn)
)
CREATE_HAS_METHOD_CHECKER(
    Method1,
    (int(void)),
    HAS_METHOD_CHECKER_STATIC,
    HAS_METHOD_CHECKER_NAME(has_Method1StaticVoidArgs)
)

// Strict
CREATE_HAS_METHOD_CHECKER(
    Method1,
    (int (Q::*)(int, char)),
    HAS_METHOD_CHECKER_NAME(has_Method1Strict),
    HAS_METHOD_CHECKER_STRICT
);
CREATE_HAS_METHOD_CHECKER(
    Method1,
    (void (Q::*)(int, char)),
    HAS_METHOD_CHECKER_NAME(has_Method1VoidReturnStrict),
    HAS_METHOD_CHECKER_STRICT
);
CREATE_HAS_METHOD_CHECKER(
    Method1,
    (int (Q::*)(void)),
    HAS_METHOD_CHECKER_NAME(has_Method1VoidArgsStrict),
    HAS_METHOD_CHECKER_STRICT
);

CREATE_HAS_METHOD_CHECKER(
    Method1,
    (int (QDifferent::*)(int, char) const),
    HAS_METHOD_CHECKER_NAME(has_Method1ConstStrict),
    HAS_METHOD_CHECKER_STRICT,
    HAS_METHOD_CHECKER_TEMPLATE_TYPENAME(QDifferent)
)
CREATE_HAS_METHOD_CHECKER(
    Method1,
    (void (QDifferent::*)(int, char) const),
    HAS_METHOD_CHECKER_NAME(has_Method1ConstVoidReturnStrict),
    HAS_METHOD_CHECKER_STRICT,
    HAS_METHOD_CHECKER_TEMPLATE_TYPENAME(QDifferent)
)
CREATE_HAS_METHOD_CHECKER(
    Method1,
    (int (QDifferent::*)(void) const),
    HAS_METHOD_CHECKER_NAME(has_Method1ConstVoidArgsStrict),
    HAS_METHOD_CHECKER_STRICT,
    HAS_METHOD_CHECKER_TEMPLATE_TYPENAME(QDifferent)
)

CREATE_HAS_METHOD_CHECKER(
    Method1,
    (int(int, char)),
    HAS_METHOD_CHECKER_NAME(has_Method1StaticStrict),
    HAS_METHOD_CHECKER_STRICT
)
CREATE_HAS_METHOD_CHECKER(
    Method1,
    (void(int, char)),
    HAS_METHOD_CHECKER_NAME(has_Method1StaticVoidReturnStrict),
    HAS_METHOD_CHECKER_STRICT
)
CREATE_HAS_METHOD_CHECKER(
    Method1,
    (int(void)),
    HAS_METHOD_CHECKER_NAME(has_Method1StaticVoidArgsStrict),
    HAS_METHOD_CHECKER_STRICT
)

// ----------------------------------------------------------------------
// |  Standard (non-const, non-static)
static_assert(has_Method1<A> == false, "");
static_assert(has_Method1<B> == false, "");
static_assert(has_Method1<C> == false, "");
static_assert(has_Method1<D>, "");
static_assert(has_Method1<E> == false, "");
static_assert(has_Method1<F> == false, "");
static_assert(has_Method1<G> == false, "");
static_assert(has_Method1<H> == false, "");
static_assert(has_Method1<I> == false, "");
static_assert(has_Method1<J> == false, "");
static_assert(has_Method1<AnonymousObj> == false, "");

static_assert(has_Method1<DA> == false, "");
static_assert(has_Method1<DB> == false, "");
static_assert(has_Method1<DC> == false, "");
static_assert(has_Method1<DD>, "");
static_assert(has_Method1<DE> == false, "");
static_assert(has_Method1<DF> == false, "");
static_assert(has_Method1<DG> == false, "");
static_assert(has_Method1<DH> == false, "");
static_assert(has_Method1<DI> == false, "");
static_assert(has_Method1<DJ> == false, "");

static_assert(has_Method1VoidReturn<A> == false, "");
static_assert(has_Method1VoidReturn<B> == false, "");
static_assert(has_Method1VoidReturn<C> == false, "");
static_assert(has_Method1VoidReturn<D> == false, "");
static_assert(has_Method1VoidReturn<E> == false, "");
static_assert(has_Method1VoidReturn<F> == false, "");
static_assert(has_Method1VoidReturn<G>, "");
static_assert(has_Method1VoidReturn<H> == false, "");
static_assert(has_Method1VoidReturn<I> == false, "");
static_assert(has_Method1VoidReturn<J> == false, "");
static_assert(has_Method1VoidReturn<AnonymousObj>, "");

static_assert(has_Method1VoidReturn<DA> == false, "");
static_assert(has_Method1VoidReturn<DB> == false, "");
static_assert(has_Method1VoidReturn<DC> == false, "");
static_assert(has_Method1VoidReturn<DD> == false, "");
static_assert(has_Method1VoidReturn<DE> == false, "");
static_assert(has_Method1VoidReturn<DF> == false, "");
static_assert(has_Method1VoidReturn<DG>, "");
static_assert(has_Method1VoidReturn<DH> == false, "");
static_assert(has_Method1VoidReturn<DI> == false, "");
static_assert(has_Method1VoidReturn<DJ> == false, "");

static_assert(has_Method1VoidArgs<A>, "");
static_assert(has_Method1VoidArgs<B> == false, "");
static_assert(has_Method1VoidArgs<C> == false, "");
static_assert(has_Method1VoidArgs<D> == false, "");
static_assert(has_Method1VoidArgs<E> == false, "");
static_assert(has_Method1VoidArgs<F> == false, "");
static_assert(has_Method1VoidArgs<G> == false, "");
static_assert(has_Method1VoidArgs<H> == false, "");
static_assert(has_Method1VoidArgs<I> == false, "");
static_assert(has_Method1VoidArgs<J> == false, "");
static_assert(has_Method1VoidArgs<AnonymousObj> == false, "");

static_assert(has_Method1VoidArgs<DA>, "");
static_assert(has_Method1VoidArgs<DB> == false, "");
static_assert(has_Method1VoidArgs<DC> == false, "");
static_assert(has_Method1VoidArgs<DD> == false, "");
static_assert(has_Method1VoidArgs<DE> == false, "");
static_assert(has_Method1VoidArgs<DF> == false, "");
static_assert(has_Method1VoidArgs<DG> == false, "");
static_assert(has_Method1VoidArgs<DH> == false, "");
static_assert(has_Method1VoidArgs<DI> == false, "");
static_assert(has_Method1VoidArgs<DJ> == false, "");

// ----------------------------------------------------------------------
// |  Const
static_assert(has_Method1Const<A> == false, "");
static_assert(has_Method1Const<B> == false, "");
static_assert(has_Method1Const<C> == false, "");
static_assert(has_Method1Const<D> == false, "");
static_assert(has_Method1Const<E>, "");
static_assert(has_Method1Const<F> == false, "");
static_assert(has_Method1Const<G> == false, "");
static_assert(has_Method1Const<H> == false, "");
static_assert(has_Method1Const<I> == false, "");
static_assert(has_Method1Const<J> == false, "");
static_assert(has_Method1Const<AnonymousObj> == false, "");

static_assert(has_Method1Const<DA> == false, "");
static_assert(has_Method1Const<DB> == false, "");
static_assert(has_Method1Const<DC> == false, "");
static_assert(has_Method1Const<DD> == false, "");
static_assert(has_Method1Const<DE>, "");
static_assert(has_Method1Const<DF> == false, "");
static_assert(has_Method1Const<DG> == false, "");
static_assert(has_Method1Const<DH> == false, "");
static_assert(has_Method1Const<DI> == false, "");
static_assert(has_Method1Const<DJ> == false, "");

static_assert(has_Method1ConstVoidReturn<A> == false, "");
static_assert(has_Method1ConstVoidReturn<B> == false, "");
static_assert(has_Method1ConstVoidReturn<C> == false, "");
static_assert(has_Method1ConstVoidReturn<D> == false, "");
static_assert(has_Method1ConstVoidReturn<E> == false, "");
static_assert(has_Method1ConstVoidReturn<F> == false, "");
static_assert(has_Method1ConstVoidReturn<G> == false, "");
static_assert(has_Method1ConstVoidReturn<H>, "");
static_assert(has_Method1ConstVoidReturn<I> == false, "");
static_assert(has_Method1ConstVoidReturn<J> == false, "");
static_assert(has_Method1ConstVoidReturn<AnonymousObj> == false, "");

static_assert(has_Method1ConstVoidReturn<DA> == false, "");
static_assert(has_Method1ConstVoidReturn<DB> == false, "");
static_assert(has_Method1ConstVoidReturn<DC> == false, "");
static_assert(has_Method1ConstVoidReturn<DD> == false, "");
static_assert(has_Method1ConstVoidReturn<DE> == false, "");
static_assert(has_Method1ConstVoidReturn<DF> == false, "");
static_assert(has_Method1ConstVoidReturn<DG> == false, "");
static_assert(has_Method1ConstVoidReturn<DH>, "");
static_assert(has_Method1ConstVoidReturn<DI> == false, "");
static_assert(has_Method1ConstVoidReturn<DJ> == false, "");

static_assert(has_Method1ConstVoidArgs<A> == false, "");
static_assert(has_Method1ConstVoidArgs<B>, "");
static_assert(has_Method1ConstVoidArgs<C> == false, "");
static_assert(has_Method1ConstVoidArgs<D> == false, "");
static_assert(has_Method1ConstVoidArgs<E> == false, "");
static_assert(has_Method1ConstVoidArgs<F> == false, "");
static_assert(has_Method1ConstVoidArgs<G> == false, "");
static_assert(has_Method1ConstVoidArgs<H> == false, "");
static_assert(has_Method1ConstVoidArgs<I> == false, "");
static_assert(has_Method1ConstVoidArgs<J> == false, "");
static_assert(has_Method1ConstVoidArgs<AnonymousObj> == false, "");

static_assert(has_Method1ConstVoidArgs<DA> == false, "");
static_assert(has_Method1ConstVoidArgs<DB>, "");
static_assert(has_Method1ConstVoidArgs<DC> == false, "");
static_assert(has_Method1ConstVoidArgs<DD> == false, "");
static_assert(has_Method1ConstVoidArgs<DE> == false, "");
static_assert(has_Method1ConstVoidArgs<DF> == false, "");
static_assert(has_Method1ConstVoidArgs<DG> == false, "");
static_assert(has_Method1ConstVoidArgs<DH> == false, "");
static_assert(has_Method1ConstVoidArgs<DI> == false, "");
static_assert(has_Method1ConstVoidArgs<DJ> == false, "");

// ----------------------------------------------------------------------
// |  Static
static_assert(has_Method1Static<A> == false, "");
static_assert(has_Method1Static<B> == false, "");
static_assert(has_Method1Static<C> == false, "");
static_assert(has_Method1Static<D> == false, "");
static_assert(has_Method1Static<E> == false, "");
static_assert(has_Method1Static<F>, "");
static_assert(has_Method1Static<G> == false, "");
static_assert(has_Method1Static<H> == false, "");
static_assert(has_Method1Static<I> == false, "");
static_assert(has_Method1Static<J> == false, "");
static_assert(has_Method1Static<AnonymousObj> == false, "");

static_assert(has_Method1Static<DA> == false, "");
static_assert(has_Method1Static<DB> == false, "");
static_assert(has_Method1Static<DC> == false, "");
static_assert(has_Method1Static<DD> == false, "");
static_assert(has_Method1Static<DE> == false, "");
static_assert(has_Method1Static<DF>, "");
static_assert(has_Method1Static<DG> == false, "");
static_assert(has_Method1Static<DH> == false, "");
static_assert(has_Method1Static<DI> == false, "");
static_assert(has_Method1Static<DJ> == false, "");

static_assert(has_Method1StaticVoidReturn<A> == false, "");
static_assert(has_Method1StaticVoidReturn<B> == false, "");
static_assert(has_Method1StaticVoidReturn<C> == false, "");
static_assert(has_Method1StaticVoidReturn<D> == false, "");
static_assert(has_Method1StaticVoidReturn<E> == false, "");
static_assert(has_Method1StaticVoidReturn<F> == false, "");
static_assert(has_Method1StaticVoidReturn<G> == false, "");
static_assert(has_Method1StaticVoidReturn<H> == false, "");
static_assert(has_Method1StaticVoidReturn<I>, "");
static_assert(has_Method1StaticVoidReturn<J> == false, "");
static_assert(has_Method1StaticVoidReturn<AnonymousObj> == false, "");

static_assert(has_Method1StaticVoidReturn<DA> == false, "");
static_assert(has_Method1StaticVoidReturn<DB> == false, "");
static_assert(has_Method1StaticVoidReturn<DC> == false, "");
static_assert(has_Method1StaticVoidReturn<DD> == false, "");
static_assert(has_Method1StaticVoidReturn<DE> == false, "");
static_assert(has_Method1StaticVoidReturn<DF> == false, "");
static_assert(has_Method1StaticVoidReturn<DG> == false, "");
static_assert(has_Method1StaticVoidReturn<DH> == false, "");
static_assert(has_Method1StaticVoidReturn<DI>, "");
static_assert(has_Method1StaticVoidReturn<DJ> == false, "");

static_assert(has_Method1StaticVoidArgs<A> == false, "");
static_assert(has_Method1StaticVoidArgs<B> == false, "");
static_assert(has_Method1StaticVoidArgs<C>, "");
static_assert(has_Method1StaticVoidArgs<D> == false, "");
static_assert(has_Method1StaticVoidArgs<E> == false, "");
static_assert(has_Method1StaticVoidArgs<F> == false, "");
static_assert(has_Method1StaticVoidArgs<G> == false, "");
static_assert(has_Method1StaticVoidArgs<H> == false, "");
static_assert(has_Method1StaticVoidArgs<I> == false, "");
static_assert(has_Method1StaticVoidArgs<J> == false, "");
static_assert(has_Method1StaticVoidArgs<AnonymousObj> == false, "");

static_assert(has_Method1StaticVoidArgs<DA> == false, "");
static_assert(has_Method1StaticVoidArgs<DB> == false, "");
static_assert(has_Method1StaticVoidArgs<DC>, "");
static_assert(has_Method1StaticVoidArgs<DD> == false, "");
static_assert(has_Method1StaticVoidArgs<DE> == false, "");
static_assert(has_Method1StaticVoidArgs<DF> == false, "");
static_assert(has_Method1StaticVoidArgs<DG> == false, "");
static_assert(has_Method1StaticVoidArgs<DH> == false, "");
static_assert(has_Method1StaticVoidArgs<DI> == false, "");
static_assert(has_Method1StaticVoidArgs<DJ> == false, "");

// ----------------------------------------------------------------------
// |  Strict Standard (non-const, non-static)
static_assert(has_Method1Strict<A> == false, "");
static_assert(has_Method1Strict<B> == false, "");
static_assert(has_Method1Strict<C> == false, "");
static_assert(has_Method1Strict<D>, "");
static_assert(has_Method1Strict<E> == false, "");
static_assert(has_Method1Strict<F> == false, "");
static_assert(has_Method1Strict<G> == false, "");
static_assert(has_Method1Strict<H> == false, "");
static_assert(has_Method1Strict<I> == false, "");
static_assert(has_Method1Strict<J> == false, "");
static_assert(has_Method1Strict<AnonymousObj> == false, "");

static_assert(has_Method1Strict<DA> == false, "");
static_assert(has_Method1Strict<DB> == false, "");
static_assert(has_Method1Strict<DC> == false, "");
static_assert(has_Method1Strict<DD> == false, "");
static_assert(has_Method1Strict<DE> == false, "");
static_assert(has_Method1Strict<DF> == false, "");
static_assert(has_Method1Strict<DG> == false, "");
static_assert(has_Method1Strict<DH> == false, "");
static_assert(has_Method1Strict<DI> == false, "");
static_assert(has_Method1Strict<DJ> == false, "");

static_assert(has_Method1VoidReturnStrict<A> == false, "");
static_assert(has_Method1VoidReturnStrict<B> == false, "");
static_assert(has_Method1VoidReturnStrict<C> == false, "");
static_assert(has_Method1VoidReturnStrict<D> == false, "");
static_assert(has_Method1VoidReturnStrict<E> == false, "");
static_assert(has_Method1VoidReturnStrict<F> == false, "");
static_assert(has_Method1VoidReturnStrict<G>, "");
static_assert(has_Method1VoidReturnStrict<H> == false, "");
static_assert(has_Method1VoidReturnStrict<I> == false, "");
static_assert(has_Method1VoidReturnStrict<J> == false, "");
static_assert(has_Method1VoidReturnStrict<AnonymousObj> == false, "");

static_assert(has_Method1VoidReturnStrict<DA> == false, "");
static_assert(has_Method1VoidReturnStrict<DB> == false, "");
static_assert(has_Method1VoidReturnStrict<DC> == false, "");
static_assert(has_Method1VoidReturnStrict<DD> == false, "");
static_assert(has_Method1VoidReturnStrict<DE> == false, "");
static_assert(has_Method1VoidReturnStrict<DF> == false, "");
static_assert(has_Method1VoidReturnStrict<DG> == false, "");
static_assert(has_Method1VoidReturnStrict<DH> == false, "");
static_assert(has_Method1VoidReturnStrict<DI> == false, "");
static_assert(has_Method1VoidReturnStrict<DJ> == false, "");

static_assert(has_Method1VoidArgsStrict<A>, "");
static_assert(has_Method1VoidArgsStrict<B> == false, "");
static_assert(has_Method1VoidArgsStrict<C> == false, "");
static_assert(has_Method1VoidArgsStrict<D> == false, "");
static_assert(has_Method1VoidArgsStrict<E> == false, "");
static_assert(has_Method1VoidArgsStrict<F> == false, "");
static_assert(has_Method1VoidArgsStrict<G> == false, "");
static_assert(has_Method1VoidArgsStrict<H> == false, "");
static_assert(has_Method1VoidArgsStrict<I> == false, "");
static_assert(has_Method1VoidArgsStrict<J> == false, "");
static_assert(has_Method1VoidArgsStrict<AnonymousObj> == false, "");

static_assert(has_Method1VoidArgsStrict<DA> == false, "");
static_assert(has_Method1VoidArgsStrict<DB> == false, "");
static_assert(has_Method1VoidArgsStrict<DC> == false, "");
static_assert(has_Method1VoidArgsStrict<DD> == false, "");
static_assert(has_Method1VoidArgsStrict<DE> == false, "");
static_assert(has_Method1VoidArgsStrict<DF> == false, "");
static_assert(has_Method1VoidArgsStrict<DG> == false, "");
static_assert(has_Method1VoidArgsStrict<DH> == false, "");
static_assert(has_Method1VoidArgsStrict<DI> == false, "");
static_assert(has_Method1VoidArgsStrict<DJ> == false, "");

// ----------------------------------------------------------------------
// |  Strict Const
static_assert(has_Method1ConstStrict<A> == false, "");
static_assert(has_Method1ConstStrict<B> == false, "");
static_assert(has_Method1ConstStrict<C> == false, "");
static_assert(has_Method1ConstStrict<D> == false, "");
static_assert(has_Method1ConstStrict<E>, "");
static_assert(has_Method1ConstStrict<F> == false, "");
static_assert(has_Method1ConstStrict<G> == false, "");
static_assert(has_Method1ConstStrict<H> == false, "");
static_assert(has_Method1ConstStrict<I> == false, "");
static_assert(has_Method1ConstStrict<J> == false, "");
static_assert(has_Method1ConstStrict<AnonymousObj> == false, "");

static_assert(has_Method1ConstStrict<DA> == false, "");
static_assert(has_Method1ConstStrict<DB> == false, "");
static_assert(has_Method1ConstStrict<DC> == false, "");
static_assert(has_Method1ConstStrict<DD> == false, "");
static_assert(has_Method1ConstStrict<DE> == false, "");
static_assert(has_Method1ConstStrict<DF> == false, "");
static_assert(has_Method1ConstStrict<DG> == false, "");
static_assert(has_Method1ConstStrict<DH> == false, "");
static_assert(has_Method1ConstStrict<DI> == false, "");
static_assert(has_Method1ConstStrict<DJ> == false, "");

static_assert(has_Method1ConstVoidReturnStrict<A> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<B> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<C> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<D> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<E> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<F> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<G> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<H>, "");
static_assert(has_Method1ConstVoidReturnStrict<I> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<J> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<AnonymousObj> == false, "");

static_assert(has_Method1ConstVoidReturnStrict<DA> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<DB> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<DC> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<DD> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<DE> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<DF> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<DG> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<DH> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<DI> == false, "");
static_assert(has_Method1ConstVoidReturnStrict<DJ> == false, "");

static_assert(has_Method1ConstVoidArgsStrict<A> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<B>, "");
static_assert(has_Method1ConstVoidArgsStrict<C> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<D> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<E> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<F> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<G> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<H> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<I> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<J> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<AnonymousObj> == false, "");

static_assert(has_Method1ConstVoidArgsStrict<DA> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<DB> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<DC> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<DD> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<DE> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<DF> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<DG> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<DH> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<DI> == false, "");
static_assert(has_Method1ConstVoidArgsStrict<DJ> == false, "");

// ----------------------------------------------------------------------
// |  Strict Static
static_assert(has_Method1StaticStrict<A> == false, "");
static_assert(has_Method1StaticStrict<B> == false, "");
static_assert(has_Method1StaticStrict<C> == false, "");
static_assert(has_Method1StaticStrict<D> == false, "");
static_assert(has_Method1StaticStrict<E> == false, "");
static_assert(has_Method1StaticStrict<F>, "");
static_assert(has_Method1StaticStrict<G> == false, "");
static_assert(has_Method1StaticStrict<H> == false, "");
static_assert(has_Method1StaticStrict<I> == false, "");
static_assert(has_Method1StaticStrict<J> == false, "");
static_assert(has_Method1StaticStrict<AnonymousObj> == false, "");

static_assert(has_Method1StaticStrict<DA> == false, "");
static_assert(has_Method1StaticStrict<DB> == false, "");
static_assert(has_Method1StaticStrict<DC> == false, "");
static_assert(has_Method1StaticStrict<DD> == false, "");
static_assert(has_Method1StaticStrict<DE> == false, "");
static_assert(has_Method1StaticStrict<DF>, "");
static_assert(has_Method1StaticStrict<DG> == false, "");
static_assert(has_Method1StaticStrict<DH> == false, "");
static_assert(has_Method1StaticStrict<DI> == false, "");
static_assert(has_Method1StaticStrict<DJ> == false, "");

static_assert(has_Method1StaticVoidReturnStrict<A> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<B> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<C> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<D> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<E> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<F> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<G> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<H> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<I>, "");
static_assert(has_Method1StaticVoidReturnStrict<J> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<AnonymousObj> == false, "");

static_assert(has_Method1StaticVoidReturnStrict<DA> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<DB> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<DC> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<DD> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<DE> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<DF> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<DG> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<DH> == false, "");
static_assert(has_Method1StaticVoidReturnStrict<DI>, "");
static_assert(has_Method1StaticVoidReturnStrict<DJ> == false, "");

static_assert(has_Method1StaticVoidArgsStrict<A> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<B> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<C>, "");
static_assert(has_Method1StaticVoidArgsStrict<D> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<E> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<F> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<G> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<H> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<I> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<J> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<AnonymousObj> == false, "");

static_assert(has_Method1StaticVoidArgsStrict<DA> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<DB> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<DC>, "");
static_assert(has_Method1StaticVoidArgsStrict<DD> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<DE> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<DF> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<DG> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<DH> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<DI> == false, "");
static_assert(has_Method1StaticVoidArgsStrict<DJ> == false, "");

// ----------------------------------------------------------------------
struct OpA {
    bool operator<(OpA const &) const;
};
struct OpB {
    bool operator>(OpB const &) const;
};
struct OpC {
    bool operator!=(OpC const &) const;
};

struct DOpA : public OpA {};
struct DOpB : public OpB {};
struct DOpC : public OpC {};

CREATE_HAS_METHOD_CHECKER(operator<,(bool(Q const &)), HAS_METHOD_CHECKER_NAME(has_LessThan), HAS_METHOD_CHECKER_CONST);
CREATE_HAS_METHOD_CHECKER(operator>,(bool(Q const &)), HAS_METHOD_CHECKER_NAME(has_GreaterThan), HAS_METHOD_CHECKER_CONST);
CREATE_HAS_METHOD_CHECKER(operator!=,(bool(QDifferent const &)), HAS_METHOD_CHECKER_NAME(has_NotEqual), HAS_METHOD_CHECKER_CONST, HAS_METHOD_CHECKER_TEMPLATE_TYPENAME(QDifferent));

CREATE_HAS_METHOD_CHECKER(operator<,(bool (Q::*)(Q const &) const), HAS_METHOD_CHECKER_NAME(has_LessThanStrict), HAS_METHOD_CHECKER_STRICT);
CREATE_HAS_METHOD_CHECKER(operator>,(bool (Q::*)(Q const &) const), HAS_METHOD_CHECKER_NAME(has_GreaterThanStrict), HAS_METHOD_CHECKER_STRICT);
CREATE_HAS_METHOD_CHECKER(operator!=,(bool (QDifferent::*)(QDifferent const &) const), HAS_METHOD_CHECKER_NAME(has_NotEqualStrict), HAS_METHOD_CHECKER_TEMPLATE_TYPENAME(QDifferent), HAS_METHOD_CHECKER_STRICT);

// ----------------------------------------------------------------------
// |  Standard
static_assert(has_LessThan<OpA>, "");
static_assert(has_LessThan<OpB> == false, "");
static_assert(has_LessThan<OpC> == false, "");

static_assert(has_LessThan<DOpA> == false, "");
static_assert(has_LessThan<DOpB> == false, "");
static_assert(has_LessThan<DOpC> == false, "");

static_assert(has_GreaterThan<OpA> == false, "");
static_assert(has_GreaterThan<OpB>, "");
static_assert(has_GreaterThan<OpC> == false, "");

static_assert(has_GreaterThan<DOpA> == false, "");
static_assert(has_GreaterThan<DOpB> == false, "");
static_assert(has_GreaterThan<DOpC> == false, "");

static_assert(has_NotEqual<OpA> == false, "");
static_assert(has_NotEqual<OpB> == false, "");
static_assert(has_NotEqual<OpC>, "");

static_assert(has_NotEqual<DOpA> == false, "");
static_assert(has_NotEqual<DOpB> == false, "");
static_assert(has_NotEqual<DOpC> == false, "");

// ----------------------------------------------------------------------
// |  Strict
static_assert(has_LessThanStrict<OpA>, "");
static_assert(has_LessThanStrict<OpB> == false, "");
static_assert(has_LessThanStrict<OpC> == false, "");

static_assert(has_LessThanStrict<DOpA> == false, "");
static_assert(has_LessThanStrict<DOpB> == false, "");
static_assert(has_LessThanStrict<DOpC> == false, "");

static_assert(has_GreaterThanStrict<OpA> == false, "");
static_assert(has_GreaterThanStrict<OpB>, "");
static_assert(has_GreaterThanStrict<OpC> == false, "");

static_assert(has_GreaterThanStrict<DOpA> == false, "");
static_assert(has_GreaterThanStrict<DOpB> == false, "");
static_assert(has_GreaterThanStrict<DOpC> == false, "");

static_assert(has_NotEqualStrict<OpA> == false, "");
static_assert(has_NotEqualStrict<OpB> == false, "");
static_assert(has_NotEqualStrict<OpC>, "");

static_assert(has_NotEqualStrict<DOpA> == false, "");
static_assert(has_NotEqualStrict<DOpB> == false, "");
static_assert(has_NotEqualStrict<DOpC> == false, "");

// ----------------------------------------------------------------------
// |
// |  make_mutable
// |
// ----------------------------------------------------------------------
TEST_CASE("make_mutable") {
    int a(10);

    CHECK(a == 10);
    make_mutable(a) = 11;
    CHECK(a == 11);

    int const &b(a);

    CHECK(b == 11);
    make_mutable(b) = 12;
    CHECK(b == 12);
    CHECK(a == 12);

    int const c(11);

    CHECK(c == 11);
    make_mutable(c) = 12;
    CHECK(c == 12);

    int const *d(&c);

    CHECK(*d == 12);
    *make_mutable(d) = 13;
    CHECK(*d == 13);
    CHECK(c == 13);
}

// ----------------------------------------------------------------------
// |
// |  IsSmartPointer
// |
// ----------------------------------------------------------------------
static_assert(CommonHelpers::TypeTraits::IsSmartPointer<int *> == false, "");
static_assert(CommonHelpers::TypeTraits::IsSmartPointer<std::unique_ptr<int>>, "");
static_assert(CommonHelpers::TypeTraits::IsSmartPointer<std::shared_ptr<int>>, "");
static_assert(CommonHelpers::TypeTraits::IsSmartPointer<std::unique_ptr<int const>>, "");
static_assert(CommonHelpers::TypeTraits::IsSmartPointer<std::shared_ptr<int const>>, "");

// ----------------------------------------------------------------------
// |
// |  IsContainer
// |
// ----------------------------------------------------------------------
static_assert(CommonHelpers::TypeTraits::IsContainer<int> == false, "");
static_assert(CommonHelpers::TypeTraits::IsContainer<std::vector<char>>, "");
static_assert(CommonHelpers::TypeTraits::IsContainer<std::vector<char>::iterator> == false, "");
static_assert(CommonHelpers::TypeTraits::IsContainer<std::set<int>>, "");

// ----------------------------------------------------------------------
// |
// |  IsVector
// |
// ----------------------------------------------------------------------
static_assert(CommonHelpers::TypeTraits::IsVector<int> == false, "");
static_assert(CommonHelpers::TypeTraits::IsVector<std::vector<int>>, "");

// ----------------------------------------------------------------------
// |
// |  IsTargetImmutable
// |
// ----------------------------------------------------------------------
static_assert(CommonHelpers::TypeTraits::IsTargetImmutable<int> == false, "");
static_assert(CommonHelpers::TypeTraits::IsTargetImmutable<int const>, "");
static_assert(CommonHelpers::TypeTraits::IsTargetImmutable<int *> == false, "");
static_assert(CommonHelpers::TypeTraits::IsTargetImmutable<int const *>, "");
static_assert(CommonHelpers::TypeTraits::IsTargetImmutable<int *const> == false, "");
static_assert(CommonHelpers::TypeTraits::IsTargetImmutable<int &> == false, "");
static_assert(CommonHelpers::TypeTraits::IsTargetImmutable<int const &>, "");
static_assert(CommonHelpers::TypeTraits::IsTargetImmutable<int const **> == false, "");
static_assert(CommonHelpers::TypeTraits::IsTargetImmutable<int const *const *>, "");

// ----------------------------------------------------------------------
// |
// |  IsTargetMutable
// |
// ----------------------------------------------------------------------
static_assert(CommonHelpers::TypeTraits::IsTargetMutable<int>, "");
static_assert(CommonHelpers::TypeTraits::IsTargetMutable<int const> == false, "");
static_assert(CommonHelpers::TypeTraits::IsTargetMutable<int *>, "");
static_assert(CommonHelpers::TypeTraits::IsTargetMutable<int const *> == false, "");
static_assert(CommonHelpers::TypeTraits::IsTargetMutable<int *const>, "");
static_assert(CommonHelpers::TypeTraits::IsTargetMutable<int &>, "");
static_assert(CommonHelpers::TypeTraits::IsTargetMutable<int const &> == false, "");
static_assert(CommonHelpers::TypeTraits::IsTargetMutable<int const **>, "");
static_assert(CommonHelpers::TypeTraits::IsTargetMutable<int const *const *> == false, "");

// ----------------------------------------------------------------------
// |
// |  MakeTargetImmutable
// |
// ----------------------------------------------------------------------
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetImmutable<int const>, int const>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetImmutable<int>, int const>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetImmutable<int const &>, int const &>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetImmutable<int &>, int const &>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetImmutable<int const *>, int const *>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetImmutable<int *>, int const *>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetImmutable<int const *>, int const *>, "");

// ----------------------------------------------------------------------
// |
// |  MakeTargetMutable
// |
// ----------------------------------------------------------------------
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetMutable<int>, int>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetMutable<int const>, int>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetMutable<int &>, int &>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetMutable<int const &>, int &>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetMutable<int *>, int *>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetMutable<int const *>, int *>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::MakeTargetMutable<int const *const>, int *>, "");

// ----------------------------------------------------------------------
// |
// |  Access
// |
// ----------------------------------------------------------------------
class ObjectWithFinalConstruct {
private:
    friend class CommonHelpers::TypeTraits::Access;

    void FinalConstruct(void) {}
};

class ObjectWithoutFinalConstruct {
private:
    friend class CommonHelpers::TypeTraits::Access;
};

TEST_CASE("Access") {
    ObjectWithFinalConstruct                with;
    ObjectWithoutFinalConstruct             without;

    CommonHelpers::TypeTraits::Access::FinalConstruct(with);
    CommonHelpers::TypeTraits::Access::FinalConstruct(without);
    CHECK(true); // Avoid warnings
}

// ----------------------------------------------------------------------
// |
// |  FunctionTraits
// |
// ----------------------------------------------------------------------
int Func(void);
void Func1(int);
int Func2(int, bool);

static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(Func)>::return_type, int>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(Func)>::args, std::tuple<>>, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(Func)>::is_method == false, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(Func)>::is_const == false, "");

static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(Func1)>::return_type, void>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(Func1)>::args, std::tuple<int>>, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(Func1)>::is_method == false, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(Func1)>::is_const == false, "");

static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(Func2)>::return_type, int>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(Func2)>::args, std::tuple<int, bool>>, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(Func2)>::is_method == false, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(Func2)>::is_const == false, "");

struct FunctionTraitsObj {
    int Method(void);
    void Method1(int);
    int Method2(int, bool);

    int ConstMethod(void) const;
    void ConstMethod1(int) const;
    int ConstMethod2(int, bool) const;
};

static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::Method)>::return_type, int>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::Method)>::args, std::tuple<>>, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::Method)>::is_method, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::Method)>::is_const == false, "");

static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::Method1)>::return_type, void>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::Method1)>::args, std::tuple<int>>, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::Method1)>::is_method, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::Method1)>::is_const == false, "");

static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::Method2)>::return_type, int>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::Method2)>::args, std::tuple<int, bool>>, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::Method2)>::is_method, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::Method2)>::is_const == false, "");

static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::ConstMethod)>::return_type, int>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::ConstMethod)>::args, std::tuple<>>, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::ConstMethod)>::is_method, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::ConstMethod)>::is_const, "");

static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::ConstMethod1)>::return_type, void>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::ConstMethod1)>::args, std::tuple<int>>, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::ConstMethod1)>::is_method, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::ConstMethod1)>::is_const, "");

static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::ConstMethod2)>::return_type, int>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::ConstMethod2)>::args, std::tuple<int, bool>>, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::ConstMethod2)>::is_method, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(&FunctionTraitsObj::ConstMethod2)>::is_const, "");

// std::function
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<std::function<int (void)>>::return_type, int>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<std::function<int (void)>>::args, std::tuple<>>, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<std::function<int (void)>>::is_method == false, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<std::function<int (void)>>::is_const == false, "");

static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<std::function<void (int)>>::return_type, void>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<std::function<void (int)>>::args, std::tuple<int>>, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<std::function<void (int)>>::is_method == false, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<std::function<void (int)>>::is_const == false, "");

static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<std::function<int (int, bool)>>::return_type, int>, "");
static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<std::function<int (int, bool)>>::args, std::tuple<int, bool>>, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<std::function<int (int, bool)>>::is_method == false, "");
static_assert(CommonHelpers::TypeTraits::FunctionTraits<std::function<int (int, bool)>>::is_const == false, "");

TEST_CASE("FunctionTraits - lambda") {
    auto const                              func([](void) { return 10; });
    auto const                              func1([](int) {});
    auto const                              func2([](int, bool) { return 10; });

    static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(func)>::return_type, int>, "");
    static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(func)>::args, std::tuple<>>, "");
    static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(func)>::is_method == false, "");
    static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(func)>::is_const == false, "");

    static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(func1)>::return_type, void>, "");
    static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(func1)>::args, std::tuple<int>>, "");
    static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(func1)>::is_method == false, "");
    static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(func1)>::is_const == false, "");

    static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(func2)>::return_type, int>, "");
    static_assert(std::is_same_v<CommonHelpers::TypeTraits::FunctionTraits<decltype(func2)>::args, std::tuple<int, bool>>, "");
    static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(func2)>::is_method == false, "");
    static_assert(CommonHelpers::TypeTraits::FunctionTraits<decltype(func2)>::is_const == false, "");

    // Avoid warnings
    CHECK(true);
}
