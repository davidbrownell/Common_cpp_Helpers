/////////////////////////////////////////////////////////////////////////
///
///  \file          ThreadPool_UnitTest.cpp
///  \brief         Unit test for ThreadPool.h
///
///  \author        David Brownell <db@DavidBrownell.com>
///  \date          2020-05-30 22:47:11
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
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#define CATCH_CONFIG_CONSOLE_WIDTH 200
#include "../ThreadPool.h"
#include <catch.hpp>

#include <optional>

static unsigned long const                  ITERATIONS = 1;

template <typename ThreadPoolT>
void WorkTestImpl(size_t numItems, std::optional<std::uint64_t> expected=std::nullopt) {
    std::atomic_uint64_t                    total(0);

    {
        auto const                          work(
            [&total](size_t ctr) {
                total += ctr;
            }
        );

        ThreadPoolT                         pool;

        for(size_t ctr = 0; ctr < numItems; ++ctr) {
            pool.enqueue(
                [&work, ctr](void) {
                    work(ctr);
                }
            );
        }
    }

    if(expected) {
        CHECK(total == *expected);
    }
}

TEST_CASE("Simple Work") {
    for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
        WorkTestImpl<CommonHelpers::SimpleThreadPool>(100, 4950);
}

TEST_CASE("Complex Work") {
    for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
        WorkTestImpl<CommonHelpers::ComplexThreadPool>(100, 4950);
}

#if (!defined DEBUG)
TEST_CASE("Simple Work Benchmark", "[Benchmark]") {
    BENCHMARK("100") { WorkTestImpl<CommonHelpers::SimpleThreadPool>(100); };
    BENCHMARK("1000") { WorkTestImpl<CommonHelpers::SimpleThreadPool>(1000); };
    BENCHMARK("5000") { WorkTestImpl<CommonHelpers::SimpleThreadPool>(5000); };
    BENCHMARK("10000") { WorkTestImpl<CommonHelpers::SimpleThreadPool>(10000); };
    BENCHMARK("20000") { WorkTestImpl<CommonHelpers::SimpleThreadPool>(20000); };
}

TEST_CASE("Complex Work Benchmark", "[Benchmark]") {
    BENCHMARK("100") { WorkTestImpl<CommonHelpers::ComplexThreadPool>(100); };
    BENCHMARK("1000") { WorkTestImpl<CommonHelpers::ComplexThreadPool>(1000); };
    BENCHMARK("5000") { WorkTestImpl<CommonHelpers::ComplexThreadPool>(5000); };
    BENCHMARK("10000") { WorkTestImpl<CommonHelpers::ComplexThreadPool>(10000); };
    BENCHMARK("20000") { WorkTestImpl<CommonHelpers::SimpleThreadPool>(20000); };
}
#endif

template <typename ThreadPoolT>
void TaskTestImpl(size_t numItems) {
    std::vector<CommonHelpers::ThreadPoolFuture<size_t>>                    futures;

    futures.reserve(numItems);

    {
        auto const                          task(
            [](size_t ctr) {
                return ctr;
            }
        );

        ThreadPoolT                         pool;

        while(futures.size() < numItems) {
            futures.emplace_back(
                pool.enqueue(
                    [&task, ctr=futures.size()](void) -> size_t {
                        return task(ctr);
                    }
                )
            );
        }
    }

    // Tasks
    size_t                              expectedValue(0);

    for(auto & future : futures) {
        CHECK(future.get() == expectedValue);
        ++expectedValue;
    }
}

TEST_CASE("Simple Task") {
    for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
        TaskTestImpl<CommonHelpers::SimpleThreadPool>(100);
}

TEST_CASE("Complex Task") {
    for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
        TaskTestImpl<CommonHelpers::ComplexThreadPool>(100);
}

