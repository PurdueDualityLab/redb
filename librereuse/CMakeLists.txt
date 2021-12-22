
# Setup libreuse
add_subdirectory(db)
add_subdirectory(query)

add_library(librereuse
        # db sources
        db/cluster.cc db/cluster.h
        db/pattern_reader.cc db/pattern_reader.h
        db/regex_cluster_repository.cc db/regex_cluster_repository.h
        db/regex_repository.cc db/regex_repository.h
        db/parallel_regex_cluster_repository.cc db/parallel_regex_cluster_repository.h
        # Query sources
        query/abstract_score.h
        query/base_cluster_query.h
        query/base_regex_query.cc query/base_regex_query.h
        query/cluster_match_query.cc query/cluster_match_query.h
        query/fuzzy_cluster_query.cc query/fuzzy_cluster_query.h
        query/match_query.cc query/match_query.h
        query/miss_score.cc query/miss_score.h
        # Util sources
        util/stats.h
        util/is_iterable.h)
target_link_libraries(librereuse PRIVATE re2::re2 nlohmann_json::nlohmann_json)
# target_include_directories(librereuse PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(librereuse PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/librereuse>)
