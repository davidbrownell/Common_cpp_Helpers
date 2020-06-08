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
    using YieldFunction                     = std::function<void (void)>;

private:
    // ----------------------------------------------------------------------
    // |
    // |  Private Data (used in public declarations)
    // |
    // ----------------------------------------------------------------------
    YieldFunction const                     _yieldFunc;
    std::future<T>                          _future;

public:
    // ----------------------------------------------------------------------
    // |
    // |  Public Methods
    // |
    // ----------------------------------------------------------------------
    NON_COPYABLE(ThreadPoolFuture);

#define ARGS                                MEMBERS(_yieldFunc, _future)

    MOVE(ThreadPoolFuture, ARGS);

#undef ARGS

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
    ThreadPoolFuture(YieldFunction function, std::future<T> future);
};

namespace Details {

/////////////////////////////////////////////////////////////////////////
///  \class         ThreadPoolQueueDoneException
///  \brief         Exception thrown when a ThreadPoolQueue is in the
///                 process of shutting down and no more work remains.
///
struct ThreadPoolQueueDoneException : public std::exception {};

static constexpr std::chrono::steady_clock::duration const                  sg_zeroDuration(std::chrono::milliseconds(0));

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
    using OnExceptionCallback               = std::function<void (size_t)>;

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
    ///  \fn            enqueue
    ///  \brief         Enqueues work to be executed at a later time. Callable can be:
    ///
    ///                     void Func(void);
    ///                     void Func(bool);
    ///
    ///                 Where the bool value passed as the first argument is true
    ///                 if the task is being executed normally and false if the
    ///                 pool is in the process of shutting down.
    ///
    template <typename CallableT> void enqueue(CallableT && callable, std::enable_if_t<std::is_same_v<void, typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type>>* =nullptr);

    /////////////////////////////////////////////////////////////////////////
    ///  \fn            enqueue
    ///  \brief         Enqueues a teat to be executed at a later time. Callback can be:
    ///
    ///                     <Some Value> Func(void);
    ///                     <Some Value> Func(bool);
    ///
    ///                 Where the bool value passed as the first argument is true
    ///                 if the task is being executed normally and false if the
    ///                 pool is in the process of shutting down.
    ///
    template <typename CallableT> ThreadPoolFuture<typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type> enqueue(CallableT && callable, std::enable_if_t<std::is_same_v<void, typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type> == false>* =nullptr);

    /////////////////////////////////////////////////////////////////////////
    ///  \fn            yield
    ///  \brief         Prevent thread starvation by performing enqueued work.
    ///
    void yield(void);

    /////////////////////////////////////////////////////////////////////////
    ///  \fn            parallel
    ///  \brief         Executes one or more  work items. Callable can be:
    ///
    ///                     <Some Value> Func(InputT const &);
    ///                     <Some Value> Func(bool, InputT const &);
    ///
    ///                 Where the bool value passed as the first argument is true
    ///                 if the task is being executed normally and false if the
    ///                 pool is in the process of shutting down.
    ///
    template <typename InputT, typename CallableT> typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type parallel(InputT const &input, CallableT && callable);
    template <typename InputT, typename CallableT> void parallel(std::vector<InputT> const &inputs, CallableT && callable, std::enable_if_t<std::is_same_v<void, typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type>>* =nullptr);
    template <typename InputT, typename CallableT> std::vector<typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type> parallel(std::vector<InputT> const &inputs, CallableT && callable, std::enable_if_t<std::is_same_v<void, typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type> == false>* =nullptr);
    template <typename InputIteratorT, typename CallableT> void parallel(InputIteratorT begin, InputIteratorT end, CallableT && callable, std::enable_if_t<std::is_same_v<void, typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type>>* =nullptr);
    template <typename InputIteratorT, typename CallableT> std::vector<typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type> parallel(InputIteratorT begin, InputIteratorT end, CallableT && callable, std::enable_if_t<std::is_same_v<void, typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type> == false>* =nullptr);

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
    std::atomic<State>                      _state;
    Threads                                 _threads;

    static thread_local size_t              _threadIndex;

    ThreadSafeCounter                       _activeWork;