#if (!defined DEBUG)
TEST_CASE("Simple Task Benchmark", "[Benchmark]") {
    BENCHMARK("100") { TaskTestImpl<CommonHelpers::SimpleThreadPool>(100); };
    BENCHMARK("1000") { TaskTestImpl<CommonHelpers::SimpleThreadPool>(1000); };
    BENCHMARK("5000") { TaskTestImpl<CommonHelpers::SimpleThreadPool>(5000); };
    BENCHMARK("10000") { TaskTestImpl<CommonHelpers::SimpleThreadPool>(10000); };
    BENCHMARK("20000") { TaskTestImpl<CommonHelpers::SimpleThreadPool>(20000); };
}

TEST_CASE("Complex Task Benchmark", "[Benchmark]") {
    BENCHMARK("100") { TaskTestImpl<CommonHelpers::ComplexThreadPool>(100); };
    BENCHMARK("1000") { TaskTestImpl<CommonHelpers::ComplexThreadPool>(1000); };
    BENCHMARK("5000") { TaskTestImpl<CommonHelpers::ComplexThreadPool>(5000); };
    BENCHMARK("10000") { TaskTestImpl<CommonHelpers::ComplexThreadPool>(10000); };
    BENCHMARK("20000") { TaskTestImpl<CommonHelpers::SimpleThreadPool>(20000); };
}
#endif

TEST_CASE("Shutdown flag") {
    SECTION("Is Active") {
        std::optional<bool>                 isActive;

        {
            CommonHelpers::SimpleThreadPool             pool(1);
            std::mutex                                  m;
            std::condition_variable                     cv;

            pool.enqueue(
                [&m, &cv, &isActive](bool isActiveParam) {
                    {
                        std::unique_lock<std::decay_t<decltype(m)>>         lock(m); UNUSED(lock);

                        isActive = isActiveParam;
                    }

                    cv.notify_one();
                }
            );

            std::unique_lock<decltype(m)>   lock(m);

            cv.wait(
                lock,
                [&isActive](void) { return static_cast<bool>(isActive); }
            );
        }

        CHECK((isActive && *isActive));
    }

    // I'm not quite sure how to test this; I know that the following code
    // does not work.
#if 0
    SECTION("Is Not Active") {
        std::condition_variable             cv;
        std::optional<bool>                 isActive;

        {
            CommonHelpers::SimpleThreadPool             pool(1);

            pool.enqueue(
                [](void) {
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(0.5s);
                }
            );

            // The pool should be stopping by the time this is executed
            // due to the sleep above.
            pool.enqueue(
                [&cv, &isActive](bool isActiveParam) {
                    isActive = isActiveParam;
                    cv.notify_one();
                }
            );
        }

        std::mutex                          m;
        std::unique_lock<decltype(m)>       lock(m);

        cv.wait(lock);
        CHECK((isActive && isActive == false));
    }
    #endif
}

TEST_CASE("Default Exception") {
    bool                                    value(false);

    std::cerr << "The following exception description is expected...\n\n\n";

    {
        CommonHelpers::SimpleThreadPool     pool(1);

        pool.enqueue(
            [](void) {
                throw std::runtime_error("This is an exception handled by the default processor; it will not kill the thread");
            }
        );

        pool.enqueue(
            [&value](void) {
                value = true;
            }
        );
    }

    std::cerr << "\n...no more exception descriptions are expected.\n\n";

    CHECK(value);
}

TEST_CASE("Custom Exception Handler") {
    bool                                    value(false);
    std::optional<size_t>                   exceptionThreadIndex;
    std::optional<std::string>              exceptionDesc;

    {
        CommonHelpers::SimpleThreadPool     pool(
            1,
            [&exceptionThreadIndex, &exceptionDesc](size_t threadIndex) {
                exceptionThreadIndex = threadIndex;

                try {
                    throw;
                }
                catch(std::exception const &ex) {
                    exceptionDesc = ex.what();
                }
            }
        );

        pool.enqueue(
            [](void) {
                throw std::logic_error("My custom exception");
            }
        );

        pool.enqueue(
            [&value](void) {
                value = true;
            }
        );
    }

    CHECK((exceptionThreadIndex && *exceptionThreadIndex == 0));
    CHECK((exceptionDesc && *exceptionDesc == "My custom exception"));
}

