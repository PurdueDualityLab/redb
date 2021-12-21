//
// Created by charlie on 11/22/21.
//

#ifndef _QUERYRESULTS_H
#define _QUERYRESULTS_H

#include <vector>
#include <chrono>
#include <optional>
#include <utility>
#include <string>
#include <unordered_set>

struct QueryResults {
public:
    QueryResults()
    : total_time(0),
    skipped_clusters(0)
    {}

    std::chrono::microseconds total_time;
    std::vector<std::chrono::microseconds> test_times;
    std::vector<std::chrono::microseconds> query_times;
    int skipped_clusters;
    std::unordered_set<std::string> query_results;
};

#endif //_QUERYRESULTS_H
