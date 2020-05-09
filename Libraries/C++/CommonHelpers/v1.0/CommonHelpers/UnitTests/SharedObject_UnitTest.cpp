/////////////////////////////////////////////////////////////////////////
///
///  \file          SharedObject_UnitTest.cpp
///  \brief         Unit test for SharedObject.h
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-05-08 20:51:53
///
///  \note
///
///  \bug
///
/////////////////////////////////////////////////////////////////////////
///
///  \attention
///  Copyright David Brownell 2020
///  Distributed under the Boost Software License, Version 1.0. See
///  accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt.
///
/////////////////////////////////////////////////////////////////////////
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_CONSOLE_WIDTH 200
#include "../SharedObject.h"
#include <catch.hpp>

struct Base : public CommonHelpers::SharedObject {
    int const                               I;

    CREATE(Base);

    template <typename PrivateConstructorTagT>
    Base(PrivateConstructorTagT tag, int i) :
        CommonHelpers::SharedObject(tag),
        I(i)
    {}

    MOVE(Base, MEMBERS(I), BASES(CommonHelpers::SharedObject));
    NON_COPYABLE(Base);
    COMPARE(Base, MEMBERS(I), BASES(CommonHelpers::SharedObject));
};

struct Derived : public Base {
    bool const                              B;

    CREATE(Derived);

    template <typename PrivateConstructorTagT>
    Derived(PrivateConstructorTagT tag, int i, bool b) :
        Base(tag, i),
        B(b)
    {}

    MOVE(Derived, MEMBERS(B), BASES(Base));
    NON_COPYABLE(Derived);
    COMPARE(Derived, MEMBERS(B), BASES(Base));
};

TEST_CASE("Standard") {
#if 0
    Base                                    base(Base::PrivateConstructorTag(), 10); // compiler error
#endif

    std::shared_ptr<Base>                   pBase(Base::Create(10));

    CHECK(pBase->I == 10);

    std::shared_ptr<Derived>                pDerived(Derived::Create(20, true));

    CHECK(pDerived->B == true);
    CHECK(pDerived->I == 20);

    std::shared_ptr<Base>                   pNewBase(pBase->CreateSharedPtr<Base>());

    CHECK(pNewBase.get() == pBase.get());

    std::shared_ptr<Derived>                pNewDerived(pDerived->CreateSharedPtr<Derived>());

    CHECK(pNewDerived.get() == pDerived.get());

    std::shared_ptr<Base>                   pBaseFromDerived(pDerived->CreateSharedPtr<Base>());

    CHECK(pBaseFromDerived.get() == pDerived.get());
    CHECK(pBaseFromDerived->I == 20);
}
