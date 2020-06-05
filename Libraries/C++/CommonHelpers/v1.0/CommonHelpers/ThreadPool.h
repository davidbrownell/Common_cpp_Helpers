/////////////////////////////////////////////////////////////////////////
///
///  \file          ThreadPool.h
///  \brief         Contains the SimpleThreadPool and ComplexThreadPool objects
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-05-30 22:08:54
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
#include "Finally.h"
#include "Misc.h"
#include "Move.h"
#include "ThreadSafeCounter.h"
#include "TypeTraits.h"

#include <cassert>
#include <future>
#include <iostream>
#include <memory>
#include <optional>
#include <thread>
#include <vector>

namespace CommonHelpers {
namespace Details {

// ----------------------------------------------------------------------
// |  Forward Declarations
template <typename SuperT>
class ThreadPoolImpl;

} // namespace Details

/////////////////////////////////////////////////////////////////////////
///  \class         ThreadPoolFuture
///  \brief         Object with the same interface as `std::future` that is
///                 re-etrant while waiting, ensuring that we don't run into
///                 scenarios where enqueued tasks can never execute because
///                 threads are servicing other work waiting on enqueued tasks
///                 of their own.
///
template <typename T>
class ThreadPoolFuture {
public:
    // ----------------------------------------------------------------------
    // |
    // |  Public Types
    // |
    // ----------------------------------------------------------------------
    using DoWorkFunction                = std::function<void (std::chrono::steady_clock::duration const &)>;

private:
    // ----------------------------------------------------------------------
    // |
    // |  Private Data (used in public declarations)
    // |
    // ----------------------------------------------------------------------
    DoWorkFunction const                _doWorkFunc;
    std::future<T>                      _future;

public:
    // ----------------------------------------------------------------------
    // |
    // |  Public Methods
    // |
    // ----------------------------------------------------------------------
    NON_COPYABLE(ThreadPoolFuture);
    MOVE(ThreadPoolFuture, _doWorkFunc, _future);

    ~ThreadPoolFuture(void) = default;

    T get(void);

    void wait(void) const;
    template <typename RepT, typename PeriodT> std::future_status wait_for(std::chrono::duration<RepT, PeriodT> const &timeout) const;
    template <typename ClockT, typename DurationT> std::future_status wait_until(std::chrono::time_point<ClockT, DurationT> const &timeout) const;

private:
    // ----------------------------------------------------------------------
    // |  Relationships
    template <typename SuperT> friend class Details::ThreadPoolImpl;

    // ----------------------------------------------------------------------
    // |
    // |  Private Methods
    // |
    // ----------------------------------------------------------------------
    ThreadPoolFuture(DoWorkFunction function, std::future<T> future);
};

namespace Details {

/////////////////////////////////////////////////////////////////////////
///  \class         ThreadPoolQueueDoneException
///  \brief         Exception thrown when a ThreadPoolQueue is in the
///                 process of shutting down and no more work remains.
///
struct ThreadPoolQueueDoneException : public std::exception {};

constexpr size_t const                      MAIN_THREAD = std::numeric_limits<size_t>::max();

// ----------------------------------------------------------------------
// |  Forward Declarations (Defined in ThreadPool.cpp)
class ThreadPoolQueue;

/////////////////////////////////////////////////////////////////////////
///  \class         ThreadPoolImpl
///  \brief         Implements functionality common to all thread pool types.
///
template <typename SuperT>
class ThreadPoolImpl {
public:
    // ----------------------------------------------------------------------
    // |
    // |  Public Types
    // |
    // ----------------------------------------------------------------------
    using OnExceptionCallback               = std::function<void (size_t, std::exception const &)>; // Return true to terminate the worker thread

    // ----------------------------------------------------------------------
    // |
    // |  Public Methods
    // |
    // ----------------------------------------------------------------------
    ThreadPoolImpl(std::optional<OnExceptionCallback> onException=std::nullopt);
    ~ThreadPoolImpl(void);

