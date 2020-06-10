/////////////////////////////////////////////////////////////////////////
///
///  \file          ThreadSafeCounter.h
///  \brief         Contains the ThreadSafeCounter object
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-06-05 11:06:50
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
#pragma once

#include "Copy.h"
#include "Move.h"

#include <cassert>

#if (defined __cpp_lib_atomic_wait) // c++20
    static_assert(false, "This code path has not been tested yet");
#   include <atomic>
#else
#   include <condition_variable>
#   include <mutex>
#endif

namespace CommonHelpers {

/////////////////////////////////////////////////////////////////////////
///  \class         ThreadSafeCounter
///  \brief         Thread safe counter that can be incremented, decremented,
///                 and waited upon. This implementation takes advantage of
///                 std::atomic's wait functionality introduced in C++20 if
///                 it is available.
///
class ThreadSafeCounter {
public:
    // ----------------------------------------------------------------------
    // |
    // |  Public Types
    // |
    // ----------------------------------------------------------------------
    using value_type                        = std::uint32_t;

    // ----------------------------------------------------------------------
    // |
    // |  Public Methods
    // |
    // ----------------------------------------------------------------------
    ThreadSafeCounter(value_type initialValue);
    ~ThreadSafeCounter(void) = default;

    NON_COPYABLE(ThreadSafeCounter);
    NON_MOVABLE(ThreadSafeCounter);

    value_type GetValue(void) const;

    ThreadSafeCounter & Increment(void);
    ThreadSafeCounter & Decrement(void);

    ThreadSafeCounter & wait_value(value_type value);

private:
    // ----------------------------------------------------------------------
    // |
    // |  Private Data
    // |
    // ----------------------------------------------------------------------
#if (defined __cpp_lib_atomic_wait)
    std::atomic_uint32_t                    _ctr;
#else
    mutable std::mutex                      _ctrMutex;
    std::condition_variable                 _ctrCV;
    std::uint32_t                           _ctr;
#endif
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
inline ThreadSafeCounter::ThreadSafeCounter(value_type initialValue) :
    _ctr(std::move(initialValue))
{}

inline ThreadSafeCounter::value_type ThreadSafeCounter::GetValue(void) const {
#if (defined __cpp_lib_atomic_wait)
    return _ctr;
#else
    std::scoped_lock                        lock(_ctrMutex); UNUSED(lock);

    return _ctr;
#endif
}

inline ThreadSafeCounter & ThreadSafeCounter::Increment(void) {
#if (defined __cpp_lib_atomic_wait)
    ++_ctr;
    _ctr.notify_all();
#else
    {
        std::scoped_lock                    lock(_ctrMutex); UNUSED(lock);

        ++_ctr;
    }

    _ctrCV.notify_all();
#endif

    return *this;
}

inline ThreadSafeCounter & ThreadSafeCounter::Decrement(void) {
#if (defined __cpp_lib_atomic_wait)
    assert(_ctr >= 1);
    --_ctr;
    _ctr.notify_all();
#else
    {
        std::scoped_lock                    lock(_ctrMutex); UNUSED(lock);

        assert(_ctr >= 1);
        --_ctr;
    }

    _ctrCV.notify_all();
#endif

    return *this;
}

inline ThreadSafeCounter & ThreadSafeCounter::wait_value(value_type value) {
#if (defined __cpp_lib_atomic_wait)
    for(;;) {
        value_type                          currentValue(_ctr);

        if(currentValue == value)
            break;

        _ctr.wait(currentValue);
    }
#else
    {
        std::unique_lock                    lock(_ctrMutex);

        _ctrCV.wait(
            lock,
            [this, &value](void) {
                return _ctr == value;
            }
        );
    }
#endif

    return *this;
}

} // namespace CommonHelpers