template <typename PoolT>
void ReentrantTasksTest(size_t numThreads) {
    int                                     value(0);

    {
        PoolT                               pool(numThreads);

        pool.enqueue(
            [&value, &pool](void) {
                value = pool.enqueue(
                    [](void) { return 10; }
                ).get();
            }
        );
    }

    CHECK(value == 10);
}

TEST_CASE("Reentrant Tasks") {
    SECTION("SimpleThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ReentrantTasksTest<CommonHelpers::SimpleThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ReentrantTasksTest<CommonHelpers::SimpleThreadPool>(5);
    }

    SECTION("ComplexThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ReentrantTasksTest<CommonHelpers::ComplexThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ReentrantTasksTest<CommonHelpers::ComplexThreadPool>(5);
    }
}

template <typename PoolT>
void ParallelWorkSingleItemSingleArgTest(size_t numThreads) {
    PoolT                                   pool(numThreads);
    int                                     value(0);

    pool.parallel(10, [&value](int v) { value = v; });
    CHECK(value == 10);
}

TEST_CASE("parallel work - single item - single arg") {
    SECTION("SimpleThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkSingleItemSingleArgTest<CommonHelpers::SimpleThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkSingleItemSingleArgTest<CommonHelpers::SimpleThreadPool>(5);
    }

    SECTION("ComplexThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkSingleItemSingleArgTest<CommonHelpers::ComplexThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkSingleItemSingleArgTest<CommonHelpers::ComplexThreadPool>(5);
    }
}

template <typename PoolT>
void ParallelWorkSingleItemMultiArgTest(size_t numThreads) {
    PoolT                                   pool(numThreads);
    int                                     value(0);

    pool.parallel(10, [&value](bool, int v) { value = v; });
    CHECK(value == 10);
}

TEST_CASE("parallel work - single item - multi arg") {
    SECTION("SimpleThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkSingleItemMultiArgTest<CommonHelpers::SimpleThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkSingleItemMultiArgTest<CommonHelpers::SimpleThreadPool>(5);
    }

    SECTION("ComplexThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkSingleItemMultiArgTest<CommonHelpers::ComplexThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkSingleItemMultiArgTest<CommonHelpers::ComplexThreadPool>(5);
    }
}

template <typename PoolT>
void ParallelTaskSingleItemSingleArgTest(size_t numThreads) {
    PoolT                                   pool(numThreads);

    CHECK(pool.parallel(10, [](int v) { return v; }) == 10);
}

TEST_CASE("parallel task - single item - single arg") {
    SECTION("SimpleThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskSingleItemSingleArgTest<CommonHelpers::SimpleThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskSingleItemSingleArgTest<CommonHelpers::SimpleThreadPool>(5);
    }

    SECTION("ComplexThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskSingleItemSingleArgTest<CommonHelpers::ComplexThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskSingleItemSingleArgTest<CommonHelpers::ComplexThreadPool>(5);
    }
}

template <typename PoolT>
void ParallelTaskSingleItemMultiArgTest(size_t numThreads) {
    PoolT                                   pool(numThreads);

    CHECK(pool.parallel(10, [](bool, int v) { return v; }) == 10);
}

TEST_CASE("parallel task - single item - multi arg") {
    SECTION("SimpleThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskSingleItemMultiArgTest<CommonHelpers::SimpleThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskSingleItemMultiArgTest<CommonHelpers::SimpleThreadPool>(5);
    }

    SECTION("ComplexThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskSingleItemMultiArgTest<CommonHelpers::ComplexThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskSingleItemMultiArgTest<CommonHelpers::ComplexThreadPool>(5);
    }
}