    NON_COPYABLE(ThreadPoolImpl);
    NON_MOVABLE(ThreadPoolImpl);

    /////////////////////////////////////////////////////////////////////////
    ///  \fn            enqueue_work
    ///  \brief         Enqueues a functor for execution.
    ///
    void enqueue_work(std::function<void (void)> functor);

    /////////////////////////////////////////////////////////////////////////
    ///  \fn            enqueue_work
    ///  \brief         Enqueues a single argument functor for execution. The
    ///                 argument is true if the pool is in a started state and
    ///                 false if the pool is in the process of shutting down.
    ///
    void enqueue_work(std::function<void (bool)> functor);

    /////////////////////////////////////////////////////////////////////////
    ///  \fn            enqueue_task
    ///  \brief         Enqueues a function that returns a value via a future.
    ///
    template <typename ReturnT>
    ThreadPoolFuture<ReturnT> enqueue_task(std::function<ReturnT (void)> functor);

    /////////////////////////////////////////////////////////////////////////
    ///  \fn            enqueue_task
    ///  \brief         Enqueues a single argument function that returns a value.
    ///                 The argument is true if the pool is in a started state and
    ///                 false if the pool is in the process of shutting down.
    ///
    template <typename ReturnT>
    ThreadPoolFuture<ReturnT> enqueue_task(std::function<ReturnT (bool)> functor);

    template <typename CallableT>
    ThreadPoolFuture<typename TypeTraits::FunctionTraits<CallableT>::return_type> enqueue_task(CallableT && callable);

protected:
    // ----------------------------------------------------------------------
    // |
    // |  Protected Types
    // |
    // ----------------------------------------------------------------------
    using ThreadPoolQueueUniquePtr          = std::unique_ptr<Details::ThreadPoolQueue>;

    // ----------------------------------------------------------------------
    // |
    // |  Protected Methods
    // |
    // ----------------------------------------------------------------------
    void Start(size_t numThreads);
    void Stop(void);

private:
    // ----------------------------------------------------------------------
    // |
    // |  Private Types
    // |
    // ----------------------------------------------------------------------
    using Threads                           = std::vector<std::thread>;

    enum class State {
        Initializing,
        Started,
        ShuttingDown,
        Stopped
    };

    // ----------------------------------------------------------------------
    // |
    // |  Private Data
    // |
    // ----------------------------------------------------------------------
    OnExceptionCallback const               _onExceptionCallback;
    State                                   _state;
    Threads                                 _threads;

    static thread_local size_t              _threadIndex;

    ThreadSafeCounter                       _activeWork;

    // ----------------------------------------------------------------------
    // |
    // |  Private Methods
    // |
    // ----------------------------------------------------------------------
    template <typename CallableT> ThreadPoolFuture<typename TypeTraits::FunctionTraits<CallableT>::return_type> EnqueueCallableTask(CallableT && callable, std::tuple<>);
    template <typename CallableT> ThreadPoolFuture<typename TypeTraits::FunctionTraits<CallableT>::return_type> EnqueueCallableTask(CallableT && callable, std::tuple<bool>);

    inline bool DoWork(std::chrono::steady_clock::duration const &timeout);
};

} // namespace Details

/////////////////////////////////////////////////////////////////////////
///  \class         SimpleThreadPool
///  \brief         Standard thread pool that manages work via a single queue.
///
class SimpleThreadPool : public Details::ThreadPoolImpl<SimpleThreadPool> {
public:
    // ----------------------------------------------------------------------
    // |
    // |  Public Types
    // |
    // ----------------------------------------------------------------------
    using BaseType                          = Details::ThreadPoolImpl<SimpleThreadPool>;

    // ----------------------------------------------------------------------
    // |
    // |  Public Methods
    // |
    // ----------------------------------------------------------------------
    SimpleThreadPool(
        size_t numThreads=std::thread::hardware_concurrency(),
        std::optional<OnExceptionCallback> onExceptionCallback=std::nullopt
    );
    ~SimpleThreadPool(void);

