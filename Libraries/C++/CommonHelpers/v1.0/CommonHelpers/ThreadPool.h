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
#include "Misc.h"
#include "Move.h"
#include "TypeTraits.h"

#include <cassert>
#include <future>
#include <memory>
#include <thread>
#include <vector>

namespace CommonHelpers {
namespace Details {

/////////////////////////////////////////////////////////////////////////
///  \class         ThreadPoolQueueDoneException
///  \brief         Exception thrown when a ThreadPoolQueue is in the
///                 process of shutting down and no more work remains.
///
struct ThreadPoolQueueDoneException : public std::exception {};

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
    // |  Public Methods
    // |
    // ----------------------------------------------------------------------
    ThreadPoolImpl(void);
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
    std::future<ReturnT> enqueue_task(std::function<ReturnT (void)> functor);

    /////////////////////////////////////////////////////////////////////////
    ///  \fn            enqueue_task
    ///  \brief         Enqueues a single argument function that returns a value.
    ///                 The argument is true if the pool is in a started state and
    ///                 false if the pool is in the process of shutting down.
    ///
    template <typename ReturnT>
    std::future<ReturnT> enqueue_task(std::function<ReturnT (bool)> functor);

    template <typename CallableT>
    std::future<typename TypeTraits::FunctionTraits<CallableT>::return_type> enqueue_task(CallableT && callable);

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
    State                                   _state;
    Threads                                 _threads;

    // ----------------------------------------------------------------------
    // |
    // |  Private Methods
    // |
    // ----------------------------------------------------------------------
    template <typename CallableT> std::future<typename TypeTraits::FunctionTraits<CallableT>::return_type> EnqueueCallableTask(CallableT && callable, std::tuple<>);
    template <typename CallableT> std::future<typename TypeTraits::FunctionTraits<CallableT>::return_type> EnqueueCallableTask(CallableT && callable, std::tuple<bool>);
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
    SimpleThreadPool(size_t numThreads=std::thread::hardware_concurrency());
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
    std::function<void (void)> GetWork(size_t threadIndex);

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
        size_t workerIterations=2
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
    std::function<void (void)> GetWork(size_t threadIndex);

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
// |  Details::ThreadPoolImpl
// |
// ----------------------------------------------------------------------
template <typename SuperT>
Details::ThreadPoolImpl<SuperT>::ThreadPoolImpl(void) :
    _state(State::Initializing)
{}

template <typename SuperT>
Details::ThreadPoolImpl<SuperT>::~ThreadPoolImpl(void) {
    assert(_state == State::Stopped);
}

template <typename SuperT>
void Details::ThreadPoolImpl<SuperT>::enqueue_work(std::function<void (void)> functor) {
    ENSURE_ARGUMENT(functor);

    assert(_state == State::Started);
    static_cast<SuperT &>(*this).AddWork(std::move(functor));
}

template <typename SuperT>
void Details::ThreadPoolImpl<SuperT>::enqueue_work(std::function<void (bool)> functor) {
    ENSURE_ARGUMENT(functor);

    this->enqueue_work(
        [this, f=std::move(functor)](void) {
            f(_state == State::Started);
        }
    );
}

template <typename SuperT>
template <typename ReturnT>
std::future<ReturnT> Details::ThreadPoolImpl<SuperT>::enqueue_task(std::function<ReturnT (void)> functor) {
    // ----------------------------------------------------------------------
    using Promise                           = std::promise<ReturnT>;
    using Future                            = std::future<ReturnT>;
    // ----------------------------------------------------------------------

    ENSURE_ARGUMENT(functor);

    assert(_state == State::Started);

    // Note that the promise needs to be a shared_ptr to work around copy-related
    // issues when using the promise within the lambda below.
    std::shared_ptr<Promise>                pPromise(std::make_shared<Promise>());
    Future                                  future(pPromise->get_future());

    static_cast<SuperT &>(*this).enqueue_work(
        [pPromise, f=std::move(functor)](void) {
            pPromise->set_value(f());
        }
    );

    return future;
}

template <typename SuperT>
template <typename ReturnT>
std::future<ReturnT> Details::ThreadPoolImpl<SuperT>::enqueue_task(std::function<ReturnT (bool)> functor) {
    ENSURE_ARGUMENT(functor);

    return this->enqueue_task(
        [this, f=std::move(functor)](void) {
            return f(_state == State::Started);
        }
    );
}

template <typename SuperT>
template <typename CallableT>
std::future<typename TypeTraits::FunctionTraits<CallableT>::return_type> Details::ThreadPoolImpl<SuperT>::enqueue_task(CallableT && callable) {
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

    std::mutex                              initMutex;
    std::condition_variable                 initCondition;
    size_t                                  initRemaining(numThreads);

    auto const                              worker(
        [this, &initMutex, &initCondition, &initRemaining](size_t threadIndex) {
            // Indicate that the thread has started
            {
                std::unique_lock            lock(initMutex); UNUSED(lock);

                assert(initRemaining);
                --initRemaining;
            }

            initCondition.notify_one();

            for(;;) {
                try {
                    static_cast<SuperT &>(*this).GetWork(threadIndex)();
                }
                catch(Details::ThreadPoolQueueDoneException const &) {
                    break;
                }
            }
        }
    );

    Threads                                 threads;

    while(threads.size() < numThreads)
        threads.emplace_back(worker, threads.size());

    // Wait for the threads to begin
    {
        std::unique_lock                    lock(initMutex);

        while(initRemaining != 0)
            initCondition.wait(lock);
    }

    _threads = std::move(threads);
    _state = State::Started;
}

template <typename SuperT>
void Details::ThreadPoolImpl<SuperT>::Stop(void) {
    assert(_state == State::Started);
    _state = State::ShuttingDown;

    static_cast<SuperT &>(*this).StopQueues();

    for(auto & thread : _threads)
        thread.join();

    _state = State::Stopped;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
template <typename SuperT>
template <typename CallableT>
std::future<typename TypeTraits::FunctionTraits<CallableT>::return_type> Details::ThreadPoolImpl<SuperT>::EnqueueCallableTask(CallableT && callable, std::tuple<>) {
    return enqueue_task(
        std::function<typename TypeTraits::FunctionTraits<CallableT>::return_type ()>(
            std::forward<CallableT>(callable)
        )
    );
}

template <typename SuperT>
template <typename CallableT>
std::future<typename TypeTraits::FunctionTraits<CallableT>::return_type> Details::ThreadPoolImpl<SuperT>::EnqueueCallableTask(CallableT && callable, std::tuple<bool>) {
    return enqueue_task(
        std::function<typename TypeTraits::FunctionTraits<CallableT>::return_type (bool)>(
            std::forward<CallableT>(callable)
        )
    );
}

} // namespace CommonHelpers
