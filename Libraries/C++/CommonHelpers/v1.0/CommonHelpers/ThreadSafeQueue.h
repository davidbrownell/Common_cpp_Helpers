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
    bool IsStopped(void) const;

    bool empty(void) const;
    size_t size(void) const;

    template <typename... ArgTs> void Push(ArgTs &&... args);
    template <typename... ArgTs> bool TryPush(ArgTs &&... args);

    value_type Pop(
        QueuePopType type=QueuePopType::NonBlocking
    );

    optional_value_type Pop(std::chrono::steady_clock::duration const &waitFor);

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
    ThreadSafeCounter                       _activePops;
    std::function<void (void)> const        _decrementActivePopsFunc;

    // Note that in all cases, _infoCV is notified within a lock. It should be possible to notify outside
    // of the lock, but doing so causes an access violation (AV) on Linux and Windows about 1 out of 200000
    // executions. Moving the notification inside the lock prevented these periodic AVs.
    mutable std::mutex                      _infoMutex;
    std::condition_variable                 _infoCV;
    Info                                    _info;

    // ----------------------------------------------------------------------
    // |
    // |  Private Methods
    // |
    // ----------------------------------------------------------------------
    optional_value_type PopImpl(
        bool shouldWait,
        // We can't use std::chrono::steady_clock::duration::max() here, as that value leads
        // to overflow on Linux. Use a very large value instead.
        std::chrono::steady_clock::duration const &waitFor=std::chrono::hours(24 * 365)
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
    _activePops(0),
    _decrementActivePopsFunc(
        [this](void) {
            _activePops.Decrement();
        }
    )
{}

template <typename T1, typename T2>
ThreadSafeQueue<T1, T2>::~ThreadSafeQueue(void) {
    Stop();
}

template <typename T1, typename T2>
void ThreadSafeQueue<T1, T2>::Stop(void) {
    {
        std::scoped_lock<decltype(_infoMutex)>          lock(_infoMutex); UNUSED(lock);

        if(_info.stopped)
            return;

        _info.stopped = true;

        // See declaration of _infoCV as to why this notification happens within the lock rather
        // than outside of it
        _infoCV.notify_all();
    }

    _activePops.wait_value(0);

#if (defined DEBUG)
    {
        // TODO: Eventually change _infoMutex to a `shared_mutex` with a `shared_lock` here (all
        //       other lock occurrences should be `unique_lock`)
        std::scoped_lock<decltype(_infoMutex)>          lock(_infoMutex); UNUSED(lock);

        assert(_info.queue.empty());
    }
#endif
}

template <typename T1, typename T2>
bool ThreadSafeQueue<T1, T2>::IsStopped(void) const {
    // TODO: Eventually change _infoMutex to a `shared_mutex` with a `shared_lock` here (all
    //       other lock occurrences should be `unique_lock`)
    std::scoped_lock<decltype(_infoMutex)>              lock(_infoMutex);

    return _info.stopped;
}

template <typename T1, typename T2>
bool ThreadSafeQueue<T1, T2>::empty(void) const {
    // TODO: Eventually change _infoMutex to a `shared_mutex` with a `shared_lock` here (all
    //       other lock occurrences should be `unique_lock`)
    std::scoped_lock<decltype(_infoMutex)>              lock(_infoMutex); UNUSED(lock);

    return _info.queue.empty();
}

template <typename T1, typename T2>
size_t ThreadSafeQueue<T1, T2>::size(void) const {
    // TODO: Eventually change _infoMutex to a `shared_mutex` with a `shared_lock` here (all
    //       other lock occurrences should be `unique_lock`)
    std::scoped_lock<decltype(_infoMutex)>              lock(_infoMutex); UNUSED(lock);

    return _info.queue.size();
}

template <typename T1, typename T2>
template <typename... ArgTs>
void ThreadSafeQueue<T1, T2>::Push(ArgTs &&... args) {
    {
        std::scoped_lock<decltype(_infoMutex)>          lock(_infoMutex); UNUSED(lock);

        if(_info.stopped)
            throw ThreadSafeQueueStoppedException();

        _info.queue.emplace(std::forward<ArgTs>(args)...);

        // See declaration of _infoCV as to why this notification happens within the lock rather
        // than outside of it
        _infoCV.notify_one();
    }
}

template <typename T1, typename T2>
template <typename... ArgTs>
bool ThreadSafeQueue<T1, T2>::TryPush(ArgTs &&... args) {
    {
        std::unique_lock<decltype(_infoMutex)>          lock(_infoMutex, std::try_to_lock);

        if(!lock)
            return false;

        if(_info.stopped)
            throw ThreadSafeQueueStoppedException();

        _info.queue.emplace(std::forward<ArgTs>(args)...);

        // See declaration of _infoCV as to why this notification happens within the lock rather
        // than outside of it
        _infoCV.notify_one();
    }

    return true;
}

namespace Details {

template <typename ResultT, typename OptionalResultT>
inline ResultT PopMove(OptionalResultT &optional, std::enable_if_t<std::is_same_v<ResultT, OptionalResultT>>* =nullptr) {
    return std::move(optional);
}

template <typename ResultT, typename OptionalResultT>
inline ResultT PopMove(OptionalResultT &optional, std::enable_if_t<std::is_same_v<ResultT, OptionalResultT> == false>* =nullptr) {
    return *optional;
}

} // namespace Details

template <typename T1, typename T2>
typename ThreadSafeQueue<T1, T2>::value_type ThreadSafeQueue<T1, T2>::Pop(QueuePopType type) {
    assert(type == QueuePopType::Blocking || type == QueuePopType::NonBlocking);

    optional_value_type                     result(PopImpl(type == QueuePopType::Blocking));

    if(!result)
        throw ThreadSafeQueueEmptyException();

    return Details::PopMove<value_type>(result);
}


template <typename T1, typename T2>
typename ThreadSafeQueue<T1, T2>::optional_value_type ThreadSafeQueue<T1, T2>::Pop(std::chrono::steady_clock::duration const &waitFor) {
    return PopImpl(true, waitFor);
}

template <typename T1, typename T2>
typename ThreadSafeQueue<T1, T2>::optional_value_type ThreadSafeQueue<T1, T2>::TryPop(void) {
    return PopImpl(false);
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
template <typename T1, typename T2>
typename ThreadSafeQueue<T1, T2>::optional_value_type ThreadSafeQueue<T1, T2>::PopImpl(bool shouldWait, std::chrono::steady_clock::duration const &waitFor) {
    _activePops.Increment();
    FINALLY(_decrementActivePopsFunc);

    std::unique_lock<decltype(_infoMutex)>              lock(_infoMutex);

    if(_info.queue.empty()) {
        if(shouldWait == false)
            return optional_value_type();

        _infoCV.wait_for(
            lock,
            waitFor,
            [this](void) {
                return _info.queue.empty() == false || _info.stopped;
            }
        );

        if(_info.queue.empty()) {
            if(_info.stopped)
                throw ThreadSafeQueueStoppedException();

            return optional_value_type();
        }
    }

    assert(_info.queue.empty() == false);

    value_type                              result(std::move(_info.queue.front()));

    _info.queue.pop();

    return std::move(result);
}

} // namespace CommonHelpers