    NON_COPYABLE(SimpleThreadPool);
    NON_MOVABLE(SimpleThreadPool);

private:
    // ----------------------------------------------------------------------
    // |  Relationships
    friend BaseType;

    // ----------------------------------------------------------------------
    // |
    // |  Private Data
    // |
    // ----------------------------------------------------------------------
    typename BaseType::ThreadPoolQueueUniquePtr         _pQueue;

    // ----------------------------------------------------------------------
    // |
    // |  Private Methods
    // |
    // ----------------------------------------------------------------------
    void AddWork(std::function<void (void)> functor);
    std::function<void (void)> GetWork(size_t threadIndex, std::chrono::steady_clock::duration duration);

    void StopQueues(void);
};

/////////////////////////////////////////////////////////////////////////
///  \class         ComplexThreadPool
///  \brief         Thread pool that spreads work across multiple queues
///                 to decrease contention when adding and checking for work.
///                 This thread pool is much more efficient in some scenarios.
///
///                 This class is based on code by Martin Vorbrodt, licensed
///                 under the Zero Clause BSD license.
///
///                     https://github.com/mvorbrodt/blog/blob/master/LICENSE
///                     https://github.com/mvorbrodt/blog/blob/master/src/pool.hpp
///
class ComplexThreadPool : public Details::ThreadPoolImpl<ComplexThreadPool> {
public:
    // ----------------------------------------------------------------------
    // |
    // |  Public Types
    // |
    // ----------------------------------------------------------------------
    using BaseType                          = Details::ThreadPoolImpl<ComplexThreadPool>;

    // ----------------------------------------------------------------------
    // |
    // |  Public Methods
    // |
    // ----------------------------------------------------------------------
    ComplexThreadPool(
        size_t numThreads=std::thread::hardware_concurrency(),
        size_t workerIterations=2,
        std::optional<OnExceptionCallback> onExceptionCallback=std::nullopt
    );
    ~ComplexThreadPool(void);

    NON_COPYABLE(ComplexThreadPool);
    NON_MOVABLE(ComplexThreadPool);

private:
    // ----------------------------------------------------------------------
    // |  Relationships
    friend BaseType;

    // ----------------------------------------------------------------------
    // |
    // |  Private Types
    // |
    // ----------------------------------------------------------------------
    using ThreadPoolQueueUniquePtrs         = std::vector<typename BaseType::ThreadPoolQueueUniquePtr>;

    // ----------------------------------------------------------------------
    // |
    // |  Private Data
    // |
    // ----------------------------------------------------------------------
    size_t const                            _numTries;

    ThreadPoolQueueUniquePtrs               _queues;
    std::atomic_uint                        _currentEnqueueIndex;

    // ----------------------------------------------------------------------
    // |
    // |  Private Methods
    // |
    // ----------------------------------------------------------------------
    void AddWork(std::function<void (void)> functor);
    std::function<void (void)> GetWork(size_t threadIndex, std::chrono::steady_clock::duration duration);

