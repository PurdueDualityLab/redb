
enable_testing()

add_executable(
        cluster_match_query_test
        cluster_match_query_test.cc
)
target_link_libraries(
        cluster_match_query_test
        gtest_main
        librereuse
)

add_executable(
        fuzzy_cluster_query_test
        fuzzy_cluster_query_test.cc
)
target_link_libraries(
        fuzzy_cluster_query_test
        gtest_main
        librereuse
)

add_executable(
        parallel_regex_cluster_repository_test
        parallel_regex_cluster_repository_test.cc
)
target_link_libraries(
        parallel_regex_cluster_repository_test
        gtest_main
        librereuse
)

# add_executable(
#         stats_test
#         stats_test.cc
# )
# target_link_libraries(
#         cluster_match_query_test
#         gtest_main
#         librereuse
# )

include(GoogleTest)
gtest_discover_tests(
        cluster_match_query_test
        fuzzy_cluster_query_test
        stats_test
)
