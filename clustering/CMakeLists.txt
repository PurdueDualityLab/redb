
add_executable(replication
        replication_main.cc
        rex_wrapper.cc rex_wrapper.h
        similarity_table.cc similarity_table.h
        base_similarity_scorer.cc base_similarity_scorer.h
        rex_similarity_scorer.cc rex_similarity_scorer.h
        ThreadPool.h
        mcl_wrapper.cc mcl_wrapper.h
        cluster_set.cc cluster_set.h egret_similarity_scorer.cc egret_similarity_scorer.h)
target_link_libraries(replication PRIVATE nlohmann_json::nlohmann_json re2::re2)