    void StopQueues(void);
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
// |  ThreadPoolFuture
// |
// ----------------------------------------------------------------------
template <typename T>
T ThreadPoolFuture<T>::get(void) {
    wait();
    return _future.get();
}

template <typename T>
void ThreadPoolFuture<T>::wait(void) const {
    wait_until(std::chrono::steady_clock::time_point::max());
}

template <typename T>
template <typename RepT, typename PeriodT>
std::future_status ThreadPoolFuture<T>::wait_for(std::chrono::duration<RepT, PeriodT> const &timeout) const {
    return wait_until(std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout));
}

template <typename T>
template <typename ClockT, typename DurationT>
std::future_status ThreadPoolFuture<T>::wait_until(std::chrono::time_point<ClockT, DurationT> const &timeout) const {
    // ----------------------------------------------------------------------
    using Clock                             = typename std::chrono::time_point<ClockT, DurationT>::clock;
    // ----------------------------------------------------------------------

    std::chrono::steady_clock::duration const           zero(std::chrono::milliseconds(0));
    std::chrono::steady_clock::duration const           waitDuration(std::chrono::milliseconds(250));

    for(;;) {
        if(_future.wait_for(zero) == std::future_status::ready)
            break;

        if(Clock::now() >= timeout)
            return std::future_status::timeout;

        _doWorkFunc(waitDuration);
    }

    return std::future_status::ready;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
template <typename T>
ThreadPoolFuture<T>::ThreadPoolFuture(DoWorkFunction function, std::future<T> future) :
    _doWorkFunc(
        std::move(
            [&function](void) -> DoWorkFunction & {
                assert(function);
                return function;
            }()
        )
    ),
    _future(
        std::move(
            [&future](void) -> std::future<T> & {
                assert(future.valid());
                return future;
            }()
        )
    )
{}

// ----------------------------------------------------------------------
// |
// |  Details::ThreadPoolImpl
// |
// ----------------------------------------------------------------------
template <typename SuperT>
Details::ThreadPoolImpl<SuperT>::ThreadPoolImpl(std::optional<OnExceptionCallback> onException/*=std::nullopt*/) :
    _onExceptionCallback(
        [&onException](void) {
            if(onException)
                return *onException;

            return OnExceptionCallback(
                [](size_t threadIndex, std::exception const &ex) {
#if (!defined THREAD_POOL_NO_DEFAULT_EXCEPTION_OUTPUT)
                    std::cerr <<
                        "Uncaught exception in ThreadPool\n"
                        "  Thread Index: " << threadIndex << "\n"
                        "  Exception:    " << typeid(ex).name() << "\n"
                        "  Message:      " << ex.what() << "\n"
                        "\n"
                    ;
#endif
                }
            );
        }()
    ),
    _state(State::Initializing),
    _activeWork(0)
{}

template <typename SuperT>
Details::ThreadPoolImpl<SuperT>::~ThreadPoolImpl(void) {
    assert(_state == State::Stopped);
}

template <typename SuperT>
void Details::ThreadPoolImpl<SuperT>::enqueue_work(std::function<void (void)> functor) {
    ENSURE_ARGUMENT(functor);
    assert(_state != State::Initializing && _state != State::Stopped);

    _activeWork.Increment();

    try {
        static_cast<SuperT &>(*this).AddWork(std::move(functor));
    }
    catch(...) {
        _activeWork.Decrement();
        throw;
    }
}

template <typename SuperT>
void Details::ThreadPoolImpl<SuperT>::enqueue_work(std::function<void (bool)> functor) {
    ENSURE_ARGUMENT(functor);
    assert(_state != State::Initializing && _state != State::Stopped);

    this->enqueue_work(
        [this, f=std::move(functor)](void) {
            f(_state == State::Started);
        }
    );
}

template <typename SuperT>
template <typename ReturnT>
ThreadPoolFuture<ReturnT> Details::ThreadPoolImpl<SuperT>::enqueue_task(std::function<ReturnT (void)> functor) {
    // ----------------------------------------------------------------------
    using Promise                           = std::promise<ReturnT>;
    using Future                            = std::future<ReturnT>;
    // ----------------------------------------------------------------------

    ENSURE_ARGUMENT(functor);
    assert(_state != State::Initializing && _state != State::Stopped);

    // Note that the promise needs to be a shared_ptr to work around copy-related
    // issues when using the promise within the lambda below.
    std::shared_ptr<Promise>                pPromise(std::make_shared<Promise>());
    Future                                  future(pPromise->get_future());

    static_cast<SuperT &>(*this).enqueue_work(
        [pPromise=std::move(pPromise), f=std::move(functor)](void) {
            pPromise->set_value(f());
        }
    );

    return ThreadPoolFuture<ReturnT>(
        [this](std::chrono::steady_clock::duration const &timeout) {
            DoWork(timeout);
        },
        std::move(future)
    );
}

template <typename SuperT>
template <typename ReturnT>
ThreadPoolFuture<ReturnT> Details::ThreadPoolImpl<SuperT>::enqueue_task(std::function<ReturnT (bool)> functor) {
    ENSURE_ARGUMENT(functor);
    assert(_state != State::Initializing && _state != State::Stopped);

    return this->enqueue_task(
        [this, f=std::move(functor)](void) {
            return f(_state == State::Started);
        }
    );
}

template <typename SuperT>
template <typename CallableT>
ThreadPoolFuture<typename TypeTraits::FunctionTraits<CallableT>::return_type> Details::ThreadPoolImpl<SuperT>::enqueue_task(CallableT && callable) {
    assert(_state != State::Initializing && _state != State::Stopped);

    return EnqueueCallableTask(
        std::forward<CallableT>(callable),
        typename TypeTraits::FunctionTraits<CallableT>::args()
    );
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
template <typename SuperT>
void Details::ThreadPoolImpl<SuperT>::Start(size_t numThreads) {
    assert(numThreads);
    assert(_state == State::Initializing);

    ThreadSafeCounter                       initRemaining(static_cast<ThreadSafeCounter::value_type>(numThreads));

    auto const                              worker(
        [this, &initRemaining](size_t threadIndex) {
            // Indicate that the thread has started
            initRemaining.Decrement();

            // This value is used in other methods and must be set here
            _threadIndex = threadIndex;

            std::chrono::steady_clock::duration const   maxDuration(std::chrono::steady_clock::duration::max());

            for(;;) {
                if(DoWork(maxDuration) == false)
                    break;
            }
        }
    );

    Threads                                 threads;

    while(threads.size() < numThreads)
        threads.emplace_back(worker, threads.size());

    // Wait for the threads to initialize
    initRemaining.wait(0);

    _threads = std::move(threads);
    _state = State::Started;
}

template <typename SuperT>
void Details::ThreadPoolImpl<SuperT>::Stop(void) {
    assert(_state == State::Started);
    _state = State::ShuttingDown;

    _activeWork.wait(0);
    static_cast<SuperT &>(*this).StopQueues();

    for(auto & thread : _threads)
        thread.join();

    _state = State::Stopped;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
template <typename SuperT>
// static
thread_local size_t Details::ThreadPoolImpl<SuperT>::_threadIndex = MAIN_THREAD;

template <typename SuperT>
template <typename CallableT>
ThreadPoolFuture<typename TypeTraits::FunctionTraits<CallableT>::return_type> Details::ThreadPoolImpl<SuperT>::EnqueueCallableTask(CallableT && callable, std::tuple<>) {
    return enqueue_task(
        std::function<typename TypeTraits::FunctionTraits<CallableT>::return_type ()>(
            std::forward<CallableT>(callable)
        )
    );
}

template <typename SuperT>
template <typename CallableT>
ThreadPoolFuture<typename TypeTraits::FunctionTraits<CallableT>::return_type> Details::ThreadPoolImpl<SuperT>::EnqueueCallableTask(CallableT && callable, std::tuple<bool>) {
    return enqueue_task(
        std::function<typename TypeTraits::FunctionTraits<CallableT>::return_type (bool)>(
            std::forward<CallableT>(callable)
        )
    );
}

template <typename SuperT>
inline bool Details::ThreadPoolImpl<SuperT>::DoWork(std::chrono::steady_clock::duration const &timeout) {
    assert(_threadIndex != MAIN_THREAD);

    try {
        std::function<void (void)>          func(static_cast<SuperT &>(*this).GetWork(_threadIndex, timeout));

        if(func) {
            FINALLY([this](void) { _activeWork.Decrement(); });
            func();
        }
    }
    catch(Details::ThreadPoolQueueDoneException const &) {
        return false;
    }
    catch(std::exception const &ex) {
        _onExceptionCallback(_threadIndex, ex);
    }

    return true;
}

} // namespace CommonHelpers
