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
    WorkTestImpl<CommonHelpers::SimpleThreadPool>(100, 4950);
}

TEST_CASE("Complex Work") {
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
    TaskTestImpl<CommonHelpers::SimpleThreadPool>(100);
}

TEST_CASE("Complex Task") {
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
    SECTION("Not set") {
        std::condition_variable             cv;

        auto const                          work(
            [&cv](bool isActive) {
                CHECK(isActive);

                cv.notify_one();
            }
        );

        CommonHelpers::SimpleThreadPool     pool(1);

        pool.enqueue(work);

        std::mutex                          m;
        std::unique_lock                    lock(m);

        cv.wait(lock);
    }

    SECTION("Set") {
        CommonHelpers::SimpleThreadPool     pool(1);

        pool.enqueue(
            [](void) {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(0.5s);
            }
        );

        // The pool should be stopping by the time this is executed
        // due to the sleep above.
        pool.enqueue(
            [](bool isActive) {
                CHECK(isActive == false);
            }
        );
    }
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
    bool                                    sawException(false);

    {
        CommonHelpers::SimpleThreadPool     pool(
            1,
            [&sawException](size_t threadIndex) {
                CHECK(threadIndex == 0);

                try {
                    throw;
                }
                catch(std::exception const &ex) {
                    CHECK(std::string(ex.what()) == "My custom exception");
                }

                sawException = true;
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

    CHECK(sawException);
    CHECK(value);
}

TEST_CASE("Reentrant Tasks") {
    int                                     value(0);

    {
        CommonHelpers::SimpleThreadPool     pool(1);

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

TEST_CASE("parallel work - single item - single arg") {
    CommonHelpers::SimpleThreadPool         pool(1);
    int                                     value(0);

    pool.parallel(10, [&value](int v) { value = v; });
    CHECK(value == 10);
}

TEST_CASE("parallel work - single item - multi arg") {
    CommonHelpers::SimpleThreadPool         pool(1);
    int                                     value(0);

    pool.parallel(10, [&value](bool, int v) { value = v; });
    CHECK(value == 10);
}

TEST_CASE("parallel task - single item - single arg") {
    CommonHelpers::SimpleThreadPool         pool(1);

    CHECK(pool.parallel(10, [](int v) { return v; }) == 10);
}

TEST_CASE("parallel task - single item - multi arg") {
    CommonHelpers::SimpleThreadPool         pool(1);

    CHECK(pool.parallel(10, [](bool, int v) { return v; }) == 10);
}

TEST_CASE("parallel work - vector - single arg") {
    CommonHelpers::SimpleThreadPool         pool(1);
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

TEST_CASE("parallel work - vector - multi arg") {
    CommonHelpers::SimpleThreadPool         pool(1);
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

TEST_CASE("parallel task - vector - single arg") {
    CommonHelpers::SimpleThreadPool         pool(1);

    CHECK(
        pool.parallel(
            std::vector<int>{10, 20, 30},
            [](int value) { return value; }
        ) == std::vector<int>{10, 20, 30}
    );
}

TEST_CASE("parallel task - vector - multi arg") {
    CommonHelpers::SimpleThreadPool         pool(1);

    CHECK(
        pool.parallel(
            std::vector<int>{10, 20, 30},
            [](bool, int value) { return value; }
        ) == std::vector<int>{10, 20, 30}
    );
}
