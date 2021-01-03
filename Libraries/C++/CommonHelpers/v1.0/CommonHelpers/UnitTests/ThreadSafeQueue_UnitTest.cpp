/////////////////////////////////////////////////////////////////////////
///
///  \file          ThreadSafeQueue_UnitTest.cpp
///  \brief         Unit test for ThreadSafeQueue
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-06-08 08:47:13
///
///  \note
///
///  \bug
///
/////////////////////////////////////////////////////////////////////////
///
///  \attention
///  Copyright David Brownell 2020-21
///  Distributed under the Boost Software License, Version 1.0. See
///  accompanying file LICENSE_1_0.txt or copy at
///  http://www.boost.org/LICENSE_1_0.txt.
///
/////////////////////////////////////////////////////////////////////////
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_CONSOLE_WIDTH 200
#include "../ThreadSafeQueue.h"
#include <catch.hpp>

#include "../ThreadSafeCounter.h"

#include <thread>

using Queue                                 = CommonHelpers::ThreadSafeQueue<int>;

TEST_CASE("Push and Pop") {
    Queue                                   queue;

    CHECK(queue.empty());
    CHECK(queue.size() == 0);

    queue.Push(10);

    CHECK(queue.empty() == false);
    CHECK(queue.size() == 1);

    CHECK(queue.Pop(Queue::PopType::Blocking) == 10);

    CHECK(queue.empty());
    CHECK(queue.size() == 0);
}

TEST_CASE("Try Pop") {
    Queue                                   queue;

    queue.Push(10);

    std::optional<int>                      value(queue.TryPop());

    REQUIRE(value);
    CHECK(*value == 10);

    value = queue.TryPop();
    CHECK(!value);
}

TEST_CASE("Multi Push") {
    Queue                                   queue;

    for(int ctr = 0; ctr < 5; ++ctr)
        queue.Push(ctr);

    for(int ctr = 0; ctr < 5; ++ctr)
        CHECK(queue.Pop(Queue::PopType::Blocking) == ctr);
}

template <typename QueueT>
std::vector<std::thread> InitThreads(size_t numThreads, QueueT &queue) {
    std::vector<std::thread>                threads;
    CommonHelpers::ThreadSafeCounter        initThreads(static_cast<CommonHelpers::ThreadSafeCounter::value_type>(numThreads));

    while(threads.size() < numThreads) {
        threads.emplace_back(
            [&queue, &initThreads](void) {
                initThreads.Decrement();

                for(;;) {
                    try {
                        queue.Pop(QueueT::PopType::Blocking)();
                    }
                    catch(CommonHelpers::ThreadSafeQueueStoppedException const &) {
                        break;
                    }
                }
            }
        );
    }

    initThreads.wait_value(0);

    return threads;
}

void TestEnqueueFirst(size_t numValues, size_t numThreads) {
    // ----------------------------------------------------------------------
    using FunctorQueue                      = CommonHelpers::ThreadSafeQueue<std::function<void (void)>>;
    // ----------------------------------------------------------------------

    std::vector<size_t>                     values(numValues);

    {
        FunctorQueue                        queue;
        std::vector<std::thread>            threads;

        {
            CommonHelpers::ThreadSafeCounter            activeTasks(static_cast<CommonHelpers::ThreadSafeCounter::value_type>(numValues));

            for(size_t value = 0; value < numValues; ++value) {
                queue.Push(
                    [&values, &activeTasks, value](void) {
                        values[value] = value;
                        activeTasks.Decrement();
                    }
                );
            }

            threads = InitThreads(numThreads, queue);

            activeTasks.wait_value(0);

            CHECK(queue.IsStopped() == false);
            queue.Stop();
            CHECK(queue.IsStopped());
        }

        for(auto & thread : threads)
            thread.join();
    }

    for(size_t value = 0; value < numValues; ++value) {
        CHECK(values[value] == value);
    }
}

TEST_CASE("Enqueue First - 100.1") {
    TestEnqueueFirst(100, 1);
}

TEST_CASE("Enqueue First - 100.2") {
    TestEnqueueFirst(100, 2);
}

TEST_CASE("Enqueue First - 100.10") {
    TestEnqueueFirst(100, 10);
}

TEST_CASE("Enqueue First - 100.100") {
    TestEnqueueFirst(100, 100);
}

TEST_CASE("Enqueue First - 100.101") {
    TestEnqueueFirst(100, 101);
}

void TestThreadFirst(size_t numValues, size_t numThreads) {
    // ----------------------------------------------------------------------
    using FunctorQueue                      = CommonHelpers::ThreadSafeQueue<std::function<void (void)>>;
    // ----------------------------------------------------------------------

    std::vector<size_t>                     values(numValues);

    {
        FunctorQueue                        queue;
        std::vector<std::thread>            threads;

        {
            threads = InitThreads(numThreads, queue);

            CommonHelpers::ThreadSafeCounter            activeTasks(static_cast<CommonHelpers::ThreadSafeCounter::value_type>(numValues));

            for(size_t value = 0; value < numValues; ++value) {
                queue.Push(
                    [&values, &activeTasks, value](void) {
                        values[value] = value;
                        activeTasks.Decrement();
                    }
                );
            }

            activeTasks.wait_value(0);

            CHECK(queue.IsStopped() == false);
            queue.Stop();
            CHECK(queue.IsStopped());
        }

        for(auto & thread : threads)
            thread.join();
    }

    for(size_t value = 0; value < numValues; ++value) {
        CHECK(values[value] == value);
    }
}

TEST_CASE("Thread First - 100.1") {
    TestThreadFirst(100, 1);
}

TEST_CASE("Thread First - 100.2") {
    TestThreadFirst(100, 2);
}

TEST_CASE("Thread First - 100.10") {
    TestThreadFirst(100, 10);
}

TEST_CASE("Thread First - 100.100") {
    TestThreadFirst(100, 100);
}

TEST_CASE("Thread First - 100.101") {
    TestThreadFirst(100, 101);
}
