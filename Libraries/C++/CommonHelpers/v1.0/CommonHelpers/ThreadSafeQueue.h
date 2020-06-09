/////////////////////////////////////////////////////////////////////////
///
///  \file          ThreadSafeQueue.h
///  \brief         Contains the ThreadSafeQueue object
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-06-08 08:48:19
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

#include "Compare.h"
#include "Constructor.h"
#include "Copy.h"
#include "Finally.h"
#include "Misc.h"
#include "Move.h"
#include "ThreadSafeCounter.h"

#include <optional>
#include <queue>
#include <tuple>

namespace CommonHelpers {

/////////////////////////////////////////////////////////////////////////
///  \class         ThreadSafeQueueStoppedException
///  \brief         Exception thrown when attempting to pop a value on a queue
///                 that is in the process of being destroyed.
///
struct ThreadSafeQueueStoppedException : public std::runtime_error {
    ThreadSafeQueueStoppedException(void) : std::runtime_error("ThreadSafeQueue is stopped") {}
};

/////////////////////////////////////////////////////////////////////////
///  \class         ThreadSafeQueueEmptyException
///  \brief         Exception thrown in calls to Pop when attempting to get a value
///                 from an empty queue.
///
struct ThreadSafeQueueEmptyException : public std::runtime_error {
    ThreadSafeQueueEmptyException(void) : std::runtime_error("ThreadSafeQueue is empty") {}
};

/////////////////////////////////////////////////////////////////////////
///  \enum          QueuePopType
///  \brief         Controls if calls to Pop are blocking- or non-blocking.
///
enum class QueuePopType {
    NonBlocking,                        /// Calls to Pop will not block, but rather throw an exception if no items are available in the queue
    Blocking                            /// Calls to Pop will block until an item is available in the queue or a stop is requested
};

/////////////////////////////////////////////////////////////////////////
///  \class         ThreadSafeQueue
///  \brief         Queue that is thread safe while also supporting efficient
///                 calls to Pop across multiple threads; this functionality
///                 can be used to implement a thread pool with multiple workers.
///
template <
    typename QueueValueT,
    typename OptionalQueryValueT=std::optional<QueueValueT>
>
class ThreadSafeQueue {
public:
    // ----------------------------------------------------------------------
    // |
    // |  Public Types
    // |
    // ----------------------------------------------------------------------
    using value_type                        = QueueValueT;
    using optional_value_type               = OptionalQueryValueT;

    using PopType                           = CommonHelpers::QueuePopType;

    // ----------------------------------------------------------------------
    // |
    // |  Public Methods
    // |
    // ----------------------------------------------------------------------
    ThreadSafeQueue(void);
    ~ThreadSafeQueue(void);

    NON_COPYABLE(ThreadSafeQueue);
    NON_MOVABLE(ThreadSafeQueue);

    void Stop(void);

    bool empty(void) const;
    size_t size(void) const;

    template <typename... ArgTs> void Push(ArgTs &&... args);
    template <typename... ArgTs> bool TryPush(ArgTs &&... args);

    value_type Pop(
        QueuePopType type=QueuePopType::NonBlocking
    );

    optional_value_type Pop(std::chrono::steady_clock::duration const &waitFor);
    optional_value_type Pop(std::chrono::steady_clock::time_point const &waitUntil);

    optional_value_type TryPop(void);

private:
    // ----------------------------------------------------------------------
    // |
    // |  Private Types
    // |
    // ----------------------------------------------------------------------

    /////////////////////////////////////////////////////////////////////////
    ///  \class         Info
    ///  \brief         Internal information stored about the queue. All access
    ///                 to this information must be protected by a mutex.
    ///
    struct Info {
        // ----------------------------------------------------------------------
        // |  Public Data
        bool                                stopped;
        std::queue<value_type>              queue;

        // ----------------------------------------------------------------------
        // |  Public Methods
        Info(void) : stopped(false) {}

        NON_COPYABLE(Info);

#define ARGS                                MEMBERS(stopped, queue)

        CONSTRUCTOR(Info, ARGS);
        MOVE(Info, ARGS);
        COMPARE(Info, ARGS);

#undef ARGS
    };

    // ----------------------------------------------------------------------
    // |
    // |  Private Data
    // |
    // ----------------------------------------------------------------------
    std::function<void (void)> const        _decrementActivePopsFunc;
    std::function<void (void)> const        _notifyOneFunc;

    mutable std::mutex                      _infoMutex;
    std::condition_variable                 _infoCV;
    Info                                    _info;

    ThreadSafeCounter                       _activePops;

