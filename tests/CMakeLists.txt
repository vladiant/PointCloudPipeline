add_executable(pcp_tests
    core_tests.cpp
    io_tests.cpp
    index_tests.cpp
    query_correctness_tests.cpp
    bench_tests.cpp
    qa_independent_tests.cpp
)
target_link_libraries(pcp_tests PRIVATE
    pcp_core pcp_io pcp_index pcp_query pcp_bench GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(pcp_tests)
