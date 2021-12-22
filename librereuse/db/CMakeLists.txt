
# Make a source group that contains all of this directories sources
add_library(librereuse-db
        cluster.cc cluster.h
        pattern_reader.cc pattern_reader.h
        regex_cluster_repository.cc regex_cluster_repository.h
        regex_repository.cc regex_repository.h)
target_link_libraries(librereuse-db PRIVATE nlohmann_json::nlohmann_json)
