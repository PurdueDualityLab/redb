
# Define clustering executable - actually clusters stuff
add_executable(clustering
        clustering_main.cc
        rex_wrapper.cc rex_wrapper.h
        similarity_table/similarity_table.cc similarity_table/similarity_table.h
        similarity_table/base_similarity_scorer.cc similarity_table/base_similarity_scorer.h
        similarity_table/rex_similarity_scorer.cc similarity_table/rex_similarity_scorer.h
        ThreadPool.h
        mcl_wrapper.cc mcl_wrapper.h
        cluster_set.cc cluster_set.h
        similarity_table/egret_similarity_scorer.cc
        similarity_table/egret_similarity_scorer.h
        program_options.cc program_options.h
        similarity_table/table_scorer.cc similarity_table/table_scorer.h)
target_link_libraries(clustering PRIVATE librereuse nlohmann_json::nlohmann_json re2::re2 egret)

# define clustering evaluation executable - measures properties of a cluster
add_subdirectory(evaluation)
