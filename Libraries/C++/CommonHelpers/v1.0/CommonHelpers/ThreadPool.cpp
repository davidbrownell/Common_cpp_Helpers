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
    NumThreads(numThreads),
    _pQueue(std::make_unique<FunctorQueue>())
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
    assert(_pQueue);
    _pQueue->Push(std::move(functor));
}

std::function<void (void)> SimpleThreadPool::GetWork(size_t, bool isBlocking) {
    assert(_pQueue);

    if(isBlocking)
        return _pQueue->Pop(QueuePopType::Blocking);

    return _pQueue->TryPop();
}

void SimpleThreadPool::StopQueues(void) {
    _pQueue->Stop();
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
    NumThreads(numThreads),
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

            FunctorQueueUniquePtrs          queues;

            while(queues.size() < numThreads)
                queues.emplace_back(std::make_unique<FunctorQueue>());

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
    assert(_queues.empty() == false);

    unsigned int                            index(_currentEnqueueIndex++);

    // Attempt to quickly push the work
    for(size_t ctr = 0; ctr < _numTries; ++ctr) {
        FunctorQueue &                      queue(*_queues[(index + ctr) % _queues.size()]);

        if(queue.TryPush(functor))
            return;
    }

    // If here, do it the hard way
    FunctorQueue &                          queue(*_queues[index % _queues.size()]);

    queue.Push(std::move(functor));
}

std::function<void (void)> ComplexThreadPool::GetWork(size_t threadIndex, bool isBlocking) {
    assert(_queues.empty() == false);

    for(size_t ctr = 0; ctr < _numTries; ++ctr) {
        FunctorQueue &                      queue(*_queues[(threadIndex + ctr) % _queues.size()]);
        FunctorQueue::optional_value_type   func(queue.TryPop());

        if(func)
            return func;
    }

    if(isBlocking == false)
        return std::function<void (void)>();

    // If here, we didn't find any work - wait for something
    return _queues[threadIndex]->Pop(QueuePopType::Blocking);
}

void ComplexThreadPool::StopQueues(void) {
    for(auto &pQueue : _queues)
        pQueue->Stop();
}

} // namespace CommonHelpers
