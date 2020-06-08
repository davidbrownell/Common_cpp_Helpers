/////////////////////////////////////////////////////////////////////////
///
///  \file          ThreadPool.cpp
///  \brief         See ThreadPool.h
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-05-30 22:49:48
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
#include "ThreadPool.h"
#include "Finally.h"

#include <queue>

namespace CommonHelpers {
namespace Details {

// ----------------------------------------------------------------------
// |
// |  ThreadPoolQueue
// |
// ----------------------------------------------------------------------
class ThreadPoolQueue {
public:
    // ----------------------------------------------------------------------
    // |  Public Types
    using Functor                           = std::function<void (void)>;

    // ----------------------------------------------------------------------
    // |  Public Methods
    ThreadPoolQueue(void) :
        _isDone(false),
        _activePops(0)
    {}

    ~ThreadPoolQueue(void) {
        Done();
    }

    void Done(void) {
        {
            std::scoped_lock                lock(_mutex); UNUSED(lock);

            if(_isDone)
                return;

            _isDone = true;
        }

        _cvQueue.notify_all();
        _activePops.wait_until(0);
    }

    void push(Functor functor) {
        assert(functor);

        {
            std::unique_lock                lock(_mutex); UNUSED(lock);

            if(_isDone)
                throw ThreadPoolQueueDoneException();

            _queue.emplace(std::move(functor));
        }

        _cvQueue.notify_one();
    }

    bool try_push(Functor const &functor) {
        assert(functor);

        {
            std::unique_lock                lock(_mutex, std::try_to_lock);

            if(!lock)
                return false;

            if(_isDone)
                throw ThreadPoolQueueDoneException();

            _queue.push(functor);
        }

        _cvQueue.notify_one();
        return true;
    }

    // Blocking
    Functor pop(std::chrono::steady_clock::duration duration) {
        Functor                             result;

        _activePops.Increment();
        FINALLY([this](void) { _activePops.Decrement(); });

        {
            std::unique_lock                lock(_mutex);

            if(_isDone)
                throw ThreadPoolQueueDoneException();

            if(
                _cvQueue.wait_for(
                    lock,
                    duration,
                    [this](void) {
                        return _queue.empty() == false || _isDone;
                    }
                )
            ) {
                if(_queue.empty() && _isDone)
                    throw ThreadPoolQueueDoneException();

                result = std::move(_queue.front());
                _queue.pop();
            }
        }

        return result;
    }

    Functor try_pop(void) {
        Functor                             result;

        {
            std::unique_lock                lock(_mutex, std::try_to_lock);

            if(lock) {
                if(_isDone)
                    throw ThreadPoolQueueDoneException();

                if(_queue.empty() == false) {
                    result = std::move(_queue.front());
                    _queue.pop();
                }
            }
        }

        return result;
    }

private:
    // ----------------------------------------------------------------------
    // |  Private Data
    mutable std::mutex                      _mutex;
    bool                                    _isDone;
    std::queue<Functor>                     _queue;

    std::condition_variable                 _cvQueue;
    ThreadSafeCounter                       _activePops;
};

} // namespace Details

// ----------------------------------------------------------------------
// |
// |  SimpleThreadPool
// |
// ----------------------------------------------------------------------
SimpleThreadPool::SimpleThreadPool(
    size_t numThreads,
    std::optional<OnExceptionCallback> onExceptionCallback
) :
    BaseType(std::move(onExceptionCallback)),
    _pQueue(std::make_unique<Details::ThreadPoolQueue>())
{
    ENSURE_ARGUMENT(numThreads);
    BaseType::Start(numThreads);
}

SimpleThreadPool::~SimpleThreadPool(void) {
    BaseType::Stop();
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
void SimpleThreadPool::AddWork(std::function<void (void)> functor) {
    assert(functor);
    _pQueue->push(std::move(functor));
}

std::function<void (void)> SimpleThreadPool::GetWork(size_t, std::chrono::steady_clock::duration duration) {
    return _pQueue->pop(duration);
}

void SimpleThreadPool::StopQueues(void) {
    _pQueue->Done();
}

// ----------------------------------------------------------------------
// |
// |  ComplexThreadPool
// |
// ----------------------------------------------------------------------
ComplexThreadPool::ComplexThreadPool(
    size_t numThreads,
    size_t workerIterations,
    std::optional<OnExceptionCallback> onExceptionCallback
) :
    BaseType(std::move(onExceptionCallback)),
    _numTries(
        [&numThreads, &workerIterations](void) {
            ENSURE_ARGUMENT(numThreads);
            ENSURE_ARGUMENT(workerIterations);

            return numThreads * workerIterations;
        }()
    ),
    _queues(
        [&numThreads](void) {
            assert(numThreads);

            ThreadPoolQueueUniquePtrs       queues;

            while(queues.size() < numThreads)
                queues.emplace_back(std::make_unique<Details::ThreadPoolQueue>());

            return queues;
        }()
    ),
    _currentEnqueueIndex(0)
{
    assert(numThreads);
    BaseType::Start(numThreads);
}

ComplexThreadPool::~ComplexThreadPool(void) {
    BaseType::Stop();
}

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
void ComplexThreadPool::AddWork(std::function<void (void)> functor) {
    assert(functor);

    unsigned int                            index(_currentEnqueueIndex++);

    // Attempt to quickly push the work
    for(size_t ctr = 0; ctr < _numTries; ++ctr) {
        Details::ThreadPoolQueue &          queue(*_queues[(index + ctr) % _queues.size()]);

        if(queue.try_push(functor))
            return;
    }

    // If here, do it the hard way
    Details::ThreadPoolQueue &              queue(*_queues[index % _queues.size()]);

    queue.push(std::move(functor));
}

std::function<void (void)> ComplexThreadPool::GetWork(size_t threadIndex, std::chrono::steady_clock::duration duration) {
    for(size_t ctr = 0; ctr < _numTries; ++ctr) {
        Details::ThreadPoolQueue &          queue(*_queues[(threadIndex + ctr) % _queues.size()]);
        Details::ThreadPoolQueue::Functor   func(queue.try_pop());

        if(func)
            return func;
    }

    // If here, we didn't find any work - wait for something
    return _queues[threadIndex]->pop(duration);
}

void ComplexThreadPool::StopQueues(void) {
    for(auto &pQueue : _queues)
        pQueue->Done();
}

} // namespace CommonHelpers