    // ----------------------------------------------------------------------
    // |
    // |  Private Methods
    // |
    // ----------------------------------------------------------------------
    optional_value_type PopImpl(
        bool shouldWait,
        std::chrono::steady_clock::time_point const &wait=std::chrono::steady_clock::time_point::max()
    );
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
template <typename T1, typename T2>
ThreadSafeQueue<T1, T2>::ThreadSafeQueue(void) :
    _decrementActivePopsFunc(
        [this](void) {
            _activePops.Decrement();
        }
    ),
    _notifyOneFunc(
        [this](void) {
            _infoCV.notify_one();
        }
    ),
    _activePops(0)
{}

template <typename T1, typename T2>
ThreadSafeQueue<T1, T2>::~ThreadSafeQueue(void) {
    Stop();
}

template <typename T1, typename T2>
void ThreadSafeQueue<T1, T2>::Stop(void) {
    {
        std::scoped_lock                    lock(_infoMutex); UNUSED(lock);

        if(_info.stopped)
            return;

        _info.stopped = true;
    }

    _infoCV.notify_all();

    _activePops.wait_value(0);

#if (defined DEBUG)
    {
        std::scoped_lock                    lock(_infoMutex); UNUSED(lock);

        assert(_info.queue.empty());
    }
#endif
}

template <typename T1, typename T2>
bool ThreadSafeQueue<T1, T2>::empty(void) const {
    std::scoped_lock                        lock(_infoMutex); UNUSED(lock);

    return _info.queue.empty();
}

template <typename T1, typename T2>
size_t ThreadSafeQueue<T1, T2>::size(void) const {
    std::scoped_lock                        lock(_infoMutex); UNUSED(lock);

    return _info.queue.size();
}

template <typename T1, typename T2>
template <typename... ArgTs>
void ThreadSafeQueue<T1, T2>::Push(ArgTs &&... args) {
    {
        std::scoped_lock                    lock(_infoMutex); UNUSED(lock);

        if(_info.stopped)
            throw ThreadSafeQueueStoppedException();

        _info.queue.emplace(std::forward<ArgTs>(args)...);
    }

    _infoCV.notify_one();
}

template <typename T1, typename T2>
template <typename... ArgTs>
bool ThreadSafeQueue<T1, T2>::TryPush(ArgTs &&... args) {
    {
        std::unique_lock                    lock(_infoMutex, std::try_to_lock);

        if(!lock)
            return false;

        if(_info.stopped)
            throw ThreadSafeQueueStoppedException();

        _info.queue.emplace(std::forward<ArgTs>(args)...);
    }

    _infoCV.notify_one();

    return true;
}

template <typename T1, typename T2>
typename ThreadSafeQueue<T1, T2>::value_type ThreadSafeQueue<T1, T2>::Pop(QueuePopType type) {
    assert(type == QueuePopType::Blocking || type == QueuePopType::NonBlocking);

    optional_value_type                     result(PopImpl(type == QueuePopType::Blocking));

    if(!result)
        throw ThreadSafeQueueEmptyException();

    return std::move(*result);
}


template <typename T1, typename T2>
typename ThreadSafeQueue<T1, T2>::optional_value_type ThreadSafeQueue<T1, T2>::Pop(std::chrono::steady_clock::duration const &waitFor) {
    // Detect overflow
    std::chrono::steady_clock::time_point const         now(std::chrono::steady_clock::now());
    std::chrono::steady_clock::time_point const         waitUntil(now + waitFor);

    ENSURE_ARGUMENT(waitUntil, now <= waitUntil);

    return PopImpl(true, waitUntil);
}

template <typename T1, typename T2>
typename ThreadSafeQueue<T1, T2>::optional_value_type ThreadSafeQueue<T1, T2>::Pop(std::chrono::steady_clock::time_point const &waitUntil) {
    return PopImpl(true, waitUntil);
}

template <typename T1, typename T2>
typename ThreadSafeQueue<T1, T2>::optional_value_type ThreadSafeQueue<T1, T2>::TryPop(void) {
    return PopImpl(false);
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
template <typename T1, typename T2>
typename ThreadSafeQueue<T1, T2>::optional_value_type ThreadSafeQueue<T1, T2>::PopImpl(bool shouldWait, std::chrono::steady_clock::time_point const &waitUntil) {
    _activePops.Increment();
    FINALLY(_decrementActivePopsFunc);

    // See the comment below as to why this object is scoped as it is.
    FinalAction                             notifyAction;

    {
        std::unique_lock                    lock(_infoMutex);

        if(_info.queue.empty()) {
            if(shouldWait == false)
                return optional_value_type();

            if(
                _infoCV.wait_until(
                    lock,
                    waitUntil,
                    [this](void) {
                        return _info.queue.empty() == false || _info.stopped;
                    }
                ) == false
            )
                return optional_value_type();

            if(_info.queue.empty() && _info.stopped)
                throw ThreadSafeQueueStoppedException();
        }

        assert(_info.queue.empty() == false);

        value_type                          result(std::move(_info.queue.front()));

        _info.queue.pop();

        if(_info.queue.empty() == false) {
            // The notify has to happen after the lock is released, so
            // we could move the notify outside of this loop. However, that
            // means that we need to also move result outside the current scope (as
            // that is the function's return value). Moving result outside of
            // the current scope means that the result type must be default
            // constructible. Rather than requiring default construction, we
            // place the FinalAction outside of the scope which means it will
            // fire after the lock is released.
            notifyAction = _notifyOneFunc;
        }

        return std::move(result);
    }
}

} // namespace CommonHelpers
