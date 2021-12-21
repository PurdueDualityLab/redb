//
// Created by charlie on 9/13/21.
//

#ifndef _BENCHMARKING_H
#define _BENCHMARKING_H

#include <chrono>
#include <functional>
#include <vector>
#include <unordered_set>
#include <string>

#include <db/regex_cluster_repository.h>
#include "query_results.h"

QueryResults time_cluster_query(const rereuse::db::RegexClusterRepository &repo,
                                                   const std::shared_ptr<rereuse::query::BaseClusterQuery> &query) {
    QueryResults results;
    auto start = std::chrono::high_resolution_clock::now();
    auto matches = repo.query(query, &results.skipped_clusters, &results.test_times, &results.query_times);
    auto end = std::chrono::high_resolution_clock::now();
    results.query_results = matches;
    results.total_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    return results;
}

#endif //_BENCHMARKING_H
