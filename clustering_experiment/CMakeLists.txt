
add_executable(
        clustering_exp
        main.cc
        benchmarking.h
        # cluster_experiment.cc cluster_experiment.h
        query_results.h
        semantic_clustering.cc semantic_clustering.h
        # set_comparison.cc set_comparison.h
        )

target_link_libraries(clustering_exp PRIVATE librereuse nlohmann_json::nlohmann_json)