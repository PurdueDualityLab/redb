
# Add source group of all query sources
add_library(librereuse-query
        abstract_score.h
        base_cluster_query.h
        base_regex_query.cc base_regex_query.h
        cluster_match_query.cc cluster_match_query.h
        fuzzy_cluster_query.cc fuzzy_cluster_query.h
        match_query.cc match_query.h
        miss_score.cc miss_score.h)