    // ----------------------------------------------------------------------
    // |
    // |  Private Methods
    // |
    // ----------------------------------------------------------------------
    template <typename CallableT> void EnqueueWork(CallableT && callable, std::true_type /*is_empty_arg*/);
    template <typename CallableT> void EnqueueWork(CallableT && callable, std::false_type /*is_empty_arg*/);

    template <typename CallableT> ThreadPoolFuture<typename TypeTraits::FunctionTraits<CallableT>::return_type> EnqueueTask(CallableT && callable, std::true_type /*is_empty_arg*/);
    template <typename CallableT> ThreadPoolFuture<typename TypeTraits::FunctionTraits<CallableT>::return_type> EnqueueTask(CallableT && callable, std::false_type /*is_empty_arg*/);

    template <typename InputT, typename CallableT, typename IsSingleArgT> void ParallelSingle(InputT const &input, CallableT && callable, std::true_type const &isVoidReturn, IsSingleArgT const &isSingleArg);
    template <typename InputT, typename CallableT, typename IsSingleArgT> typename TypeTraits::FunctionTraits<CallableT>::return_type ParallelSingle(InputT const &input, CallableT && callable, std::false_type const &isVoidReturn, IsSingleArgT const &isSingleArg);

    template <typename InputIteratorT, typename CallableT> void ParallelMultiple(InputIteratorT begin, InputIteratorT end, CallableT && callable, std::true_type /*is_void_result*/, std::true_type /*is_single_arg*/);
    template <typename InputIteratorT, typename CallableT> void ParallelMultiple(InputIteratorT begin, InputIteratorT end, CallableT && callable, std::true_type /*is_void_result*/, std::false_type /*is_single_arg*/);
    template <typename InputIteratorT, typename CallableT> std::vector<typename TypeTraits::FunctionTraits<CallableT>::return_type> ParallelMultiple(InputIteratorT begin, InputIteratorT end, CallableT && callable, std::false_type /*is_void_result*/, std::true_type /*is_single_arg*/);
    template <typename InputIteratorT, typename CallableT> std::vector<typename TypeTraits::FunctionTraits<CallableT>::return_type> ParallelMultiple(InputIteratorT begin, InputIteratorT end, CallableT && callable, std::false_type /*is_void_result*/, std::false_type /*is_single_arg*/);

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
    std::chrono::steady_clock::time_point const         now(std::chrono::steady_clock::now());
    std::chrono::steady_clock::time_point const         waitUntil(now + std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout));

    ENSURE_ARGUMENT(timeout, now <= waitUntil);

    return wait_until(waitUntil);
}

