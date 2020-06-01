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
#include <optional>

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
        if(_isDone)
            return;

        {
            UniqueLock                      lock(_mutex); UNUSED(lock);

            _isDone = true;
        }

        _pushed.notify_all();

        {
            UniqueLock                      lock(_mutex);

            while(_activePops != 0)
                _exitedPop.wait(lock);
        }
    }

    void push(Functor functor) {
        assert(functor);

        if(_isDone)
            throw ThreadPoolQueueDoneException();

        {
            UniqueLock                      lock(_mutex); UNUSED(lock);

            _queue.emplace(std::move(functor));
        }

        _pushed.notify_one();
    }

    bool try_push(Functor const &functor) {
        assert(functor);

        if(_isDone)
            throw ThreadPoolQueueDoneException();

        {
            UniqueLock                      lock(_mutex, std::try_to_lock);

            if(!lock)
                return false;

            _queue.push(functor);
        }

        _pushed.notify_one();
        return true;
    }

    // Blocking
    Functor pop(void) {
        UniqueLock                          lock(_mutex);

        ++_activePops;

        FINALLY(
            [this](void) {
                assert(_activePops);
                --_activePops;

                _exitedPop.notify_one();
            }
        );

        while(_queue.empty() && _isDone == false)
            _pushed.wait(lock);

        if(_queue.empty())
            throw ThreadPoolQueueDoneException();

        Functor                             result(std::move(_queue.front()));

        _queue.pop();

        return result;
    }

    std::optional<Functor> try_pop(void) {
        UniqueLock                          lock(_mutex, std::try_to_lock);

        if(!lock || _queue.empty())
            return std::nullopt;

        Functor                             result(std::move(_queue.front()));

        _queue.pop();

        return std::move(result);
    }

private:
    // ----------------------------------------------------------------------
    // |  Private Types
    using Mutex                             = std::mutex;
    using UniqueLock                        = std::unique_lock<Mutex>;

    // ----------------------------------------------------------------------
    // |  Private Data
    mutable Mutex                           _mutex;
    std::queue<Functor>                     _queue;

    std::condition_variable                 _pushed;
    std::condition_variable                 _exitedPop;

    bool                                    _isDone;
    size_t                                  _activePops;
};

} // namespace Details

// ----------------------------------------------------------------------
// |
// |  SimpleThreadPool
// |
// ----------------------------------------------------------------------
SimpleThreadPool::SimpleThreadPool(size_t numThreads) :
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

std::function<void (void)> SimpleThreadPool::GetWork(size_t) {
    return _pQueue->pop();
}

void SimpleThreadPool::StopQueues(void) {
    _pQueue->Done();
}

// ----------------------------------------------------------------------
// |
// |  ComplexThreadPool
// |
// ----------------------------------------------------------------------
ComplexThreadPool::ComplexThreadPool(size_t numThreads, size_t workerIterations) :
    _numTries(
        [&numThreads, &workerIterations](void) {
            ENSURE_ARGUMENT(numThreads);
            ENSURE_ARGUMENT(workerIterations);

            return numThreads * workerIterations;
        }()
    ),
    _queues(
        [&numThreads](void) {
            ThreadPoolQueueUniquePtrs       queues;

            while(queues.size() < numThreads)
                queues.emplace_back(std::make_unique<Details::ThreadPoolQueue>());

            return queues;
        }()
    ),
    _currentEnqueueIndex(0)
{
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

std::function<void (void)> ComplexThreadPool::GetWork(size_t threadIndex) {
    for(size_t ctr = 0; ctr < _numTries; ++ctr) {
        Details::ThreadPoolQueue &                                          queue(*_queues[(threadIndex + ctr) % _queues.size()]);
        std::optional<std::function<void (void)>>                           func(queue.try_pop());

        if(func)
            return(std::move(*func));
    }

    // If here, we didn't find any work - wait for something
    return _queues[threadIndex]->pop();
}

void ComplexThreadPool::StopQueues(void) {
    for(auto &pQueue : _queues)
        pQueue->Done();
}

} // namespace CommonHelpers