template <typename PoolT>
void ParallelWorkVectorSingleArgTest(size_t numThreads) {
    PoolT                                   pool(numThreads);
    std::vector<int>                        v;

    v.resize(3);

    pool.parallel(
        std::vector<int>{10, 20, 30},
        [&v](int value) {
            size_t                          index(
                [&value](void) -> size_t {
                    if(value == 10)
                        return 0;
                    if(value == 20)
                        return 1;
                    if(value == 30)
                        return 2;

                    return 3;
                }()
            );

            v[index] = value;
        }
    );
    CHECK(v == std::vector<int>{10, 20, 30});
}

TEST_CASE("parallel work - vector - single arg") {
    SECTION("SimpleThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkVectorSingleArgTest<CommonHelpers::SimpleThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkVectorSingleArgTest<CommonHelpers::SimpleThreadPool>(5);
    }

    SECTION("ComplexThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkVectorSingleArgTest<CommonHelpers::ComplexThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkVectorSingleArgTest<CommonHelpers::ComplexThreadPool>(5);
    }
}

template <typename PoolT>
void ParallelWorkVectorMultiArgTest(size_t numThreads) {
    PoolT                                   pool(numThreads);
    std::vector<int>                        v;

    v.resize(3);

    pool.parallel(
        std::vector<int>{10, 20, 30},
        [&v](bool, int value) {
            size_t                          index(
                [&value](void) -> size_t {
                    if(value == 10)
                        return 0;
                    if(value == 20)
                        return 1;
                    if(value == 30)
                        return 2;

                    return 3;
                }()
            );

            v[index] = value;
        }
    );
    CHECK(v == std::vector<int>{10, 20, 30});
}

TEST_CASE("parallel work - vector - multi arg") {
    SECTION("SimpleThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkVectorMultiArgTest<CommonHelpers::SimpleThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkVectorMultiArgTest<CommonHelpers::SimpleThreadPool>(5);
    }

    SECTION("ComplexThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkVectorMultiArgTest<CommonHelpers::ComplexThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelWorkVectorMultiArgTest<CommonHelpers::ComplexThreadPool>(5);
    }
}

template <typename PoolT>
void ParallelTaskVectorSingleArgTest(size_t numThreads) {
    PoolT                                   pool(numThreads);

    CHECK(
        pool.parallel(
            std::vector<int>{10, 20, 30},
            [](int value) { return value; }
        ) == std::vector<int>{10, 20, 30}
    );
}

TEST_CASE("parallel task - vector - single arg") {
    SECTION("SimpleThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskVectorSingleArgTest<CommonHelpers::SimpleThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskVectorSingleArgTest<CommonHelpers::SimpleThreadPool>(5);
    }

    SECTION("ComplexThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskVectorSingleArgTest<CommonHelpers::ComplexThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskVectorSingleArgTest<CommonHelpers::ComplexThreadPool>(5);
    }
}

template <typename PoolT>
void ParallelTaskVectorMultiArgTest(size_t numThreads) {
    PoolT                                   pool(numThreads);

    CHECK(
        pool.parallel(
            std::vector<int>{10, 20, 30},
            [](bool, int value) { return value; }
        ) == std::vector<int>{10, 20, 30}
    );
}

TEST_CASE("parallel task - vector - multi arg") {
    SECTION("SimpleThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskVectorMultiArgTest<CommonHelpers::SimpleThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskVectorMultiArgTest<CommonHelpers::SimpleThreadPool>(5);
    }

    SECTION("ComplexThreadPool") {
        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskVectorMultiArgTest<CommonHelpers::ComplexThreadPool>(1);

        for(unsigned long ctr = 0; ctr < ITERATIONS; ctr++)
            ParallelTaskVectorMultiArgTest<CommonHelpers::ComplexThreadPool>(5);
    }
}