template <typename T>
template <typename ClockT, typename DurationT>
std::future_status ThreadPoolFuture<T>::wait_until(std::chrono::time_point<ClockT, DurationT> const &timeout) const {
    // ----------------------------------------------------------------------
    using Clock                             = typename std::chrono::time_point<ClockT, DurationT>::clock;
    // ----------------------------------------------------------------------

    for(;;) {
        if(_future.wait_for(Details::sg_zeroDuration) == std::future_status::ready)
            break;

        if(Clock::now() >= timeout)
            return std::future_status::timeout;

        _yieldFunc();
    }

    return std::future_status::ready;
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
template <typename T>
ThreadPoolFuture<T>::ThreadPoolFuture(YieldFunction function, std::future<T> future) :
    _yieldFunc(
        std::move(
            [&function](void) -> YieldFunction & {
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
                [](size_t threadIndex) {
#if (!defined THREAD_POOL_NO_DEFAULT_EXCEPTION_OUTPUT)
                    try {
                        throw;
                    }
                    catch(std::exception const &ex) {
                        std::cerr <<
                            "Uncaught exception in ThreadPool\n"
                            "  Thread Index: " << threadIndex << "\n"
                            "  Exception:    " << typeid(ex).name() << "\n"
                            "  Message:      " << ex.what() << "\n"
                            "\n"
                        ;
                    }
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
    assert(_state.load() == State::Stopped);
}

template <typename SuperT>
template <typename CallableT>
void Details::ThreadPoolImpl<SuperT>::enqueue(CallableT && callable, std::enable_if_t<std::is_same_v<void, typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type>>*) {
    // ----------------------------------------------------------------------
    using Args                              = typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::args;
    // ----------------------------------------------------------------------

    EnqueueWork(
        std::forward<CallableT>(callable),
        std::integral_constant<bool, std::tuple_size_v<Args> == 0>()
    );
}

template <typename SuperT>
template <typename CallableT>
ThreadPoolFuture<typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type> Details::ThreadPoolImpl<SuperT>::enqueue(CallableT && callable, std::enable_if_t<std::is_same_v<void, typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type> == false>*) {
    // ----------------------------------------------------------------------
    using Args                              = typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::args;
    // ----------------------------------------------------------------------

    return EnqueueTask(
        std::forward<CallableT>(callable),
        std::integral_constant<bool, std::tuple_size_v<Args> == 0>()
    );
}

template <typename SuperT>
void Details::ThreadPoolImpl<SuperT>::yield(void) {
    DoWork(sg_zeroDuration);
}

template <typename SuperT>
template <typename InputT, typename CallableT>
typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type Details::ThreadPoolImpl<SuperT>::parallel(InputT const &input, CallableT && callable) {
    // ----------------------------------------------------------------------
    using ReturnType                        = typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type;
    using Args                              = typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::args;
    // ----------------------------------------------------------------------

    return ParallelSingle(
        input,
        std::forward<CallableT>(callable),
        std::integral_constant<bool, std::is_same_v<void, ReturnType>>(),
        std::integral_constant<bool, std::tuple_size_v<Args> == 1>()
    );
}

template <typename SuperT>
template <typename InputT, typename CallableT>
void Details::ThreadPoolImpl<SuperT>::parallel(std::vector<InputT> const &inputs, CallableT && callable, std::enable_if_t<std::is_same_v<void, typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type>>*) {
    parallel(inputs.cbegin(), inputs.cend(), std::forward<CallableT>(callable));
}

template <typename SuperT>
template <typename InputT, typename CallableT>
std::vector<typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type> Details::ThreadPoolImpl<SuperT>::parallel(std::vector<InputT> const &inputs, CallableT && callable, std::enable_if_t<std::is_same_v<void, typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type> == false>*) {
    return parallel(inputs.cbegin(), inputs.cend(), std::forward<CallableT>(callable));
}

template <typename SuperT>
template <typename InputIteratorT, typename CallableT>
void Details::ThreadPoolImpl<SuperT>::parallel(InputIteratorT begin, InputIteratorT end, CallableT && callable, std::enable_if_t<std::is_same_v<void, typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type>>*) {
    // ----------------------------------------------------------------------
    using Args                              = typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::args;
    // ----------------------------------------------------------------------

    ParallelMultiple(
        begin,
        end,
        std::forward<CallableT>(callable),
        std::true_type(),
        std::integral_constant<bool, std::tuple_size_v<Args> == 1>()
    );
}

template <typename SuperT>
template <typename InputIteratorT, typename CallableT>
std::vector<typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type> Details::ThreadPoolImpl<SuperT>::parallel(InputIteratorT begin, InputIteratorT end, CallableT && callable, std::enable_if_t<std::is_same_v<void, typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::return_type> == false>*) {
    // ----------------------------------------------------------------------
    using Args                              = typename TypeTraits::FunctionTraits<std::decay_t<CallableT>>::args;
    // ----------------------------------------------------------------------

    return ParallelMultiple(
        begin,
        end,
        std::forward<CallableT>(callable),
        std::false_type(),
        std::integral_constant<bool, std::tuple_size_v<Args> == 1>()
    );
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
template <typename SuperT>
void Details::ThreadPoolImpl<SuperT>::Start(size_t numThreads) {
    assert(numThreads);
    assert(_state.load() == State::Initializing);

    ThreadSafeCounter                       initRemaining(static_cast<ThreadSafeCounter::value_type>(numThreads));

    auto const                              worker(
        [this, &initRemaining](size_t threadIndex) {
            // Indicate that the thread has started
            initRemaining.Decrement();

            // This value is used in other methods and must be set here
            _threadIndex = threadIndex;

            // We can't use std::chrono::steady_clock::duration::max() here, as
            // the implementation of wait_for will internally call wait_until with
            // the current time plus this duration; this will cause an overflow
            // when using duration::max. Use a really big value instead of duration::max.
            std::chrono::steady_clock::duration const   maxDuration(std::chrono::hours(24 * 365));

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
    initRemaining.wait_until(0);

    _threads = std::move(threads);
    _state = State::Started;
}

template <typename SuperT>
void Details::ThreadPoolImpl<SuperT>::Stop(void) {
    assert(_state.load() == State::Started);
    _state = State::ShuttingDown;

    _activeWork.wait_until(0);
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
thread_local size_t Details::ThreadPoolImpl<SuperT>::_threadIndex = 0;

template <typename SuperT>
template <typename CallableT>
void Details::ThreadPoolImpl<SuperT>::EnqueueWork(CallableT && callable, std::true_type /*is_empty_arg*/) {
    assert(_state.load() != State::Initializing && _state.load() != State::Stopped);

    _activeWork.Increment();

    try {
        static_cast<SuperT &>(*this).AddWork(std::forward<CallableT>(callable));
    }
    catch(...) {
        _activeWork.Decrement();
        throw;
    }
}

template <typename SuperT>
template <typename CallableT>
void Details::ThreadPoolImpl<SuperT>::EnqueueWork(CallableT && callable, std::false_type /*is_empty_arg*/) {
    enqueue(
        [this, func=std::forward<CallableT>(callable)](void) {
            func(_state.load() == State::Started);
        }
    );
}

template <typename SuperT>
template <typename CallableT>
ThreadPoolFuture<typename TypeTraits::FunctionTraits<CallableT>::return_type> Details::ThreadPoolImpl<SuperT>::EnqueueTask(CallableT && callable, std::true_type /*is_empty_arg*/) {
    // ----------------------------------------------------------------------
    using ReturnType                        = typename TypeTraits::FunctionTraits<CallableT>::return_type;
    using Promise                           = std::promise<ReturnType>;
    using Future                            = std::future<ReturnType>;
    // ----------------------------------------------------------------------

    assert(_state.load() != State::Initializing && _state.load() != State::Stopped);

    // Note that the promise needs to be a shared_ptr to work around copy-related
    // issues when using the promise within the lambda below.
    std::shared_ptr<Promise>                pPromise(std::make_shared<Promise>());
    Future                                  future(pPromise->get_future());

    enqueue(
        [pPromise=std::move(pPromise), func=std::forward<CallableT>(callable)](void) {
            pPromise->set_value(func());
        }
    );

    return ThreadPoolFuture<ReturnType>(
        [this](void) {
            yield();
        },
        std::move(future)
    );
}

template <typename SuperT>
template <typename CallableT>
ThreadPoolFuture<typename TypeTraits::FunctionTraits<CallableT>::return_type> Details::ThreadPoolImpl<SuperT>::EnqueueTask(CallableT && callable, std::false_type /*is_empty_arg*/) {
    return enqueue(
        [this, func=std::forward<CallableT>(callable)](void) {
            return func(_state.load() == State::Started);
        }
    );
}

template <typename SuperT>
template <typename InputT, typename CallableT, typename IsSingleArgT>
void Details::ThreadPoolImpl<SuperT>::ParallelSingle(InputT const &input, CallableT && callable, std::true_type const &isVoidReturn, IsSingleArgT const &isSingleArg) {
    ParallelMultiple(
        &input,
        &input + 1,
        std::forward<CallableT>(callable),
        isVoidReturn,
        isSingleArg
    );
}

template <typename SuperT>
template <typename InputT, typename CallableT, typename IsSingleArgT>
typename TypeTraits::FunctionTraits<CallableT>::return_type Details::ThreadPoolImpl<SuperT>::ParallelSingle(InputT const &input, CallableT && callable, std::false_type const &isVoidReturn, IsSingleArgT const &isSingleArg) {
    // ----------------------------------------------------------------------
    using ReturnType                        = typename TypeTraits::FunctionTraits<CallableT>::return_type;
    using ReturnTypes                       = std::vector<ReturnType>;
    // ----------------------------------------------------------------------

    ReturnTypes                             results(
        ParallelMultiple(
            &input,
            &input + 1,
            std::forward<CallableT>(callable),
            isVoidReturn,
            isSingleArg
        )
    );

    assert(results.size() == 1);
    return std::move(results[0]);
}

template <typename SuperT>
template <typename InputIteratorT, typename CallableT>
void Details::ThreadPoolImpl<SuperT>::ParallelMultiple(InputIteratorT begin, InputIteratorT end, CallableT && callable, std::true_type /*is_void_result*/, std::true_type /*is_single_arg*/) {
    size_t const                            numElements(static_cast<size_t>(std::distance(begin, end)));

    if(numElements == 0)
        return;

    ThreadSafeCounter                       ctr(static_cast<ThreadSafeCounter::value_type>(numElements));

    while(begin != end) {
        enqueue(
            [&callable, &ctr, begin](void) {
                FINALLY([&ctr](void) { ctr.Decrement(); });
                callable(*begin);
            }
        );
        ++begin;
    }

    while(ctr.GetValue() != 0)
        yield();
}

template <typename SuperT>
template <typename InputIteratorT, typename CallableT>
void Details::ThreadPoolImpl<SuperT>::ParallelMultiple(InputIteratorT begin, InputIteratorT end, CallableT && callable, std::true_type /*is_void_result*/, std::false_type /*is_single_arg*/) {
    size_t const                            numElements(static_cast<size_t>(std::distance(begin, end)));

    if(numElements == 0)
        return;

    ThreadSafeCounter                       ctr(static_cast<ThreadSafeCounter::value_type>(numElements));

    while(begin != end) {
        enqueue(
            [&callable, &ctr, begin](bool isStarted) {
                FINALLY([&ctr](void) { ctr.Decrement(); });
                callable(isStarted, *begin);
            }
        );
        ++begin;
    }

    while(ctr.GetValue() != 0)
        yield();
}

template <typename SuperT>
template <typename InputIteratorT, typename CallableT>
std::vector<typename TypeTraits::FunctionTraits<CallableT>::return_type> Details::ThreadPoolImpl<SuperT>::ParallelMultiple(InputIteratorT begin, InputIteratorT end, CallableT && callable, std::false_type /*is_void_result*/, std::true_type /*is_single_arg*/) {
    // ----------------------------------------------------------------------
    using ReturnType                        = typename TypeTraits::FunctionTraits<CallableT>::return_type;
    using Future                            = ThreadPoolFuture<ReturnType>;
    using Futures                           = std::vector<Future>;
    // ----------------------------------------------------------------------

    size_t const                            numElements(static_cast<size_t>(std::distance(begin, end)));

    if(numElements == 0)
        return std::vector<ReturnType>();

    Futures                                 futures;

    futures.reserve(numElements);

    while(begin != end) {
        futures.emplace_back(
            enqueue(
                [&callable, begin](void) {
                    return callable(*begin);
                }
            )
        );
        ++begin;
    }

    std::vector<ReturnType>                 results;

    results.reserve(futures.size());

    for(auto & future : futures)
        results.emplace_back(std::move(future.get()));

    return results;
}

template <typename SuperT>
template <typename InputIteratorT, typename CallableT>
std::vector<typename TypeTraits::FunctionTraits<CallableT>::return_type> Details::ThreadPoolImpl<SuperT>::ParallelMultiple(InputIteratorT begin, InputIteratorT end, CallableT && callable, std::false_type /*is_void_result*/, std::false_type /*is_single_arg*/) {
    // ----------------------------------------------------------------------
    using ReturnType                        = typename TypeTraits::FunctionTraits<CallableT>::return_type;
    using Future                            = ThreadPoolFuture<ReturnType>;
    using Futures                           = std::vector<Future>;
    // ----------------------------------------------------------------------

    size_t const                            numElements(static_cast<size_t>(std::distance(begin, end)));

    if(numElements == 0)
        return std::vector<ReturnType>();

    Futures                                 futures;

    futures.reserve(numElements);

    while(begin != end) {
        futures.emplace_back(
            enqueue(
                [&callable, begin](bool isStarted) {
                    return callable(isStarted, *begin);
                }
            )
        );
        ++begin;
    }

    std::vector<ReturnType>                 results;

    results.reserve(futures.size());

    for(auto & future : futures)
        results.emplace_back(std::move(future.get()));

    return results;
}

template <typename SuperT>
inline bool Details::ThreadPoolImpl<SuperT>::DoWork(std::chrono::steady_clock::duration const &timeout) {
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
    catch(...) {
        _onExceptionCallback(_threadIndex);
    }

    return true;
}

} // namespace CommonHelpers
